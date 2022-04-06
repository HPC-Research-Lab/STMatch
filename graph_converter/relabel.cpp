#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cstring>
#include "def.h"
#include <omp.h>

void relabel(std::string fname) {
  const auto vertex_in = MemoryMappedFile<uint64_t>::Read(fname + ".vertex.bin");
  const auto edge_in = MemoryMappedFile<vidType>::Read(fname + ".edge.bin");
  const auto meta_in = MemoryMappedFile<char>::Read(fname + ".meta.txt");
  const vidType n_vertices = vertex_in.size() - 1;
  auto vertex_out = MemoryMappedFile<uint64_t>::Write(fname + ".ord.vertex.bin", n_vertices+1);
  auto edge_out = MemoryMappedFile<vidType>::Write(fname + ".ord.edge.bin", edge_in.size());
  auto meta_out = MemoryMappedFile<char>::Write(fname + ".ord.meta.txt", meta_in.size());

  vertex_in.fault_in();
  edge_in.fault_in();
  vertex_out.fault_in();
  edge_out.fault_in();

  memcpy(meta_out.ptr(), meta_in.ptr(), meta_in.size());
  std::cout << "Determining new id order\n" << std::flush;
  std::vector<std::pair<vidType, vidType>> deg_id(n_vertices);
  TimeStamp t1;
  #pragma omp parallel for
  for(vidType v = 0; v < n_vertices; v++) {
    vidType deg = vertex_in[v + 1] - vertex_in[v];
    deg_id[v].first = deg;
    deg_id[v].second = v;
  }
  std::sort(deg_id.begin(), deg_id.end(), std::greater<>());
  std::vector<vidType> label(n_vertices, 0);
  #pragma omp parallel for
  for(vidType i = 0; i < n_vertices; i++) {
    vidType v = deg_id[i].second;
    label.at(v) = i;
    vertex_out[i + 1] = deg_id[i].first;
  }
  //printf("high=%u low=%u\n", deg_id[0].first, deg_id[n_vertices-1].first);
  std::cout << "Beginning scan\n" << std::flush;
  vertex_out[0] = 0;
  std::partial_sum(vertex_out.ptr(1), vertex_out.ptr(n_vertices+1), vertex_out.ptr(1));
  assert(vertex_out[n_vertices] == vertex_in[n_vertices]);
  //printf("off[0]=%lu off[1]=%lu off[%u]=%lu\n", vertex_out[0], vertex_out[1], n_vertices, vertex_out[n_vertices]);
  std::cout << "Beginning relabel\n" << std::flush;
  #pragma omp parallel for schedule(dynamic,1024)
  for(vidType new_v = 0; new_v < n_vertices; new_v++) {
    if(new_v % 100000 == 0) LOG("%u vertices relabeled", new_v);
    vidType old_v = deg_id[new_v].second;
    assert(label.at(old_v) == new_v);
    uint64_t new_begin = vertex_out[new_v];
    vidType new_deg = vertex_out[new_v + 1] - new_begin;
    uint64_t old_begin = vertex_in[old_v];
    vidType old_deg = vertex_in[old_v + 1] - old_begin;
    assert(new_deg == old_deg);
    for(uint64_t off = 0; off < new_deg; off++) {
      edge_out[new_begin + off] = label.at(edge_in[old_begin + off]);
    }
    std::sort(edge_out.ptr(new_begin), edge_out.ptr(new_begin + new_deg));
  }
  LOG("%u vertices relabeled", n_vertices);
  TimeStamp t2;
  meta_out.sync();
  vertex_out.sync();
  edge_out.sync();
  printf("\ndone, %f seconds\n", t2-t1);
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "usage: ./convert <snap file>\n";
    return 1;
  }
  std::string fname = argv[1];
  relabel(fname);
  return 0;
}

