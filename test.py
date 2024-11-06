import numpy as np
import my_module
import time

rows = 1000000;
cols = 10;
runs = 10;

def timeit(X1,X2,I1,I2):
  x,i,s,t = my_module.foo(X1,X2,I1,I2)
  # Time the function
  start_time = time.time()
  for _ in range(runs):
      my_module.foo(X1,X2,I1,I2)
  end_time = time.time()
  print(f"np.{X1.dtype} order=\"{X1.flags['C_CONTIGUOUS'] and 'C' or 'F'}\": {(end_time-start_time)/runs} secs") 

X1 = np.array( np.random.rand(rows,cols),order="f",dtype=np.float64)
X2 = np.array( np.random.rand(rows,cols),order="f",dtype=np.float64)
I1 = np.array( np.random.rand(rows,cols),order="f",dtype=np.int64)
I2 = np.array( np.random.rand(rows,cols),order="f",dtype=np.int64)
timeit(X1,X2,I1,I2)

X1 = np.array( np.random.rand(rows,cols),order="c",dtype=np.float64)
X2 = np.array( np.random.rand(rows,cols),order="c",dtype=np.float64)
I1 = np.array( np.random.rand(rows,cols),order="c",dtype=np.int64)
I2 = np.array( np.random.rand(rows,cols),order="c",dtype=np.int64)
timeit(X1,X2,I1,I2)

X1 = np.array( np.random.rand(rows,cols),order="f",dtype=np.float32)
X2 = np.array( np.random.rand(rows,cols),order="f",dtype=np.float32)
I1 = np.array( np.random.rand(rows,cols),order="f",dtype=np.int32)
I2 = np.array( np.random.rand(rows,cols),order="f",dtype=np.int32)
timeit(X1,X2,I1,I2)

X1 = np.array( np.random.rand(rows,cols),order="c",dtype=np.float32)
X2 = np.array( np.random.rand(rows,cols),order="c",dtype=np.float32)
I1 = np.array( np.random.rand(rows,cols),order="c",dtype=np.int32)
I2 = np.array( np.random.rand(rows,cols),order="c",dtype=np.int32)
timeit(X1,X2,I1,I2)
