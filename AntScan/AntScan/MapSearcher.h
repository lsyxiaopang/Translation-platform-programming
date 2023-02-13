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
	MatrixXf tmat;//��Ϣ�ؾ���
	MatrixXf rmat;//���ʾ���
	vector<int>* refresh_mat;//��Ϣ�ؾ�������б�
	Ant* ants;
	int ant_num;
	int point_num;//�ж��ٸ���
	float p;//��Ϣ�ص�ÿ�ֲ�����
	float alpha;//��Ϣ���ж���Ҫ
	float beta;//�����ж���Ҫ
	uniform_real_distribution<float> dis;//���̶��õ������
	mt19937 gen;

	float best_now;
	vector<int> best_route;

	float one_iter();

	~MapSearcher();
	MapSearcher(MatrixXf rmat, int ant_num_per_point,
		float p, float alpha, float beta,int rand_state);
};

