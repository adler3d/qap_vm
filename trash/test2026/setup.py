from setuptools import setup, Extension
import pybind11

ext = Extension(
    name="tcalc",
    sources=["tcalc_binding.cpp"],
    include_dirs=[pybind11.get_include(), "../"],  # "../" core.h
    libraries=[],  # "tcalc" (DLL)
    extra_compile_args=["-std=c++17"],
)

setup(
    name="tcalc",
    version="0.1",
    ext_modules=[ext],
)