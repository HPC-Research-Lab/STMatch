DEBUG = 

OPTIONS = -Xptxas -v

objects = gpu_match.o

all: $(objects)
	nvcc -std=c++17 $(DEBUG) $(OPTIONS) -arch=sm_75 $(objects) cu_test.cu -o cu_test

%.o: src/%.cu
	nvcc -std=c++17 $(DEBUG) $(OPTIONS) -arch=sm_75 -dc -I. $< -o $@

clean:
	rm -f *.o cu_test