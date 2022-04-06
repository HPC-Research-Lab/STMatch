#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <parallel/algorithm>
#include <parallel/numeric>
#include <cstring>
#include "def.h"

struct Edge {
  vidType src, dst;
public:
  bool operator<(const Edge &o) const {
    return src < o.src;
  }
  Edge rev() const {
    Edge r;
    r.src = dst;
    r.dst = src;
    return r;
  }
};

void makeLists(std::string fname) {
  const auto degree_map = MemoryMappedFile<uint64_t>::Read(fname + ".raw.degree.bin");
  vidType n_vertices = degree_map.size();
  std::vector<uint64_t> offset(n_vertices + 1);
  offset.at(0) = 0;
  std::cout << "starting scan\n" << std::flush;
  __gnu_parallel::partial_sum(degree_map.begin(), degree_map.end(), offset.data() + 1);
  std::cout << "done, writing offsets\n" << std::flush;
  std::ofstream raw_vertex((fname + ".raw.vertex.bin").c_str(), std::ios::binary);
  raw_vertex.write(reinterpret_cast<const char*>(offset.data()), (n_vertices + 1) * sizeof(uint64_t));
  raw_vertex.close();
  const auto edgelist_map = MemoryMappedFile<Edge>::Read(fname + ".bin");
  uint64_t n_edges = edgelist_map.size();
  const auto edgelist_rev_map = MemoryMappedFile<Edge>::Read(fname + ".rev.bin");
  auto edges = MemoryMappedFile<vidType>::Write(fname + ".raw.edge.bin", 2*n_edges);
  std::cout << "\ndone, starting adj list write\n" << std::flush;

  /*uint64_t off_out = 0, off_edge = 0, off_rev = 0;
  for(vidType v = 0; v < n_vertices; v++) {
    vidType v_be = __builtin_bswap32(v);
    assert(off_out == offset.at(v));
    if(v % 10000 == 0) LOG("%u vertices processed (%lu edges)", v, off_edge);
    while(off_edge < n_edges && edgelist_map[off_edge].src == v_be) {
      edges[off_out++] = __builtin_bswap32(edgelist_map[off_edge++].dst);
    }
    while(off_rev < n_edges && edgelist_rev_map[off_rev].src == v_be) {
      edges[off_out++] = __builtin_bswap32(edgelist_map[off_rev++].dst);
    }
    if(off_out != offset.at(v + 1)) {
      printf("\nexpected %lu, got %lu after vertex %u\n", offset.at(v + 1), off_out, v);
      exit(1);
    }
  }*/

  #pragma omp parallel for num_threads(omp_get_max_threads()) schedule(dynamic,1024)
  for(uint64_t e = 0; e < n_edges; e++) {
    if(e % 1000000 == 0) LOG("%lu edges processed", e);
    vidType src = __builtin_bswap32(edgelist_map[e].src);
    vidType dst = __builtin_bswap32(edgelist_map[e].dst);
    vidType src2 = __builtin_bswap32(edgelist_rev_map[e].src);
    vidType dst2 = __builtin_bswap32(edgelist_rev_map[e].dst);
    uint64_t idx, idx2;
    #pragma omp critical
    {
      idx = offset.at(src)++;
      idx2 = offset.at(src2)++;
    }
    edges[idx] = dst;
    edges[idx2] = dst2;
  }
  LOG("%lu edges processed", n_edges);
  std::cout << "\ndone\n" << std::flush;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "usage: ./convert <snap file>\n";
    return 1;
  }
  std::string fname = argv[1];
  makeLists(fname);
  return 0;
}

