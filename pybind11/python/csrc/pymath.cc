#include "pymodule/pymath.h"

#include "pybind11/functional.h"
#include "pybind11/numpy.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace py = pybind11;

PYBIND11_MODULE(_pymodule, m, py::mod_gil_not_used()) {
  m.doc() = "pybind11 binding of pymodule";

  m.def("add", &add, "A function that adds two numbers");
  m.def("sub", &sub, "A function that subtracts two numbers");
}