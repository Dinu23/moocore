#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <vector>
#include <memory>

#include "mooarchiver.hpp"

namespace py = pybind11;

void define_solution(py::module &m);
void define_archives(py::module &m);
void define_random(py::module &m);

PYBIND11_MODULE(mooarchives, m) {
    m.doc() = "Multi-Objective Archive Library Python bindings";

    define_solution(m);

    define_archives(m);

    define_random(m);
}