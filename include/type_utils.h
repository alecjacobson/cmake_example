#pragma once
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/eigen.h>
#include <Eigen/Dense>
#include <optional>
#include <cstdint>

///////////////////////////////////////////////////////////////////////////////////
/// Utility functions for handling all combinations of input numeric and integer
/// types
///////////////////////////////////////////////////////////////////////////////////

namespace pyigl
{
  namespace py = pybind11;
  enum TYPE { NUMERIC=0, INTEGER=1};
  using NUMERIC_dtypes = std::tuple<float, double>;
  using INTEGER_dtypes = std::tuple<int32_t, int64_t>;
  
  template <typename Func, typename Numeric>
  py::object for_each_integer_type(
    const py::dtype& numeric_in,
    const py::dtype& integer_in,
    Func func, 
    Numeric numeric)
  {
    py::object ret = py::none();
    bool called = false;
    std::apply([&](auto... Integers)
      {
        (
          [&]()
          {
            auto integer_tag = std::type_identity<decltype(Integers)>{};
            using integer = typename decltype(integer_tag)::type;
            if(!called && integer_in.is(py::dtype::of<integer>()))
            {
              ret = func(numeric, integer_tag);
              called = true;
            }
          }()
        , ...);
      }, INTEGER_dtypes{});
    if(!called)
    {
      throw py::value_error("Invalid integer type.");
    }
    return ret;
  }
  
  template <typename Func>
  py::object for_each_numeric_type(
    const py::dtype& numeric_in,
    const py::dtype& integer_in,
    Func func)
  {
    py::object ret = py::none();
    bool called = false;
    std::apply([&](auto... Numerics)
      {
        (
          [&]()
          {
            auto numeric_tag = std::type_identity<decltype(Numerics)>{};
            using numeric = typename decltype(numeric_tag)::type;
            if(numeric_in.is(py::dtype::of<numeric>()))
            {
              ret = for_each_integer_type(numeric_in, integer_in, func, numeric_tag);
              called = true;
            }
          }()
        , ...);
      }, NUMERIC_dtypes{});
    if(!called)
    {
      throw py::value_error("Invalid numeric type.");
    }
    return ret;
  }
  
  template <typename Func>
  py::object for_each_numeric_integer_pair(
    const py::dtype& numeric_in,
    const py::dtype& integer_in,
    Func func) 
  {
    return for_each_numeric_type(numeric_in,integer_in, func);
  }
  
  template <typename Tuple>
  bool matches_dtype(const py::dtype& dtype, const Tuple& types_tuple) {
      return std::apply([&](auto... types) {
          return ((dtype.is(py::dtype::of<decltype(types)>())) || ...);
      }, types_tuple);
  }
  
  template <TYPE type>
  bool is(const pybind11::dtype& first_dtype) {
    if constexpr (type == NUMERIC) {
      return matches_dtype(first_dtype, NUMERIC_dtypes{});
    } else if constexpr (type == INTEGER) {
      return matches_dtype(first_dtype, INTEGER_dtypes{});
    }
    return false;
  }
  
  template <
    TYPE type = NUMERIC,
    typename... Arrays
    >
  py::dtype same_dtypes(const Arrays&... arrays) 
  {
    std::string type_name = type == NUMERIC ? "numeric" : "integer";
    std::string types_list = type == NUMERIC ? "float32 or float64" : "int32 or int64";
  
    std::optional<pybind11::dtype> first_dtype;
    // Helper lambda to check each array's dtype
    auto check_dtype = [&](const pybind11::array& array) -> bool 
    {
      if (array.size() > 0)
      {
        if (!first_dtype)
        {
          first_dtype = array.dtype();
          if(!is<type>(*first_dtype)) 
          {
            // use type_name
            throw pybind11::value_error("All " + type_name + " array types must be " + types_list + ".");
          }
        } else 
        {
          if (!array.dtype().is(*first_dtype)) 
          {
            // use type_name
            throw pybind11::value_error("All non-empty " + type_name + " arrays must have the same dtype.");
          }
        }
      }
      return true;
    };
  
    // Expand the lambda for each array in the parameter pack
    (check_dtype(arrays) && ...);
    if(first_dtype)
    {
      return *first_dtype;
    }
    // otherwise use default
    if constexpr (type == NUMERIC) 
    {
      return py::dtype::of<double>();
    } 
    else 
    {
      return py::dtype::of<int64_t>();
    }
  }
  
  template <typename T>
  py::EigenDRef<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> matrix(py::array A)
  {
    return py::cast<py::EigenDRef<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>>(A);
  }
  
  template <typename T>
  py::EigenDRef<Eigen::Matrix<T, Eigen::Dynamic, 1>> vector(py::array A)
  {
    return py::cast<py::EigenDRef<Eigen::Matrix<T, Eigen::Dynamic, 1>>>(A);
  }
}
