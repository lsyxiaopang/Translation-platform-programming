#include"a_star.hpp"

using namespace std;

//*下面这个程序是利用A*算法对于二维栅格中存在障碍物的情况下最短路径的查找
//*程序将分为两个部分:
//*第一部分是设计一种用来表示点的数据类型
//*第二部分是利用A*算法对于在一张网格中存在障碍物的情况进行追踪

//. 第一部分:点数据类型(方法实现)
dot::dot(int x,int y,bool reachable)//这个函数只能用来找点(见==运算符重载)
{
    this->x=x;
    this->y=y;
    this->reachable=reachable;
    this->parent = NULL;
}

dot::dot(int x,int y,int endx,int endy,
         float base_penalty, shared_ptr<dot> parent)
{
    this->x=x;
    this->y=y;
    this->parent=parent;
    this->reachable=true;
    //* 这里我们使用曼哈顿距离进行计算
    this->penaltyg=base_penalty+1;
    //this->penaltyh=sqrt((x-endx)^2+(y-endy)^2);
    this->penaltyh = abs(x - endx) + abs(y - endy);
    //在这里使用欧几里得距离会有一个坑
    //在调试的过程中,你会惊奇的发现:set中间出现了重复的元素
    //这与set自身的算法有关,set使用的是红黑树存储数据,那么如果我们的比较对象
    //差的很远,那么set就走不到树根(相当于我们的大小排序需要满足传递性质)
    //因为使用欧几里得法的代价可能相同点会有多个,所以可能触发这个错误
}
dot::dot(const dot& s)
{
    this->x=s.x;
    this->y=s.y;
    this->parent=parent;
    this->reachable=s.reachable;
    this->penaltyg=s.penaltyg;
    this->penaltyh=s.penaltyh;
    this->parent=s.parent;
}

bool dot::operator<(const dot& another) const
{
    if (this->x==another.x&&this->y==another.y)
        return false;//!这与set判断相等有关,如果既不大于又不小于则相等
    if(this->penaltyg+this->penaltyh==
           another.penaltyg+another.penaltyh)
        if(this->x!=another.x)
            return this->x<another.x;
        else
            return this->y<another.y;
    return (this->penaltyg+this->penaltyh)<
           (another.penaltyg+another.penaltyh);
}
//!需要注意到比较大小的运算符和比较是否相等的运算符比较的东西是否相同
//!因为前者是用来排序,后者是用来寻找
bool dot::operator==(const dot& another) const
{
    if(this->x==another.x)
    {
        if(this->y==another.y)
            return true;
    }
    return false;
}

//.这个是实现A*搜索判断点是否合法的函数
shared_ptr<dot> check_dot(MatrixXi &reachable,
               int xlength,int ylength,
               int endx,int endy,
               int x,int y,
               float base_penalty, shared_ptr<dot> parent,
               set<dot>& openset,set<dot>& closeset)
{
    if(x>=0 && x<xlength && y>=0 && y<ylength)//合法点位置
    {
        if(reachable(y,x)==1)//检查符号(是否为障碍物)
        {
            shared_ptr<dot> d(new dot(x, y, endx, endy, base_penalty, parent));
            if (openset.count(*d) + closeset.count(*d) == 0)//检查是否已经在集合内了(这一部分实际上是无效的,原因和前面相似)
            {           
                return d;
            }
        }
    }
    return NULL;
}

//.第二部分:一个函数用来实现A*搜索
vector<dot> AStarSearch(MatrixXi reachable,//输入的障碍矩阵
                        int xlength,int ylength,
                        int startx,int starty,
                        int endx,int endy)
{
    //初始化open_set和close_set
    set<dot> open_set;
    set<dot> close_set;
    //设定第一个点为初始点
    dot begin_point=dot(startx,starty,endx,endy,0,NULL);
    open_set.insert(begin_point);
    //设定一个终点,便于判断
    dot end_point=dot(endx,endy,true);
    //开始迭代
    while(!open_set.empty())
    {
        //首先选取open_set中penalty最小的一个点并从open_set中删除
        shared_ptr<dot> least_dot(new dot(*(begin(open_set))));
        open_set.erase(begin(open_set));
        close_set.insert(*least_dot);
        //判断least_dot是否就是我们要求的终点并处理
        if (*least_dot==end_point)
        {
            //开始回溯生成路径
            vector<dot> route;
            shared_ptr<dot> parent=least_dot;
            while(parent!=NULL)
            {
                route.push_back(*parent);
                shared_ptr<dot> nparent=parent->parent;
                parent = nparent;
            }
            return route;
        }
        //接下来开始对open_set四周进行查找
        //*我们目前只考虑上下左右移动的情况
        //TODO这样的代码非常不优雅,需要找个机会修改一下
        int x=least_dot->x;
        int y=least_dot->y;
        shared_ptr<dot> d=check_dot(reachable,xlength,ylength,endx,endy,x+1,y,least_dot->penaltyg,least_dot,open_set,close_set);
        if(d!=NULL)
        {
            open_set.insert(*d);
        }
        d=check_dot(reachable,xlength,ylength,endx,endy,x-1,y,least_dot->penaltyg,least_dot,open_set,close_set);
        if(d!=NULL)
        {
            open_set.insert(*d);
        }
        d=check_dot(reachable,xlength,ylength,endx,endy,x,y+1,least_dot->penaltyg,least_dot,open_set,close_set);
        if(d!=NULL)
        {
            open_set.insert(*d);
        }
        d=check_dot(reachable,xlength,ylength,endx,endy,x,y-1,least_dot->penaltyg,least_dot,open_set,close_set);
        if(d!=NULL)
        {
            open_set.insert(*d);
        }
        //delete least_dot;
    }
    return vector<dot>();
}

