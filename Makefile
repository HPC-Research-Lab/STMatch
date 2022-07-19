DEBUG = 

OPTIONS = -Xptxas -v
GPU_MATCH = src/gpu_match.cu

define compile_cu_test
	nvcc -std=c++17 $(DEBUG) $(OPTIONS) -arch=compute_86 $(1) cu_test.cu -o $(2)
endef

define compile_gpu_match
	nvcc -std=c++17 $(DEBUG) $(OPTIONS) -arch=compute_86 -dc -I. $(1) -o $(2)
endef

define edit_config
	sed -i "/#include \"config_for_ae/c\#include \"config_for_ae/$(1)\" " src/config.h
endef

.PHONY:all
all:bin/table_vertex_ulb.exe bin/table_edge_ulb.exe bin/table_edge_lb.exe bin/fig_naive.exe bin/fig_local.exe bin/fig_local_global.exe  bin/fig_local_global_unroll.exe 

bin/%.exe:bin/%.o;
	$(call compile_cu_test,$<,$@)
bin/%.o:
	$(call edit_config,$(patsubst bin/%.o,%.h,$@))
	$(call compile_gpu_match,src/gpu_match.cu,$@)

.PHONY:clean
clean:
	rm -f bin/*.o bin/*.exe