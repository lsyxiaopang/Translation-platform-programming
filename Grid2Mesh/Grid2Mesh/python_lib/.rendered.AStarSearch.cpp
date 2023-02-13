// cppimport
// #include"pybind11/pybind11.h"
#define PYTHON_LIB
#include"../vec2grid.h"
namespace py = pybind11;

PYBIND11_MODULE(AStarSearch,m)
{
    py::class_<Mesh>(m,"Mesh")
        .def(py::init<float,float,float,float,int,int>(),
            py::arg("xstart"),py::arg("xend"),
            py::arg("ystart"),py::arg("yend"),
            py::arg("xn"),py::arg("yn"))
        .def("vec2mesh",&Mesh::vec2mesh,py::arg("vecinps"))
        .def("find_route",&Mesh::find_route,
            py::arg("start"),py::arg("end"));
    //m.def("AStarSearch",&AStarSearch,"A function to find dots");
}

/*

*/