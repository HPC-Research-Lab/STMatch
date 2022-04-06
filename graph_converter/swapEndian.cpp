#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include "def.h"
#include <omp.h>

void swap(std::string fname) {
  auto edgelist = MemoryMappedFile<uint8_t>::Modify(fname);
  constexpr uint64_t record_len = sizeof(vidType);
  uint64_t n_vals = edgelist.size() / record_len;
  #pragma omp parallel for schedule(dynamic,1024)
  for(uint64_t i = 0; i < n_vals; i++) {
    if(i % 1000000 == 0) LOG("processed %lu vals / %lu", i, n_vals);
    uint64_t off = i * record_len;
    std::swap(edgelist[off+0], edgelist[off+3]);
    std::swap(edgelist[off+1], edgelist[off+2]);
  }
  std::cout << "\ndone\n";
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "usage: ./convert <snap file>\n";
    return 1;
  }
  std::string fname = argv[1];
  swap(fname);
  return 0;
}

