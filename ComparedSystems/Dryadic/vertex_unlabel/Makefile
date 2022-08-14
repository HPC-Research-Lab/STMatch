#CXX:=icpc
#CPPFLAGS:=-Wall -Werror=return-type -Wshadow -O0 -g -rdynamic -D_GLIBCXX_DEBUG
#CPPFLAGS := -Wall -Werror=return-type -Wshadow -Wno-unused-variable -Wno-reorder -O0 -g -fpermissive
CPPFLAGS := -Wall -Werror=return-type -Wshadow -Wno-unused-variable -Wno-reorder -O9
#CPPFLAGS += -DUSE_NUMA -lnuma
#CXX:=g++ -std=c++17 -fopenmp
CXX:=g++ -std=c++0x -fopenmp
#CXX:=clang -std=c++11 -O3

sources:=$(wildcard plans/*.hpp)
objects:=$(patsubst plans/plan_%.hpp, bin/%, $(sources))

all:; @$(MAKE) _all -j4
_all: $(objects)
.PHONY: all _all

bin/%: plans/plan_%.hpp template_test.cpp support.h Makefile
	sed "/INCLUDE_HERE/c\#include \"$<\"" 'template_test.cpp' | $(CXX) -x c++ - $(CPPFLAGS) -o $@

clean:
	-@rm -rf bin/*
