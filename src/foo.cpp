#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/eigen/dense.h>
#include <nanobind/eigen/sparse.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <cstdint>

namespace nb = nanobind;

using Numeric = double;
using Integer = int64_t;
namespace Eigen
{
  typedef Matrix<Numeric, Dynamic, Dynamic> MatrixXN;
  typedef Matrix<Integer, Dynamic, Dynamic> MatrixXI;
  typedef Matrix<Numeric, Dynamic, 1> VectorXN;
  typedef Matrix<Integer, Dynamic, 1> VectorXI;
  typedef SparseMatrix<Numeric> SparseMatrixN;
  typedef SparseMatrix<Integer> SparseMatrixI;
}



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
  x(0) = 1;//X1.array().sum() + X2.array().sum();
  i.resize(1);
  i(0) = 2;//I1.array().sum() + I2.array().sum();

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
///////////////////////////////////////////////////////////////////////////////

nb::object foo_binding(
  const nb::DRef<const Eigen::MatrixXN> & X1,
  const nb::DRef<const Eigen::MatrixXN> & X2,
  const nb::DRef<const Eigen::MatrixXI> & I1,
  const nb::DRef<const Eigen::MatrixXI> & I2)
{
  Eigen::VectorXN x;
  Eigen::VectorXI i;
  Eigen::SparseMatrixN s;
  Eigen::SparseMatrixI t;
  foo(X1, X2, "hello", I1, I2, x, i, s, t);
  return nb::make_tuple(x, i, s, t);
  //return nb::none();
}

// Finally actually bind the dispatcher to the Python module
// Once again it's sad to have to repeat the input parameters.
NB_MODULE(my_module, m) {
  m.def("foo", &foo_binding, 
    nb::arg("X1"), nb::arg("X2"), nb::arg("I1"), nb::arg("I2"));
}
