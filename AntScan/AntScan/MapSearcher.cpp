#include "data_object.h"

MapSearcher::~MapSearcher()
{
	//delete []this->ants;
}

MapSearcher::MapSearcher(MatrixXf rmat, int ant_num_per_point,
	double p, double alpha, double beta,int rand_state)
{
	//���̶��õ������������
	this->dis = uniform_real_distribution<double>(0, 1);
	this->gen = mt19937(rand_state);
	//��ʼ����������
	this->rmat = rmat;
	this->point_num = rmat.rows();
	this->p = p;
	this->alpha = alpha;
	this->beta = beta;
	this->ant_num = ant_num_per_point * this->point_num;
	//��ʼ����Ϣ�ؾ���
    double msum=rmat.sum()/this->point_num;
    this->msum=msum;
	this->tmat = MatrixXf(this->point_num, this->point_num).Ones(this->point_num, this->point_num);//���0.01�Ƿ����,δ���д���֤
	//��ʼ��������Ϣ
	this->best_now = 1e8;
}

double MapSearcher::one_iter()
{
	//��ʼ����Ϣ�ظ��¾���
	this->refresh_mat = new vector<int>[this->point_num* this->point_num];
	//���ȷ������е�
	int ants_per_point = this->ant_num / this->point_num;
	this->ants = new Ant[this->ant_num];
	//Ȼ���ÿһֻ���Ͻ��г�ʼ��
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
	//��ʼ��������
	while (true)
	{
		bool newd=true;
		for (int i = 0; i < this->ant_num; i++)
			newd = this->ants[i].choose_next(this);
		if (newd == false)
			break;
	}
	//��������,��ʼ���������Ϣ
	//��������Ϣ�ؾ���
	MatrixXf dt = MatrixXf::Zero(this->point_num, this->point_num);
	for (int i = 0; i < this->point_num; i++)
	{
		for (int j = 0; j < this->point_num; j++)
		{
			for (auto num = this->refresh_mat[i * this->point_num + j].begin(); num < this->refresh_mat[i * this->point_num + j].end(); num++)
			{
				Ant* tant = &(this->ants[*num]);
				dt(i, j) =  msum / tant->route_length;//����1��ϵ���Ƿ����,�д�ȷ֤
			}
		}
	}
    //double np=this->p+1;
	this->tmat = this->tmat *( this->p) + dt;//ʹ�ù�ʽ����
	//***DEBUG!!!!!///
	//cout << dt << endl<<endl;
	//Ȼ���Ƕ������·��
	double bestl = 1e8;
	int bestn = -1;
	for (int i = 0; i < this->ant_num; i++)
	{
		Ant* antp = &(this->ants[i]);
		if (antp->route_length < bestl)
		{
			bestl = antp->route_length;
			bestn = i;
		}
	}
	vector<int> bestroute(this->ants[bestn].route);
	//�����е�ȫ����ѱȽ�
	if (bestl < this->best_now)
	{
		this->best_now = bestl;
		this->best_route = bestroute;
	}
	//ɾ��ʹ�õ���ʱ����
	delete []this->ants;
	delete []this->refresh_mat;

	return bestl;
}