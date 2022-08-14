
CC := g++ -std=c++17 -O3
CFLAGS := -fopenmp -Wall -Werror=return-type -Wshadow -Wno-unused-variable -Wno-reorder

cc_sources:=$(wildcard *.cpp)
cc_objects:=$(patsubst %.cpp, bin/%, $(cc_sources))

all:; @$(MAKE) _all -j16
_all: $(cc_objects)
.PHONY: all _all

bin/%: %.cpp def.h Makefile
	$(CC) $(CFLAGS) $< -o $@

clean:
	-@rm -rf bin/*

