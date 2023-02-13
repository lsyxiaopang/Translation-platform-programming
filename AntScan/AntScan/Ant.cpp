#include "data_object.h"

void Ant::setAnt(int point_now, set<int> allow_points,int antno)
{
	this->point_now = point_now;
	this->allow_point = allow_points;
	this->ant_no = antno;
	this->route_length = 0;
}
Ant::Ant()
{

}

bool Ant::choose_next(MapSearcher* map)
{
	//���ж��ǲ���������
	if (this->allow_point.size() == 0)
		return false;
	//�������ǲ������̵ķ�ʽ���м���
	// 
	//���ȼ����ÿһ�����̸���������ܺ�
	vector<int> dotnum;
	vector<double> score;
	double all_score = 0;//��һ���ܺ�
	for (auto dnow:this->allow_point)
	{
		dotnum.push_back(dnow);
		double ns = pow((map->tmat(this->point_now, dnow)),(map->alpha)) /
					pow((map->rmat(this->point_now, dnow)),(map->beta));
		score.push_back(ns);
		all_score += ns;
	}
	//���濪ʼ���̶�
	double sn = 0;
	double rand = map->dis(map->gen);
	for (int i = 0; i < dotnum.size(); i++)
	{
		sn += score[i] / all_score;
		if (sn > rand)
		{
			//ѡ���µ�
			int new_point = dotnum[i];
			(map->refresh_mat+this->point_now*map->point_num+new_point)->push_back(this->ant_no);//���һ����Ϣ�ؾ���
			//this->processed_point.insert(this->point_now);
			route.push_back(this->point_now);
			this->route_length += map->rmat(this->point_now, new_point);
			this->allow_point.erase(this->allow_point.find(new_point));//���µ���Ϣ
			this->point_now = new_point;
			return true;
		}
	}
    cout<<all_score<<endl<<map->rmat<<endl<<map->tmat;
	throw "Cannot handle next point";
}