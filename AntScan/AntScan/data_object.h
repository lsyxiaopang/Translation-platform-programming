#pragma once

#ifdef PYTHON_LIB
    #include"pybind11/stl.h"
    #include"pybind11/eigen.h"
    #include"pybind11/pybind11.h"
    #include"pybind11/iostream.h"
#else
    #include<set>
    #include<vector>
    #include<iostream>
	#include<Eigen/Dense>
#endif
#include<math.h>
#include<random>

using namespace std;
using namespace Eigen;

class MapSearcher;

class Ant
{
public:
	void setAnt(int point_now, set<int> allow_points, int ant_no);
	Ant();
	bool choose_next(MapSearcher* map);
	double route_length;
	vector<int> route;
	bool available;//判断有没有走完全程
private:
	int point_now;
	set<int> allow_point;
	//set<int> processed_point	
	int ant_no;	
};

class MapSearcher
{
public:
	MatrixXf tmat;//信息素矩阵
	MatrixXf rmat;//路径矩阵
	vector<int>* refresh_mat;//信息素刷新所需要的信息(每条路哪些蚂蚁爬过)
	Ant* ants;
	int ant_num;
	int point_num;//点数
	double p;//信息素一次迭代后保留量
	double alpha;//信息素重要程度指数
	double beta;//路径重要程度指数
    double msum;
	uniform_real_distribution<double> dis;//随机数生成器
	mt19937 gen;

	double best_now;
	vector<int> best_route;

	double one_iter();

	~MapSearcher();
	MapSearcher(MatrixXf rmat, int ant_num_per_point,
		double p, double alpha, double beta, int rand_state);
};


