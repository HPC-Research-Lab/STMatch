#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <parallel/numeric>
#include <cstring>
#include "def.h"

void compactify(std::string fname) {
  const auto raw_vertex = MemoryMappedFile<uint64_t>::Read(fname + ".raw.vertex.bin");
  auto raw_edge = MemoryMappedFile<vidType>::Modify(fname + ".raw.edge.bin");
  vidType n_vertices = raw_vertex.size() - 1;
  std::vector<uint64_t> degree(n_vertices);
  #pragma omp parallel for schedule(dynamic,64)
  for(vidType v = 0; v < n_vertices; v++) {
    if(v % 100000 == 0) LOG("%u vertices sorted", v);
    uint64_t low = raw_vertex[v], high = raw_vertex[v + 1];
    std::sort(raw_edge.ptr(low), raw_edge.ptr(high));
    vidType deg = std::unique(raw_edge.ptr(low), raw_edge.ptr(high)) - raw_edge.ptr(low);
    degree.at(v) = deg;
  }
  LOG("%u vertices sorted", n_vertices);

  std::cout << "Starting scan\n" << std::flush;
  auto vertex = MemoryMappedFile<uint64_t>::Write(fname + ".vertex.bin", n_vertices+1);
  vertex[0] = 0;
  __gnu_parallel::partial_sum(degree.begin(), degree.end(), vertex.ptr(1));
  uint64_t n_edges = vertex[n_vertices];
  std::cout << "done\n";

  std::cout << raw_edge.size() << " edges came in, " << n_edges << " edges came out\n" << std::flush;

  vidType max_degree = 0;
  auto edge = MemoryMappedFile<vidType>::Write(fname + ".edge.bin", n_edges);
  #pragma omp parallel for schedule(dynamic,64) reduction(max:max_degree)
  for(vidType v = 0; v < n_vertices; v++) {
    if(v % 100000 == 0) LOG("%u vertices compacted", v);
    uint64_t raw_low = raw_vertex[v];
    uint64_t out_low = vertex[v], out_high = vertex[v + 1];
    vidType deg = out_high - out_low;
    memcpy(edge.ptr(out_low), raw_edge.ptr(raw_low), deg * sizeof(vidType));
    max_degree = std::max(max_degree, deg);
  }
  LOG("%u vertices processed", n_vertices);

  std::ofstream meta((fname + ".meta.txt").c_str());
  meta << n_vertices << "\n" << n_edges << "\n" << sizeof(vidType) << "\n" << max_degree << "\n";
  meta.close();
  std::cout << "\ndone\n" << std::flush;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "usage: ./convert <snap file>\n";
    return 1;
  }
  std::string fname = argv[1];
  compactify(fname);
  return 0;
}

