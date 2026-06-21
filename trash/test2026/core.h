#pragma once
#include <string>


#ifdef _WIN32
#define CORE_API extern "C" __declspec(dllexport)
#else
#define CORE_API extern "C" __attribute__((visibility("default")))
#endif

CORE_API double eval_expr(const char* expr, double* vars, int var_count);