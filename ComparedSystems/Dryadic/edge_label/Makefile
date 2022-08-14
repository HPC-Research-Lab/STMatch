#CXX:=icpc
#CPPFLAGS:=-Wall -Werror=return-type -Wshadow -O0 -g -rdynamic -D_GLIBCXX_DEBUG
#CPPFLAGS := -Wall -Werror=return-type -Wshadow -Wno-unused-variable -Wno-reorder -O0 -g -fpermissive
CPPFLAGS := -Wall -Werror=return-type -Wshadow -Wno-unused-variable -Wno-reorder -O3 -fpermissive -g
#CPPFLAGS += -DUSE_NUMA -lnuma
#CXX:=g++ -std=c++17 -fopenmp
CXX:=g++-9 -std=c++0x -fopenmp
#CXX:=clang -std=c++11 -O3

sources:=$(wildcard plans/*.hpp)
objects:=$(patsubst plans/plan_%.hpp, bin/%, $(sources))

all:; @$(MAKE) _all -j4
_all: $(objects)
.PHONY: all _all

bin/%: plans/plan_%.hpp template_test.cpp support.h Makefile
	touch -a $<.h
	sed "/INCLUDE_HERE/c\#include \"$<\"" 'template_test.cpp'| sed "/INCLUDE_HEADER_HERE/c\#include \"$<.h\"" | tee $<.ceep | $(CXX) -x c++ - $(CPPFLAGS) -o $@


clean:
	-@rm -rf bin/*
