#import sys
#sys.path.append("build")
import numpy as np
import my_module
import time

X1 = np.array([[1,2],[3,4]],dtype=np.float64)
X2 = np.array([[5,6],[7,8]],dtype=np.float64)
I1 = np.array([[1,2],[3,4]],dtype=np.int32)
I2 = np.array([[5,6],[7,8]],dtype=np.int32)
x,i = my_module.foo(X1,X2,"f64_i32",I1,I2)
print(f"x: {x.dtype} {x} | i: {i.dtype} {i}")

X1 = np.array([[1,2],[3,4]],dtype=np.float64,order='F')
X2 = np.array([[5,6],[7,8]],dtype=np.float64,order='F')
I1 = np.array([[1,2],[3,4]],dtype=np.int32  ,order='F')
I2 = np.array([[5,6],[7,8]],dtype=np.int32  ,order='F')
x,i = my_module.foo(X1,X2,"f64F_i32F",I1,I2)
print(f"x: {x.dtype} {x} | i: {i.dtype} {i}")

X1 = np.array([[1,2],[3,4]],dtype=np.float64,order='C')
X2 = np.array([[5,6],[7,8]],dtype=np.float64,order='C')
I1 = np.array([[1,2],[3,4]],dtype=np.int32  ,order='C')
I2 = np.array([[5,6],[7,8]],dtype=np.int32  ,order='C')
x,i = my_module.foo(X1,X2,"f64C_i32C",I1,I2)
print(f"x: {x.dtype} {x} | i: {i.dtype} {i}")


X1 = np.array([[1,2],[3,4]],dtype=np.float32)
X2 = np.array([[5,6],[7,8]],dtype=np.float32)
I1 = np.array([[1,2],[3,4]],dtype=np.int32)
I2 = np.array([[5,6],[7,8]],dtype=np.int32)
x,i = my_module.foo(X1,X2,"f32_i32",I1,I2)
print(f"x: {x.dtype} {x} | i: {i.dtype} {i}")

X1 = np.array([[1,2],[3,4]],dtype=np.float64)
X2 = np.array([[5,6],[7,8]],dtype=np.float64)
I1 = np.array([[1,2],[3,4]],dtype=np.int64)
I2 = np.array([[5,6],[7,8]],dtype=np.int64)
x,i = my_module.foo(X1,X2,"f64_i64",I1,I2)
print(f"x: {x.dtype} {x} | i: {i.dtype} {i}")
