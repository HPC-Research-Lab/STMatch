#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include "def.h"
#include <omp.h>

void index_half(std::string fname) {
  printf("index_half\n");
  const auto vertex_in = MemoryMappedFile<uint64_t>::Read(fname + ".vertex.bin");
  const auto edge_in = MemoryMappedFile<vidType>::Read(fname + ".edge.bin");
  const vidType n_vertices = vertex_in.size() - 1;
  auto ends_out = MemoryMappedFile<uint64_t>::Write(fname + ".ends_half.bin", n_vertices);
  uint64_t edge_count = 0;
  #pragma omp parallel for schedule(dynamic,1024) reduction(+:edge_count)
  for(vidType vid = 0; vid < n_vertices; vid++) {
    if(vid % 100000) LOG("%u vertices processed", vid);
    uint64_t begin = vertex_in[vid], end = vertex_in[vid + 1];
    const vidType *upper = std::upper_bound(edge_in.ptr(begin), edge_in.ptr(end), vid);
    ends_out[vid] = upper - edge_in.ptr(0);
    edge_count += upper - edge_in.ptr(begin);
  }
  assert(edge_count * 2 == edge_in.size());
  printf("\ndone\n");
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "usage: ./convert <snap file>\n";
    return 1;
  }
  std::string fname = argv[1];
  index_half(fname);
  return 0;
}

