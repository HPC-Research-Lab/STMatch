# Libra: a load-balancing graph mining system 


nvcc -gencode arch=compute_86,code=sm_86 --std=c++17 cu_test.cu src/KernelLauncher.cu 