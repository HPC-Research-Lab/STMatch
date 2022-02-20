DEBUG = -G -g

objects = cu_test.o KernelLauncher.o PatternMatcher.o

all: $(objects)
	nvcc -arch=compute_86 $(objects) -o main

%.o: src/%.cu
	nvcc -std=c++17 $(DEBUG) -arch=compute_86 -dc -I. $< -o $@

clean:
	rm -f *.o main