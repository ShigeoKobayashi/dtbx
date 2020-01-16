DLL_SOURCES=dtbx.cpp block3.cpp debug.cpp lusolver.cpp sort.cpp
TEST_SOURCE=test.c
HEADERS=stdafx.h debug.h dtbx.h stdafx.h templates.h
OBJS=dtbx.o block3.o debug.o lusolver.o sort.o

lib:
	g++ -o libdtbx.so -shared -fPIC -fvisibility=hidden -D _DEBUG $(DLL_SOURCES)
link:
	gcc -L./ -o test test.c -ldtbx
run:
	LD_LIBRARY_PATH=. ./test
