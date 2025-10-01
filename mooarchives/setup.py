import os
import sys
import subprocess
from pathlib import Path

from pybind11.setup_helpers import Pybind11Extension, build_ext
from pybind11 import get_cmake_dir
import pybind11
from setuptools import setup, find_packages

# Get the absolute path to the project root
PROJECT_ROOT = Path(__file__).parent.absolute()

# Define include directories
include_dirs = [
    str(PROJECT_ROOT / "include"),
    str(PROJECT_ROOT / "include" / "archiver"),
    str(PROJECT_ROOT / "include" / "archiver" / "utils"),
    str(PROJECT_ROOT / "include" / "archiver" / "archives"),
    str(PROJECT_ROOT / "external" / "libmoocore"),
    pybind11.get_include(),
]

# Define library directories
library_dirs = [
    str(PROJECT_ROOT / "lib"),
    str(PROJECT_ROOT / "external" / "libmoocore"),
]

# Define libraries to link against
libraries = ["mooarchives", "moocore", "m"]

# Define source files for the extension
ext_modules = [
    Pybind11Extension(
        "mooarchives",
        sources=[
            str( "moa/src/mooarchives.cpp"),
            str( "moa/src/archives.cpp"),
            str( "moa/src/solution.cpp"),
            str( "moa/src/random.cpp"),
        ],
        include_dirs=include_dirs,
        library_dirs=library_dirs,
        libraries=libraries,
        language="c++",
        cxx_std=11,
        extra_compile_args=["-std=c++14"],
        define_macros=[("VERSION_INFO", '"0.0.1"')],
    ),
]

class CustomBuildExt(build_ext):
    """Custom build extension to ensure libraries are built first"""
    
    def run(self):
        # Build the C++ libraries first
        self.build_cpp_libraries()
        super().run()
    
    def build_cpp_libraries(self):
        """Build the C++ libraries using make"""
        print("Building C++ libraries...")
        try:
            subprocess.check_call(["make", "lib"], cwd=PROJECT_ROOT)
            print("✓ C++ libraries built successfully")
        except subprocess.CalledProcessError as e:
            print(f"Failed to build C++ libraries: {e}")
            sys.exit(1)

setup(
    name="mooarchives",
    version="0.0.1",
    author="Your Name",
    author_email="your.email@example.com",
    description="Multi-Objective Archive Library with Python bindings",
    long_description=open("README.md").read() if os.path.exists("README.md") else "",
    long_description_content_type="text/markdown",
    url="https://github.com/yourusername/mooarchives",
    packages=["moa"],
    package_dir={"moa": "moa"},
    ext_modules=ext_modules,
    cmdclass={"build_ext": CustomBuildExt},
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Mathematics"
    ],
    python_requires=">=3.7",
    install_requires=[
        "numpy>=1.15.0",
    ],
    extras_require={
        "test": ["pytest>=6.0"],
        "dev": ["pytest>=6.0", "build", "twine"],
    },
    zip_safe=False,
)