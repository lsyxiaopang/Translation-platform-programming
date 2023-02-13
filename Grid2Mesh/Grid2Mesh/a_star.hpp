#include"header.h"
using namespace std;
using Eigen::MatrixXi;
class dot//* 数据点的实现,其中记录了xy坐标,父节点信息,以及实现了比较运算符以便于set进行比较
{
    public:
    int x;
    int y;// 定义xy坐标
    float penaltyg;//定义从起始点到此处代价
    float penaltyh;//定义从此处到终点代价
    bool reachable;//用于判断是否可达(初始化障碍物)
    shared_ptr<dot> parent;//定义父节点信息(便于回溯)
    bool operator<(const dot& another) const;//定义比较运算符(用于set排序)
    bool operator==(const dot& another) const;//定义相等运算符(用于寻找)
    dot(int x,int y,int xend,int yend,
        float base_penalty, shared_ptr<dot> parent);//对于一个点的完整构造
    dot(int x,int y,bool reachable);//只构造点坐标,用来找点
    dot(const dot& s);
};

std::vector<dot> AStarSearch(MatrixXi reachable,//输入的障碍矩阵
                        int xlength,int ylength,
                        int startx,int starty,
                        int endx,int endy);