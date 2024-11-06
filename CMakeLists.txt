cmake_minimum_required(VERSION 3.14)
project(MyEigenPybind11Module)

# use C++20
set(CMAKE_CXX_STANDARD 20)

if (CMAKE_VERSION VERSION_LESS 3.18)
  set(DEV_MODULE Development)
else()
  set(DEV_MODULE Development.Module)
endif()

find_package(Python 3.8 COMPONENTS Interpreter ${DEV_MODULE} REQUIRED)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Enable FetchContent to download dependencies at configure time
include(FetchContent)

# Download and set up nanobind
FetchContent_Declare(
  nanobind
  GIT_REPOSITORY https://github.com/wjakob/nanobind.git
  GIT_TAG        v2.2.0
)
FetchContent_MakeAvailable(nanobind)

# Download and set up libigl
FetchContent_Declare(
  libigl
  GIT_REPOSITORY https://github.com/libigl/libigl.git
  GIT_TAG f962e4a6b68afe978dc12a63702b7846a3e7a6ed
)
FetchContent_MakeAvailable(libigl)

# Add the target for the nanobind module
nanobind_add_module(my_module src/foo.cpp)

# Link Eigen to the target
# include the include/ directory of this project
target_include_directories(my_module PRIVATE include)
target_link_libraries(my_module PRIVATE igl::core)
