#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <vector>
#include <cstring>
#include <ostream>
#include <sstream>
#include "ASTree.h"

namespace py = pybind11;


#ifdef WIN32
#  define PATHSEP '\\'
#else
#  define PATHSEP '/'
#endif

py::str decompyle_binding(py::bytes &data) {
    PycModule mod;
    auto str = data.cast<std::string>();
    PycBuffer buffer(
            reinterpret_cast<const unsigned char*>(str.c_str()),
            str.size()
    );
    mod.loadFromMarshalledStream(
            buffer,
            PY_MAJOR_VERSION,
            PY_MINOR_VERSION
    );
    std::ostringstream pyc_output;
    decompyle(mod.code(), &mod, pyc_output);
    return pyc_output.str();
}

PYBIND11_MODULE(bindings, m) {
    m.doc() = "pycdcpy bindings";
    m.def("decompyle", &decompyle_binding, "Decompile a marshalled python file");
}