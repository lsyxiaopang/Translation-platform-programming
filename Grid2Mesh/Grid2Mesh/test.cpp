#include"header.h"
#include"vec2grid.h"
#include<iostream>
using namespace std;
using Eigen::MatrixXf;
using Eigen::MatrixXd;
using Eigen::Matrix3f;
int main()
{
	dotveclist s(3,2);
	int xn = 20;
	int yn = 20;
	Mesh test_mesh(0, 10, 0, 10, xn, yn);
	s << 1.3, 2.2,
		3.6, 8.9,
		8.8, 8.5;//存在bug,太靠近会出现问题
	dotveclist sf = s.cast<float>();
	MatrixXi data = test_mesh.vec2mesh(s);
	for (int j = 0; j < yn; j++)
	{
		for (int i = 0; i < xn; i++)
		{
			if (data(j, i) == 1)
				cout << "*";
			else
				cout << "x";
		}
		cout << endl;
	}
	Vector2f beg(1, 1);
	Vector2f end(1, 1);
	dotveclist output = test_mesh.find_route(beg, end);
	cout << output;
}