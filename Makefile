DEBUG =

objects = Match.o

all: $(objects)
	nvcc -std=c++17 $(DEBUG) -arch=compute_86 $(objects) cu_test.cu -o cu_test

%.o: src/%.cu
	nvcc -std=c++17 $(DEBUG) -arch=compute_86 -dc -I. $< -o $@

clean:
	rm -f *.o main