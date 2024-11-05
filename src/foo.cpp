#include "type_utils.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/eigen.h>
#include <Eigen/Dense>

namespace py = pybind11;

///////////////////////////////////////////////////////////////////////////////////
/// Example library function (we assume this cannot be changed)
///////////////////////////////////////////////////////////////////////////////////

// Libigl style function with const MatrixBase & for input and PlainObjectBase & for output
template <
  typename DerivedX1,
  typename DerivedX2,
  typename DerivedI1,
  typename DerivedI2,
  typename Derivedx,
  typename Derivedi,
  typename s_type,
  typename t_type
  >
void foo(
  const Eigen::MatrixBase<DerivedX1> & X1,
  const Eigen::MatrixBase<DerivedX2> & X2,
  const std::string & str,
  const Eigen::MatrixBase<DerivedI1> & I1,
  const Eigen::MatrixBase<DerivedI2> & I2,
  Eigen::PlainObjectBase<Derivedx> & x,
  Eigen::PlainObjectBase<Derivedi> & i,
  Eigen::SparseMatrix<s_type> & s,
  Eigen::SparseMatrix<t_type> & t)
{
  x.resize(1);
  x(0) = X1.array().sum() + X2.array().sum();
  i.resize(1);
  i(0) = I1.array().sum() + I2.array().sum();

  s.resize(1,1);
  s.insert(0,0) = x(0);
  s.makeCompressed();
  t.resize(1,1);
  t.insert(0,0) = i(0);
  t.makeCompressed();
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
  Eigen::SparseMatrix<numeric> s;
  Eigen::SparseMatrix<integer> t;
  foo(X1, X2, str, I1, I2, x, i, s, t);
  return py::make_tuple(x, i, s, t);
}

// This dispatcher is responsible for exploding the different combinations of
// numeric and integer types and making sure that all are invoked at compilation
// time and that the correct one is called at runtime.
//
// It's sad to have to keep repeating the input parameters. 
py::object foo_dispatch(py::array X1, py::array X2, std::string str, py::array I1, py::array I2) 
{
  // Check that all numeric/integer inputs are using the same dtype and note it
  py::dtype numeric_in = pyigl::same_dtypes<pyigl::NUMERIC>(X1,X2);
  py::dtype integer_in = pyigl::same_dtypes<pyigl::INTEGER>(I1,I2);

  return pyigl::for_each_numeric_integer_pair(
    numeric_in,integer_in,
    [&](auto numeric_tag, auto integer_tag)->py::object
    {
      using numeric = typename decltype(numeric_tag)::type;
      using integer = typename decltype(integer_tag)::type;
      // Repeating the input parameters yet again, this time wrapped in
      // corresponding casts to numeric or integer py::EigenDRef matrices
      return foo_binding<numeric, integer>(
        pyigl::matrix<numeric>(X1), 
        pyigl::matrix<numeric>(X2), 
        str, 
        pyigl::matrix<integer>(I1), 
        pyigl::matrix<integer>(I2));
    });
}

// Finally actually bind the dispatcher to the Python module
// Once again it's sad to have to repeat the input parameters.
PYBIND11_MODULE(my_module, m) {
  m.def("foo", &foo_dispatch, 
    py::arg("X1"), py::arg("X2"), py::arg("str"), py::arg("I1"), py::arg("I2"));
}
