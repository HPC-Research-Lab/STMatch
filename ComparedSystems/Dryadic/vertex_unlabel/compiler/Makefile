#CXX:=icpc
CXX:=g++
#CPPFLAGS:=-Wall -Werror=return-type -Wshadow -O0 -march=native -std=c++11 -qopenmp -g -traceback -debug all -D_GLIBCXX_DEBUG
#CPPFLAGS:=-Wall -Werror=return-type -Wshadow -O0 -g -std=c++17 -fopenmp -Wno-sign-compare
CPPFLAGS:=-Wall -Werror=return-type -Wshadow -O3 -march=native -std=c++17 -fopenmp -Wno-sign-compare
#CPPFLAGS += -DGPU_GENCODE

sources:=$(wildcard *.cpp)
objects:=$(patsubst %.cpp, bin/%.o, $(sources))

all:; @$(MAKE) _all 
_all: bin/compile
.PHONY: all _all

bin/%.o: %.cpp $(wildcard *.h) Makefile
	$(CXX) $(CPPFLAGS) $< -c -o $@

bin/compile: $(objects)
	$(CXX) $(CPPFLAGS) $^ -o $@

.PHONY: clean
clean:
	-@rm -f bin/*
