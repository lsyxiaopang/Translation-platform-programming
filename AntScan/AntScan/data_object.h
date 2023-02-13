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
private:
	int point_now;
	set<int> allow_point;
	//set<int> processed_point	
	int ant_no;
};

class MapSearcher
{
public:
	MatrixXf tmat;//��Ϣ�ؾ���
	MatrixXf rmat;//���ʾ���
	vector<int>* refresh_mat;//��Ϣ�ؾ�������б�
	Ant* ants;
	int ant_num;
	int point_num;//�ж��ٸ���
	double p;//��Ϣ�ص�ÿ�ֲ�����
	double alpha;//��Ϣ���ж���Ҫ
	double beta;//�����ж���Ҫ
    double msum;
	uniform_real_distribution<double> dis;//���̶��õ������
	mt19937 gen;

	double best_now;
	vector<int> best_route;

	double one_iter();

	~MapSearcher();
	MapSearcher(MatrixXf rmat, int ant_num_per_point,
		double p, double alpha, double beta, int rand_state);
};


