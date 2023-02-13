#include"data_object.h"
#include<iostream>

int main()
{
	Matrix4f testmat;
	testmat << 0, 1, 5, 4,
		1, 0, 2, 1,
		5, 2, 0, 3,
		4, 1, 3, 0;
	MapSearcher test_search(testmat, 2, 0.5, 100, 1, 1);
	cout << test_search.tmat << endl;
	vector<double> rlength;
	for(int i=0;i<10000;i++)
		rlength.push_back(test_search.one_iter());
	for (auto i = rlength.begin(); i < rlength.end(); i++)
		cout << *i << endl;
	cout << test_search.tmat << endl;
}