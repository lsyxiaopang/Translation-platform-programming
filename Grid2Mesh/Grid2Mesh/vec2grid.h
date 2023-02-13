#pragma once
#include"a_star.hpp"

using Eigen::MatrixXd;
using Eigen::MatrixXi;
using Eigen::MatrixXf;
using Eigen::Vector2i;//用来表示网格内坐标
using Eigen::Vector2f;
using Eigen::Index;
using Eigen::seq;

typedef Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> MatrixBool;
typedef Eigen::Matrix<float, Eigen::Dynamic, 2> dotveclist;

using namespace::std;

class Mesh
{
public:
	MatrixXi vec2mesh(dotveclist& vecinp);
	Mesh(float xstart, float xend,
			float ystart, float yend,
			int xn, int yn);
	dotveclist find_route(Vector2f& start, Vector2f& end);
private:
	float xstart, xend;
	float ystart, yend;
	int xn, yn;
	float xstep, ystep;
	MatrixXi wall_mat;
	Vector2i get_mesh_coor(Vector2f coor);
	Vector2f get_real_coor(Vector2i coor);
	void line(Vector2i coor0, Vector2i coor1);//这一部分代码是Bresenham模型,来自于https://zhuanlan.zhihu.com/p/537899426
};