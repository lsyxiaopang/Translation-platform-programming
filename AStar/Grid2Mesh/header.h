#pragma once

#ifdef PYTHON_LIB

#include"pybind11/eigen.h"
#include"pybind11/pybind11.h"
#include"pybind11/stl.h"

#else

#include<vector>
#include<array>
#include<math.h>
#include<set>
#include<Eigen/Dense>

#endif