#define PYTHON_LIB//这里定义为python库编译
#include"../data_object.h"
namespace py = pybind11;

PYBIND11_MODULE(AntScan,m)
{
    py::class_<MapSearcher>(m,"MapSearcher")
                        .def(py::init<MatrixXf,int,double,double,double,int>(),
                        py::arg("road_mat"),py::arg("ant_num_per_point"),
                        py::arg("p"),py::arg("alpha"),py::arg("beta"),py::arg("rand_state"))
                        .def("one_iter",&MapSearcher::one_iter)
                        .def_readonly("best_now",&MapSearcher::best_now)
                        .def_readonly("best_route",&MapSearcher::best_route)
                        .def_readonly("tmat",&MapSearcher::tmat);

}

/*

*/