# dtbx
[Digital Toolbox (C/C++ API library)](http://www.tinyforest.jp/oss/dtbx.html)

Digital Toolbox (C/C++ API library) offers some useful functions(as of 2019) such as:
 1.Matrix block triangular decomposition
 2.Linear equation system solver
 3.Sorting

Matrix block triangular decomposition is sometimes useful to solve linear equation system such as
 Ax=b by decomposing into a sequence of smaller sized equation systems.
This is an another approach to reduce total amount of computations.

To build this software on Linux => see makefile bundled
  
To build this software on Windows => create Visual studio solution,and add source files.

To use this library => include dtbx.h in your source codes(see test.c) and link appropriate library files.
