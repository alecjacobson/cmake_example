cmake_minimum_required(VERSION 3.14)
project(MyEigenPybind11Module)

# use C++20
set(CMAKE_CXX_STANDARD 20)

# Enable FetchContent to download dependencies at configure time
include(FetchContent)

# Download and set up pybind11
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG        v2.13.6
)
FetchContent_MakeAvailable(pybind11)

# Download and set up libigl
FetchContent_Declare(
  libigl
  GIT_REPOSITORY https://github.com/libigl/libigl.git
  GIT_TAG f962e4a6b68afe978dc12a63702b7846a3e7a6ed
)
FetchContent_MakeAvailable(libigl)

# Add the target for the pybind11 module
pybind11_add_module(my_module src/foo.cpp)

# Link Eigen to the target
# include the include/ directory of this project
target_include_directories(my_module PRIVATE include)
target_link_libraries(my_module PRIVATE igl::core)
