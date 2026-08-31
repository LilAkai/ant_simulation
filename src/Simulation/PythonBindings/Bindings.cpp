#include <nanobind/nanobind.h>

namespace nb = nanobind;

NB_MODULE(ant_simulation_py, m) {
    m.doc() = "Python bindings for ant_simulation";

    m.def("ping", []() { return "pong"; });
}
