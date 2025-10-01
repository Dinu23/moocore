#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <vector>
#include <memory>

#include "mooarchiver.hpp"
namespace py = pybind11;

Solution create_solution_from_vector(const std::vector<double>& objectives) {
    Solution sol;
    sol.setObjectives(objectives);
    return sol;
}
unsigned int Solution::_num_objs = 0;
void define_solution(py::module &m) {
    py::class_<Solution>(m, "Solution")
        .def(py::init<>())
        .def("set_objectives", &Solution::setObjectives, "Set objective values")
        .def("get_objectives", [](const Solution& s) {
            return s.o;
        }, "Get objective values")
        .def("get_objective", [](const Solution& s, size_t i) {
            if (i >= s.o.size()) throw std::out_of_range("Index out of range");
            return s.o[i];
        }, "Get single objective value")
        .def("num_objs", [](const Solution& s) {
            return s.o.size();
        }, "Get number of objectives")
        .def("__repr__", [](const Solution &s) {
            std::string repr = "Solution([";
            for (size_t i = 0; i < s.o.size(); ++i) {
                if (i > 0) repr += ", ";
                repr += std::to_string(s.o[i]);
            }
            repr += "])";
            return repr;
        });

    // Factory function for creating solutions
    m.def("create_solution", &create_solution_from_vector, "Create solution from objectives vector");
}