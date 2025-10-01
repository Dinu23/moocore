#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "mooarchiver.hpp"

namespace py = pybind11;

class PyArchive : public BaseArchive<Solution> {
public:
    PyArchive(unsigned dimension) : BaseArchive<Solution>(dimension) {}

    dominance_t add(const Solution &s) override {
        PYBIND11_OVERLOAD_PURE(
            dominance_t,
            BaseArchive<Solution>,
            add,
            s
        );
    }

    // REMOVE override from these if not virtual in base!
    size_t size() const { return BaseArchive<Solution>::size(); }
    size_t dimension() const { return BaseArchive<Solution>::dimension(); }
    bool overfull() { return BaseArchive<Solution>::overfull(); }
    void clear() { BaseArchive<Solution>::clear(); }
};

void define_archives(py::module &m) {
    py::enum_<dominance_t>(m, "Dominance")
        .value("IS_DOMINATED_BY", dominance_t::IS_DOMINATED_BY)
        .value("DOMINATES", dominance_t::DOMINATES)
        .value("NONDOMINATED", dominance_t::NONDOMINATED)
        .value("EQUALS", dominance_t::EQUALS)
        .export_values();

    py::class_<BaseArchive<Solution>, PyArchive>(m, "BaseArchive")
        .def(py::init<unsigned>())
        .def("add", &BaseArchive<Solution>::add)
        .def("add", [](BaseArchive<Solution>& archive, const std::vector<double>& values) {
            Solution s;
            s.setObjectives(values);
            return archive.add(s);
        }, "Add solution from objectives vector")
        .def("size", &BaseArchive<Solution>::size)
        .def("dimension", &BaseArchive<Solution>::dimension)
        .def("overfull", &BaseArchive<Solution>::overfull)
        .def("clear", &BaseArchive<Solution>::clear)
        .def("__len__", &BaseArchive<Solution>::size)
        .def("__repr__", [](const BaseArchive<Solution> &a) {
            return "BaseArchive(size=" + std::to_string(a.size()) +
                   ", dimension=" + std::to_string(a.dimension()) + ")";
        });

    py::class_<UnboundedArchive<Solution>, BaseArchive<Solution>>(m, "UnboundedArchive")
        .def(py::init<unsigned>())
        .def("add", &UnboundedArchive<Solution>::add, "Add Solution to archive")
        .def("add", [](UnboundedArchive<Solution>& archive, const std::vector<double>& values) {
            Solution s;
            s.setObjectives(values);
            return archive.add(s);
        }, "Add solution from objectives vector")
        .def("size", &UnboundedArchive<Solution>::size, "Get archive size")
        .def("max_size", static_cast<size_t (UnboundedArchive<Solution>::*)() const>(&UnboundedArchive<Solution>::max_size), "Get archive max size")
        .def("dimension", &UnboundedArchive<Solution>::dimension, "Get archive dimension")
        .def("overfull", &UnboundedArchive<Solution>::overfull, "Check if archive is overfull")
        .def("clear", &UnboundedArchive<Solution>::clear, "Clear the archive")
        .def("get_solutions", [](const UnboundedArchive<Solution>& archive) {
            std::vector<Solution> solutions;
            for (size_t i = 0; i < archive.size(); ++i) {
                solutions.push_back(*archive[i]);
            }
            return solutions;
        })
        .def("__repr__", [](const UnboundedArchive<Solution> &a) {
            return "UnboundedArchive(size=" + std::to_string(a.size()) +
                   ", dimension=" + std::to_string(a.dimension()) + ")";
        });

    py::class_<DominatingArchive<Solution>, BaseArchive<Solution>>(m, "DominatingArchive")
        .def(py::init<typename DominatingArchive<Solution>::size_type, typename DominatingArchive<Solution>::size_type>(),
             "Create dominating archive with maxsize and dimension")
        .def("add", &DominatingArchive<Solution>::add, "Add Solution to archive")
        .def("add", [](DominatingArchive<Solution>& archive, const std::vector<double>& values) {
                Solution s;
                s.setObjectives(values);
                return archive.add(s);
            }, "Add solution from objectives vector")
        .def("size", &DominatingArchive<Solution>::size, "Get archive size")
        .def("max_size", static_cast<size_t (DominatingArchive<Solution>::*)() const>(&DominatingArchive<Solution>::max_size), "Get archive max size")
        .def("dimension", &DominatingArchive<Solution>::dimension, "Get archive dimension")
        .def("overfull", &DominatingArchive<Solution>::overfull, "Check if archive is overfull")
        .def("clear", &DominatingArchive<Solution>::clear, "Clear the archive")
        .def("get_solutions", [](const DominatingArchive<Solution>& archive) {
            std::vector<Solution> solutions;
            for (size_t i = 0; i < archive.size(); ++i) {
                solutions.push_back(*archive[i]);
            }
            return solutions;
        })
        .def("__repr__", [](const DominatingArchive<Solution> &a) {
            return "DominatingArchive(size=" + std::to_string(a.size()) +
                   ", dimension=" + std::to_string(a.dimension()) + ")";
        });
    
    
    py::class_<HVArchive<Solution>, BaseArchive<Solution>>(m, "HVArchive")
        .def(py::init<unsigned int, unsigned int, Random&, bool>(),
            py::arg("maxsize"),
            py::arg("dim"),
            py::arg("rng"),
            py::arg("only_nondominated_p") = true,
            "Create HVArchive with maxsize, dimension, RNG, and only_nondominated_p flag")
        .def("add", &HVArchive<Solution>::add, "Add Solution to archive")
        .def("add", [](HVArchive<Solution>& archive, const std::vector<double>& values) {
                Solution s;
                s.setObjectives(values);
                return archive.add(s);
            }, "Add solution from objectives vector")
        .def("size", &HVArchive<Solution>::size, "Get archive size")
        .def("max_size", static_cast<size_t (HVArchive<Solution>::*)() const>(&HVArchive<Solution>::max_size), "Get archive max size")
        .def("dimension", &HVArchive<Solution>::dimension, "Get archive dimension")
        .def("overfull", &HVArchive<Solution>::overfull, "Check if archive is overfull")
        .def("clear", &HVArchive<Solution>::clear, "Clear the archive")
        .def("get_solutions", [](const HVArchive<Solution>& archive) {
            std::vector<Solution> solutions;
            for (size_t i = 0; i < archive.size(); ++i) {
                solutions.push_back(*archive[i]);
            }
            return solutions;
        })
        .def("__repr__", [](const HVArchive<Solution> &a) {
            return "HVArchive(size=" + std::to_string(a.size()) +
                   ", dimension=" + std::to_string(a.dimension()) + ")";
        });
    
}
