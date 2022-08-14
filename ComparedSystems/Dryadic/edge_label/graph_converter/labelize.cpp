#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <parallel/numeric>
#include <cstring>
#include "def.h"

template<typename T>
T* custom_alloc_local(size_t elements) {
  return new T[elements];
}
template<typename T>
T* custom_alloc_global(size_t elements) {
  return new T[elements];
}
template<typename T>
void custom_free(T *ptr, size_t elements) {
  delete[] ptr;
}
template<typename T>
static void read_file(std::string fname, T *& pointer, size_t elements) {
  pointer = custom_alloc_global<T>(elements);
  assert(pointer);
  std::ifstream inf(fname.c_str(), std::ios::binary);
  if(!inf.good()) {
    std::cerr << "Failed to open file: " << fname << "\n";
    exit(1);
  }
  inf.read(reinterpret_cast<char*>(pointer), sizeof(T) * elements);
  inf.close();
}
typedef uint32_t vidType; // type of vertex id fields, 32-bit works up to 4 bill
void convert(std::string prefix) {

  vidType n_vertices, *edges;
  uint64_t n_edges, *vertices;
  std::srand(0);
  std::ifstream f_meta((prefix + ".meta.txt").c_str());
  assert(f_meta);
  int vid_size;
  vidType max_degree;
  f_meta >> n_vertices >> n_edges >> vid_size >> max_degree;
  assert(sizeof(vidType) == vid_size);
  f_meta.close();
  read_file(prefix + ".vertex.bin", vertices, n_vertices+1);
  auto labels = MemoryMappedFile<uint32_t>::Write(prefix + ".label.bin", n_vertices);
  for(int i=0;i<n_vertices;++i){
    labels[i]=std::rand()%10;
  }
    
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "usage: ./convtoturb <snap file>\n";
    return 1;
  }
  std::string fname = argv[1];
  convert(fname);
  return 0;
}

