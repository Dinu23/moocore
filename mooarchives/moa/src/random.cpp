#include <pybind11/pybind11.h>
#include "archiver/Random.hpp"

namespace py = pybind11;

void define_random(py::module &m) {
    py::class_<Random>(m, "Random")
        .def(py::init<unsigned long int>())
        .def("rand_01", &Random::rand_01)
        .def("next", &Random::next)
        .def("rand_double", &Random::rand_double)
        .def("rand_int", py::overload_cast<int>(&Random::rand_int))
        .def("rand_int_range", py::overload_cast<int, int>(&Random::rand_int))
        .def("__call__", &Random::operator());
}