#include <pybind11/pybind11.h>
//#include "../SCImages/core.h"

namespace py = pybind11;

double eval_expr_py(const std::string& expr, const py::dict& vars) {
    /*
    int n = vars.size();
    std::vector<double> args(n);

    CompiledFunc* func = compile_func(expr.c_str(), "<expr>");
    if (!func) throw std::runtime_error("compile error");

    double result = eval_func(func, args.data(), n);
    free_func(func);*/
    return 31456;//result;
}

PYBIND11_MODULE(tcalc, m) {
    m.doc() = "x86-64 JIT expression evaluator";
    m.def("eval_expr", eval_expr_py, "compile and eval expr");
    // m.def("plot_formula", ...);
}