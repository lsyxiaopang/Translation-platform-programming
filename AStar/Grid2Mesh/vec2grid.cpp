#include"vec2grid.h"

Mesh::Mesh(float xstart, float xend,
	float ystart, float yend,
	int xn, int yn)
{
	this->xstart = xstart;
	this->ystart = ystart;
	this->xend = xend;
	this->yend = yend;
	this->xn = xn;
	this->yn = yn;
	this->xstep = (xend - xstart) / xn;
	this->ystep = (yend - ystart) / xn;
}

inline Vector2i Mesh::get_mesh_coor(Vector2f coor)
{
	float xc = coor[0];
	float yc = coor[1];
	int x = int((xc - this->xstart) / this->xstep);
	int y = int((yc - this->ystart) / this->ystep);
	return Vector2i(x, y);
}

inline Vector2f Mesh::get_real_coor(Vector2i coor)
{
	int xc = coor[0];
	int yc = coor[1];
	float x = (coor[0]+0.5) * this->xstep;//这里加上0.5是因为之前是直接向下取整
	float y = (coor[1]+0.5) * this->ystep;
	return Vector2f(x, y);
}

MatrixXi Mesh::vec2mesh(dotveclist& vecinp)
{
	this->wall_mat = MatrixXi::Ones(this->yn, this->xn);
	//需要注意这里输入的vecinp并没有经过特殊处理,其首尾并不相连
	for (int i = 0; i < vecinp.rows()-1; i++)
	{
		this->line(this->get_mesh_coor(vecinp(i, seq(0, 1))),this->get_mesh_coor(vecinp(i + 1, seq(0, 1))));
	}
	this->line(this->get_mesh_coor(vecinp(Eigen::last, seq(0, 1))), this->get_mesh_coor(vecinp(0, seq(0, 1))));
	return this->wall_mat;
}

void Mesh::line(Vector2i coor0, Vector2i coor1) 
{
	int x0 = coor0[0];
	int x1 = coor1[0];
	int y0 = coor0[1];
	int y1 = coor1[1];
	bool steep = false;
	if (std::abs(x1 - x0) < std::abs(y1 - y0)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int deltaY = std::abs(dy << 1);
	int middle = dx;
	int y = y0;
	for (int x = x0; x <= x1; ++x) {
		if (steep) {
			this->wall_mat(y, x) = 0;
		}
		else {
			this->wall_mat(x, y) = 0;
		}
		deltaY += std::abs(dy << 1);
		if (deltaY >= middle) {
			y += (y1 > y0 ? 1 : -1);
			middle += std::abs(dx << 1);
		}
	}
}

dotveclist Mesh::find_route(Vector2f& start, Vector2f& end)
{
	Vector2i startp = this->get_mesh_coor(start);
	Vector2i endp = this->get_mesh_coor(end);
	vector<dot> route=AStarSearch(this->wall_mat, this->xn, this->yn,
		startp[0], startp[1], endp[0], endp[1]);
	reverse(route.begin(), route.end());//这里是因为路径是反过来的
	int dx = 0;
	int dy = 0;
	dotveclist reduced_route(0,2);
	for (auto d = route.begin(); d < route.end() - 1; d++)
	{
		bool cflag = false;
		if (((d + 1)->x) - ((d)->x) != dx)
		{
			dx = ((d + 1)->x) - ((d)->x);
			cflag = true;
		}
		if (((d + 1)->y - ((d)->y) != dy))
		{
			dy = ((d + 1)->y) - ((d)->y);
			cflag = true;
		}
		if (cflag)
		{
			Vector2f rp = get_real_coor(Vector2i(d->x, d->y));
			reduced_route.conservativeResize(reduced_route.rows() + 1,Eigen::NoChange);
			reduced_route.row(reduced_route.rows() - 1) << rp[0],rp[1];
		}
	}
	reduced_route.conservativeResize(reduced_route.rows() + 1, reduced_route.cols());
	Vector2f rl = this->get_real_coor(Vector2i((route.end()-1)->x, (route.end()-1)->y));
	reduced_route.row(reduced_route.rows() - 1) << rl[0],rl[1];
    reduced_route.conservativeResize(reduced_route.rows() + 1, reduced_route.cols());
	reduced_route.row(reduced_route.rows() - 1) << end[0],end[1];
	return reduced_route;
}