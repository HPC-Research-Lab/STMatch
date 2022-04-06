#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include "def.h"
#include <omp.h>

void halve(std::string fname) {
  const auto vertex = MemoryMappedFile<uint64_t>::Read(fname + ".vertex.bin");
  const vidType n_vertices = vertex.size() - 1;
  auto half_vertex = MemoryMappedFile<uint64_t>::Write(fname + ".half.vertex.bin", n_vertices+1);
  std::ifstream edge((fname + ".edge.bin").c_str());
  std::ofstream half_edge((fname + ".half.edge.bin").c_str(), std::ios::binary);
  half_vertex[0] = 0;
  uint64_t half_max_degree = 0;
  for(vidType src = 0; src < n_vertices; src++) {
    if(src % 100000 == 0) LOG("%u vertices halved", src);
    uint64_t &half_end = half_vertex[src + 1];
    half_end = half_vertex[src];
    const uint64_t begin = vertex[src], end = vertex[src + 1], src_deg = end - begin;
    for(uint64_t off = begin; off < end; off++) {
      vidType dst;
      edge.read(reinterpret_cast<char*>(&dst), sizeof(vidType));
      const uint64_t dst_deg = vertex[dst + 1] - vertex[dst];
      if(dst_deg > src_deg || (dst_deg == src_deg && dst > src)) {
        half_edge.write(reinterpret_cast<const char*>(&dst), sizeof(vidType));
        half_end++;
      }
    }
    half_max_degree = std::max(half_max_degree, half_vertex[src + 1] - half_vertex[src]);
  }
  LOG("%u vertices halved", n_vertices);
  if(half_vertex[n_vertices] * 2 != vertex[n_vertices]) {
    std::cerr << "MISMATCH: half_vertex[n_vertices]=" << half_vertex[n_vertices] << ", vertex[n_vertices]=" << vertex[n_vertices] << "\n";
  }
  std::ofstream half_meta((fname + ".half.meta.txt").c_str());
  half_meta << n_vertices << "\n" << half_vertex[n_vertices] << "\n" << sizeof(vidType) << "\n" << half_max_degree << "\n";
  half_meta.close();
  std::cout << "\ndone\n";
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "usage: ./convert <snap file>\n";
    return 1;
  }
  std::string fname = argv[1];
  halve(fname);
  return 0;
}

