#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <parallel/numeric>
#include <cstring>
#include <filesystem>
#include "def.h"

void sortLists(std::string fname) {
  try {
    std::filesystem::rename(fname + ".raw.edge.bin", fname + ".edge.bin");
    std::filesystem::rename(fname + ".raw.vertex.bin", fname + ".vertex.bin");
  } catch (std::filesystem::filesystem_error& e) {
    std::cout << e.what() << '\n';
    exit(1);
  }
  const auto vertex = MemoryMappedFile<uint64_t>::Read(fname + ".vertex.bin");
  auto edge = MemoryMappedFile<vidType>::Modify(fname + ".edge.bin");
  vidType n_vertices = vertex.size() - 1, max_degree = 0;
  #pragma omp parallel for schedule(dynamic,64) reduction(max:max_degree)
  for(vidType v = 0; v < n_vertices; v++) {
    if(v % 100000 == 0) LOG("%u vertices sorted", v);
    uint64_t low = vertex[v], high = vertex[v + 1];
    std::sort(edge.ptr(low), edge.ptr(high));
    vidType deg = std::unique(edge.ptr(low), edge.ptr(high)) - edge.ptr(low);
    max_degree = std::max(deg, max_degree);
    if(deg != high - low) {
      printf("ERROR vertex %u has %u neighbors when it should have %lu\n", v, deg, high - low);
    }
  }
  std::ofstream meta((fname + ".meta.txt").c_str());
  meta << n_vertices << "\n" << edge.size() << "\n" << sizeof(vidType) << "\n" << max_degree << "\n";
  meta.close();
  std::cout << "\ndone\n" << std::flush;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "usage: ./convert <snap file>\n";
    return 1;
  }
  std::string fname = argv[1];
  sortLists(fname);
  return 0;
}

