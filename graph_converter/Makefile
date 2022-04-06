
#CC := /scratch/env/usr/bin/g++ -std=c++17 -O3
CC := g++ -std=c++17 -O3
CFLAGS := -fopenmp -Wall -Werror=return-type -Wshadow #-Wno-unused-variable -Wno-reorder

#NV := nvcc -ccbin g++ -m64 -std=c++14 -O3
#NVFLAGS := -gencode arch=compute_35,code=sm_35 -gencode arch=compute_70,code=sm_70
#$(NV) $(NVFLAGS) -Xcompiler "-fopenmp" $< -o $@

cc_sources:=$(wildcard *.cpp)
cc_objects:=$(patsubst %.cpp, bin/%, $(cc_sources))

all:; @$(MAKE) _all -j16
_all: $(cc_objects) bin/bsort
.PHONY: all _all

bin/%: %.cpp def.h Makefile
	$(CC) $(CFLAGS) $< -o $@

bin/bsort: bsort.c Makefile
	gcc -O3 $< -o $@

clean:
	-@rm -rf bin/*

