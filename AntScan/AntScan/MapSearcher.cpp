#include "data_object.h"

MapSearcher::~MapSearcher()
{
	//delete []this->ants;
}

MapSearcher::MapSearcher(MatrixXf rmat, int ant_num_per_point,
	double p, double alpha, double beta,int rand_state)
{
	//轮盘赌用的随机数与种子
	this->dis = uniform_real_distribution<double>(0, 1);
	this->gen = mt19937(rand_state);
	//初始化其他参数
	this->rmat = rmat;
	this->point_num = rmat.rows();
	this->p = p;
	this->alpha = alpha;
	this->beta = beta;
	this->ant_num = ant_num_per_point * this->point_num;
	//初始化信息素矩阵
    double msum=rmat.sum()/this->point_num;
    this->msum=msum;
	this->tmat = MatrixXf(this->point_num, this->point_num).Ones(this->point_num, this->point_num);//这个0.01是否合理,未来有待验证
	//初始化迭代信息
	this->best_now = 1e8;
}

double MapSearcher::one_iter()
{
	//初始化信息素更新矩阵
	this->refresh_mat = new vector<int>[this->point_num* this->point_num];
	//首先分配所有点
	int ants_per_point = this->ant_num / this->point_num;
	this->ants = new Ant[this->ant_num];
	//然后对每一只蚂蚁进行初始化
	set<int> stdset;
	for (int i = 0; i < this->point_num; i++)
		stdset.insert(i);
	for (int i = 0; i < this->point_num; i++)
	{
		stdset.erase(i);
		for (int j = 0; j < ants_per_point; j++)
		{
			this->ants[i * ants_per_point + j].setAnt(i, stdset, i * ants_per_point + j);
		}
		stdset.insert(i);
	}
	//开始迭代计算
	while (true)
	{
		bool newd=true;
		for (int i = 0; i < this->ant_num; i++)
			if(this->ants[i].available)
				newd = this->ants[i].choose_next(this);
		if (newd == false)
			break;
	}
	//迭代结束,开始更新相关信息
	//首先是信息素矩阵
	MatrixXf dt = MatrixXf::Zero(this->point_num, this->point_num);
	for (int i = 0; i < this->point_num; i++)
	{
		for (int j = 0; j < this->point_num; j++)
		{
			for (auto num = this->refresh_mat[i * this->point_num + j].begin(); num < this->refresh_mat[i * this->point_num + j].end(); num++)
			{
				Ant* tant = &(this->ants[*num]);
				if(tant->available)//如果走完全程才更新
					dt(i, j) =  msum / tant->route_length;//这里1的系数是否合理,有待确证
			}
		}
	}
    //double np=this->p+1;
	this->tmat = this->tmat *( this->p) + dt;//使用公式更新
	//对于tmat爆炸的可能修复方案,使用平均值归一化
	this->tmat = this->tmat / this->tmat.mean();
	//***DEBUG!!!!!///
	//cout << dt << endl<<endl;
	//然后是对于最短路径
	double bestl = 1e8;
	int bestn = -1;
	for (int i = 0; i < this->ant_num; i++)
	{
		Ant* antp = &(this->ants[i]);
		if (antp->available && antp->route_length < bestl)
		{
			bestl = antp->route_length;
			bestn = i;
		}
	}
	vector<int> bestroute(this->ants[bestn].route);
	//与类中的全局最佳比较
	if (bestl < this->best_now)
	{
		this->best_now = bestl;
		this->best_route = bestroute;
	}
	//删除使用的临时矩阵
	delete []this->ants;
	delete []this->refresh_mat;

	return bestl;
}