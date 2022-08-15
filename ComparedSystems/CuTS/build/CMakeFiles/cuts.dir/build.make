# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /space/ywei51/project/CuTS-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /space/ywei51/project/CuTS-master/build

# Include any dependencies generated for this target.
include CMakeFiles/cuts.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cuts.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cuts.dir/flags.make

CMakeFiles/cuts.dir/src/common.cpp.o: CMakeFiles/cuts.dir/flags.make
CMakeFiles/cuts.dir/src/common.cpp.o: ../src/common.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/space/ywei51/project/CuTS-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cuts.dir/src/common.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cuts.dir/src/common.cpp.o -c /space/ywei51/project/CuTS-master/src/common.cpp

CMakeFiles/cuts.dir/src/common.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cuts.dir/src/common.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /space/ywei51/project/CuTS-master/src/common.cpp > CMakeFiles/cuts.dir/src/common.cpp.i

CMakeFiles/cuts.dir/src/common.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cuts.dir/src/common.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /space/ywei51/project/CuTS-master/src/common.cpp -o CMakeFiles/cuts.dir/src/common.cpp.s

CMakeFiles/cuts.dir/src/device_funcs.cu.o: CMakeFiles/cuts.dir/flags.make
CMakeFiles/cuts.dir/src/device_funcs.cu.o: ../src/device_funcs.cu
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/space/ywei51/project/CuTS-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CUDA object CMakeFiles/cuts.dir/src/device_funcs.cu.o"
	/usr/local/cuda/bin/nvcc  $(CUDA_DEFINES) $(CUDA_INCLUDES) $(CUDA_FLAGS) -x cu -c /space/ywei51/project/CuTS-master/src/device_funcs.cu -o CMakeFiles/cuts.dir/src/device_funcs.cu.o

CMakeFiles/cuts.dir/src/device_funcs.cu.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CUDA source to CMakeFiles/cuts.dir/src/device_funcs.cu.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CUDA_CREATE_PREPROCESSED_SOURCE

CMakeFiles/cuts.dir/src/device_funcs.cu.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CUDA source to assembly CMakeFiles/cuts.dir/src/device_funcs.cu.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CUDA_CREATE_ASSEMBLY_SOURCE

CMakeFiles/cuts.dir/src/free_memories.cu.o: CMakeFiles/cuts.dir/flags.make
CMakeFiles/cuts.dir/src/free_memories.cu.o: ../src/free_memories.cu
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/space/ywei51/project/CuTS-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CUDA object CMakeFiles/cuts.dir/src/free_memories.cu.o"
	/usr/local/cuda/bin/nvcc  $(CUDA_DEFINES) $(CUDA_INCLUDES) $(CUDA_FLAGS) -x cu -c /space/ywei51/project/CuTS-master/src/free_memories.cu -o CMakeFiles/cuts.dir/src/free_memories.cu.o

CMakeFiles/cuts.dir/src/free_memories.cu.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CUDA source to CMakeFiles/cuts.dir/src/free_memories.cu.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CUDA_CREATE_PREPROCESSED_SOURCE

CMakeFiles/cuts.dir/src/free_memories.cu.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CUDA source to assembly CMakeFiles/cuts.dir/src/free_memories.cu.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CUDA_CREATE_ASSEMBLY_SOURCE

CMakeFiles/cuts.dir/src/gpu_memory_allocation.cu.o: CMakeFiles/cuts.dir/flags.make
CMakeFiles/cuts.dir/src/gpu_memory_allocation.cu.o: ../src/gpu_memory_allocation.cu
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/space/ywei51/project/CuTS-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CUDA object CMakeFiles/cuts.dir/src/gpu_memory_allocation.cu.o"
	/usr/local/cuda/bin/nvcc  $(CUDA_DEFINES) $(CUDA_INCLUDES) $(CUDA_FLAGS) -x cu -c /space/ywei51/project/CuTS-master/src/gpu_memory_allocation.cu -o CMakeFiles/cuts.dir/src/gpu_memory_allocation.cu.o

CMakeFiles/cuts.dir/src/gpu_memory_allocation.cu.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CUDA source to CMakeFiles/cuts.dir/src/gpu_memory_allocation.cu.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CUDA_CREATE_PREPROCESSED_SOURCE

CMakeFiles/cuts.dir/src/gpu_memory_allocation.cu.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CUDA source to assembly CMakeFiles/cuts.dir/src/gpu_memory_allocation.cu.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CUDA_CREATE_ASSEMBLY_SOURCE

CMakeFiles/cuts.dir/src/graph.cpp.o: CMakeFiles/cuts.dir/flags.make
CMakeFiles/cuts.dir/src/graph.cpp.o: ../src/graph.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/space/ywei51/project/CuTS-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/cuts.dir/src/graph.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cuts.dir/src/graph.cpp.o -c /space/ywei51/project/CuTS-master/src/graph.cpp

CMakeFiles/cuts.dir/src/graph.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cuts.dir/src/graph.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /space/ywei51/project/CuTS-master/src/graph.cpp > CMakeFiles/cuts.dir/src/graph.cpp.i

CMakeFiles/cuts.dir/src/graph.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cuts.dir/src/graph.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /space/ywei51/project/CuTS-master/src/graph.cpp -o CMakeFiles/cuts.dir/src/graph.cpp.s

CMakeFiles/cuts.dir/src/host_funcs.cu.o: CMakeFiles/cuts.dir/flags.make
CMakeFiles/cuts.dir/src/host_funcs.cu.o: ../src/host_funcs.cu
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/space/ywei51/project/CuTS-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CUDA object CMakeFiles/cuts.dir/src/host_funcs.cu.o"
	/usr/local/cuda/bin/nvcc  $(CUDA_DEFINES) $(CUDA_INCLUDES) $(CUDA_FLAGS) -x cu -c /space/ywei51/project/CuTS-master/src/host_funcs.cu -o CMakeFiles/cuts.dir/src/host_funcs.cu.o

CMakeFiles/cuts.dir/src/host_funcs.cu.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CUDA source to CMakeFiles/cuts.dir/src/host_funcs.cu.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CUDA_CREATE_PREPROCESSED_SOURCE

CMakeFiles/cuts.dir/src/host_funcs.cu.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CUDA source to assembly CMakeFiles/cuts.dir/src/host_funcs.cu.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CUDA_CREATE_ASSEMBLY_SOURCE

CMakeFiles/cuts.dir/src/score.cpp.o: CMakeFiles/cuts.dir/flags.make
CMakeFiles/cuts.dir/src/score.cpp.o: ../src/score.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/space/ywei51/project/CuTS-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/cuts.dir/src/score.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cuts.dir/src/score.cpp.o -c /space/ywei51/project/CuTS-master/src/score.cpp

CMakeFiles/cuts.dir/src/score.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cuts.dir/src/score.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /space/ywei51/project/CuTS-master/src/score.cpp > CMakeFiles/cuts.dir/src/score.cpp.i

CMakeFiles/cuts.dir/src/score.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cuts.dir/src/score.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /space/ywei51/project/CuTS-master/src/score.cpp -o CMakeFiles/cuts.dir/src/score.cpp.s

CMakeFiles/cuts.dir/src/util.cpp.o: CMakeFiles/cuts.dir/flags.make
CMakeFiles/cuts.dir/src/util.cpp.o: ../src/util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/space/ywei51/project/CuTS-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/cuts.dir/src/util.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cuts.dir/src/util.cpp.o -c /space/ywei51/project/CuTS-master/src/util.cpp

CMakeFiles/cuts.dir/src/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cuts.dir/src/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /space/ywei51/project/CuTS-master/src/util.cpp > CMakeFiles/cuts.dir/src/util.cpp.i

CMakeFiles/cuts.dir/src/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cuts.dir/src/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /space/ywei51/project/CuTS-master/src/util.cpp -o CMakeFiles/cuts.dir/src/util.cpp.s

CMakeFiles/cuts.dir/main.cu.o: CMakeFiles/cuts.dir/flags.make
CMakeFiles/cuts.dir/main.cu.o: ../main.cu
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/space/ywei51/project/CuTS-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CUDA object CMakeFiles/cuts.dir/main.cu.o"
	/usr/local/cuda/bin/nvcc  $(CUDA_DEFINES) $(CUDA_INCLUDES) $(CUDA_FLAGS) -x cu -c /space/ywei51/project/CuTS-master/main.cu -o CMakeFiles/cuts.dir/main.cu.o

CMakeFiles/cuts.dir/main.cu.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CUDA source to CMakeFiles/cuts.dir/main.cu.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CUDA_CREATE_PREPROCESSED_SOURCE

CMakeFiles/cuts.dir/main.cu.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CUDA source to assembly CMakeFiles/cuts.dir/main.cu.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CUDA_CREATE_ASSEMBLY_SOURCE

# Object files for target cuts
cuts_OBJECTS = \
"CMakeFiles/cuts.dir/src/common.cpp.o" \
"CMakeFiles/cuts.dir/src/device_funcs.cu.o" \
"CMakeFiles/cuts.dir/src/free_memories.cu.o" \
"CMakeFiles/cuts.dir/src/gpu_memory_allocation.cu.o" \
"CMakeFiles/cuts.dir/src/graph.cpp.o" \
"CMakeFiles/cuts.dir/src/host_funcs.cu.o" \
"CMakeFiles/cuts.dir/src/score.cpp.o" \
"CMakeFiles/cuts.dir/src/util.cpp.o" \
"CMakeFiles/cuts.dir/main.cu.o"

# External object files for target cuts
cuts_EXTERNAL_OBJECTS =

cuts: CMakeFiles/cuts.dir/src/common.cpp.o
cuts: CMakeFiles/cuts.dir/src/device_funcs.cu.o
cuts: CMakeFiles/cuts.dir/src/free_memories.cu.o
cuts: CMakeFiles/cuts.dir/src/gpu_memory_allocation.cu.o
cuts: CMakeFiles/cuts.dir/src/graph.cpp.o
cuts: CMakeFiles/cuts.dir/src/host_funcs.cu.o
cuts: CMakeFiles/cuts.dir/src/score.cpp.o
cuts: CMakeFiles/cuts.dir/src/util.cpp.o
cuts: CMakeFiles/cuts.dir/main.cu.o
cuts: CMakeFiles/cuts.dir/build.make
cuts: CMakeFiles/cuts.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/space/ywei51/project/CuTS-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX executable cuts"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cuts.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cuts.dir/build: cuts

.PHONY : CMakeFiles/cuts.dir/build

CMakeFiles/cuts.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cuts.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cuts.dir/clean

CMakeFiles/cuts.dir/depend:
	cd /space/ywei51/project/CuTS-master/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /space/ywei51/project/CuTS-master /space/ywei51/project/CuTS-master /space/ywei51/project/CuTS-master/build /space/ywei51/project/CuTS-master/build /space/ywei51/project/CuTS-master/build/CMakeFiles/cuts.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cuts.dir/depend
