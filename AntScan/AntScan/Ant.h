#pragma once
#include<set>
#include<Eigen/Dense>
#include<vector>
#include"MapSearcher.h"
#include<math.h>
#include<random>
using namespace std;
using namespace Eigen;
class Ant
{
public:
	void setAnt(int point_now, set<int> allow_points,int ant_no);
	Ant();
	bool choose_next(MapSearcher* map);
	float route_length;
	vector<int> route;
private:
	int point_now;	
	set<int> allow_point;
	//set<int> processed_point	
	int ant_no;
};

