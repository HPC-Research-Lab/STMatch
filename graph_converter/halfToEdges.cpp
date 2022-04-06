#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <parallel/numeric>
#include <cstring>
#include "def.h"
#include <omp.h>

void halfToEdge(std::string fname) {
  const auto half_vertex = MemoryMappedFile<uint64_t>::Read(fname + ".half.vertex.bin");
  const vidType n_vertices = half_vertex.size() - 1;
  const auto half_edge = MemoryMappedFile<uint32_t>::Read(fname + ".half.edge.bin");

  std::ofstream outfile((fname + ".bin").c_str(), std::ios::binary);
  std::ofstream out_rev((fname + ".rev.bin").c_str(), std::ios::binary);
  auto degrees = MemoryMappedFile<uint64_t>::Write(fname + ".raw.degree.bin", n_vertices);
  std::memset(degrees.begin(), 0, sizeof(uint64_t)*n_vertices);

	vidType edge[2];
  uint64_t last_begin = 0;
  for(vidType src = 0; src < n_vertices; src++) {
    uint64_t begin = half_vertex[src], end = half_vertex[src+1];
    degrees[src] += end - begin;
    if(begin - last_begin >= 1000000) {
      LOG("%lu edges written", begin);
      last_begin = begin;
    }
    for(uint64_t off = begin; off < end; off++) {
      vidType dst = half_edge[off];
      degrees[dst]++;
      edge[0] = __builtin_bswap32(src); // conversion to big-endian
      edge[1] = __builtin_bswap32(dst); // conversion to big-endian
      outfile.write(reinterpret_cast<const char*>(edge), 2*sizeof(vidType));
      std::swap(edge[0], edge[1]);
      out_rev.write(reinterpret_cast<const char*>(edge), 2*sizeof(vidType));
    }
  }
  LOG("%u edges written, DONE", n_vertices);
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "usage: ./convert <snap file>\n";
    return 1;
  }
  std::string fname = argv[1];
  halfToEdge(fname);
  return 0;
}

