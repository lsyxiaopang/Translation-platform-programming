#pragma once
#include<set>
#include<memory>
#include<vector>
#include<Eigen/Dense>
#include"Ant.h"

using namespace std;
using namespace Eigen;

class MapSearcher
{
public:
	MatrixXf tmat;//信息素矩阵
	MatrixXf rmat;//概率矩阵
	vector<int>* refresh_mat;//信息素矩阵更新列表
	Ant* ants;
	int ant_num;
	int point_num;//有多少个点
	float p;//信息素的每轮残留率
	float alpha;//信息素有多重要
	float beta;//距离有多重要
	uniform_real_distribution<float> dis;//轮盘赌用的随机数
	mt19937 gen;

	float best_now;
	vector<int> best_route;

	float one_iter();

	~MapSearcher();
	MapSearcher(MatrixXf rmat, int ant_num_per_point,
		float p, float alpha, float beta,int rand_state);
};

