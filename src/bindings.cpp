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

///////////////////////////////////////////////////////////////////////////////////
/// Example library function
///////////////////////////////////////////////////////////////////////////////////

// Libigl style function with const MatrixBase & for input and PlainObjectBase & for output
template <
  typename DerivedX1,
  typename DerivedX2,
  typename DerivedI1,
  typename DerivedI2,
  typename Derivedx,
  typename Derivedi >
void foo(
  const  Eigen::MatrixBase<DerivedX1> & X1,
  const  Eigen::MatrixBase<DerivedX2> & X2,
  const std::string & str,
  const  Eigen::MatrixBase<DerivedI1> & I1,
  const  Eigen::MatrixBase<DerivedI2> & I2,
  Eigen::PlainObjectBase<Derivedx> & x,
  Eigen::PlainObjectBase<Derivedi> & i)
{
  x.resize(1);
  x(0) = X1.array().sum() + X2.array().sum();
  i.resize(1);
  i(0) = I1.array().sum() + I2.array().sum();
}

///////////////////////////////////////////////////////////////////////////////////
/// For each library function like `foo` we'll need to write the following
/// boilerplate:
///////////////////////////////////////////////////////////////////////////////////

// No getting around writing some small amount of code to prepare the output
// types and return the result (as a tuple for multiple return values).
//
// This function is templated on the numeric (e.g., double) and integer (e.g.,
// int) types. py::EigenDRef takes care of storage orders and seems compatible
// with MatrixBase without copying. Is it possible that using py::EigenDRef will
// be slower than compiling each stride type separately?
template <typename numeric, typename integer>
py::object foo_binding(
    py::EigenDRef<Eigen::Matrix<numeric, Eigen::Dynamic, Eigen::Dynamic>> X1,
    py::EigenDRef<Eigen::Matrix<numeric, Eigen::Dynamic, Eigen::Dynamic>> X2,
    std::string str,
    py::EigenDRef<Eigen::Matrix<integer, Eigen::Dynamic, Eigen::Dynamic>> I1,
    py::EigenDRef<Eigen::Matrix<integer, Eigen::Dynamic, Eigen::Dynamic>> I2)
{
  Eigen::Matrix<numeric, Eigen::Dynamic, 1> x;
  Eigen::Matrix<integer, Eigen::Dynamic, 1> i;
  foo(X1, X2, str, I1, I2, x, i);
  return py::make_tuple(x, i);
}

// This dispatcher is responsible for exploding the different combinations of
// numeric and integer types and making sure that all are invoked at compilation
// time and that the correct one is called at runtime.
//
// It's sad to have to keep repeating the input parameters. 
py::object foo_dispatch(py::array X1, py::array X2, std::string str, py::array I1, py::array I2) 
{
  // Check that all numeric/integer inputs are using the same dtype and note it
  py::dtype numeric_in = same_dtypes<NUMERIC>(X1,X2);
  py::dtype integer_in = same_dtypes<INTEGER>(I1,I2);

  return for_each_numeric_integer_pair(
    numeric_in,integer_in,
    [&](auto numeric_tag, auto integer_tag)->py::object
    {
      using numeric = typename decltype(numeric_tag)::type;
      using integer = typename decltype(integer_tag)::type;
      // Repeating the input parameters yet again, this time wrapped in
      // corresponding casts to numeric or integer py::EigenDRef matrices
      return foo_binding<numeric, integer>(
        matrix<numeric>(X1), matrix<numeric>(X2), str, matrix<integer>(I1), matrix<integer>(I2));
    });
}

// Finally actually bind the dispatcher to the Python module
// Once again it's sad to have to repeat the input parameters.
PYBIND11_MODULE(my_module, m) {
  m.def("foo", &foo_dispatch);
  // This is only useful for naming types...
  //m.def("foo", &foo_dispatch, 
  //  py::arg("X1"), py::arg("X2"), py::arg("str"), py::arg("I1"), py::arg("I2"));
}
