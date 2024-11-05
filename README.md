# Build

```bash
mkdir build
cd build
cmake ..
make
```

# Test

```bash
cd build
PYTHONPATH=. python ../test.py
```


# Design goals

oh I see. Yeah, my criteria for an acceptable solution are roughly the same as what numpyeigen was providing:

 - no copying
 - support float64 or float32 for numeric input 
 - support int64 or int32 for integer input
 - support order='C' or order='F' 

