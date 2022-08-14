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
    vidType* L;
    vidType n_vertices, *edges;
    uint64_t n_edges, *vertices;
    std::ofstream f_out((prefix+".daf").c_str(),std::ios::out);
    std::ifstream f_meta((prefix + ".meta.txt").c_str());
    assert(f_meta);
    int vid_size;
    vidType max_degree;
    f_meta >> n_vertices >> n_edges >> vid_size >> max_degree;
    assert(sizeof(vidType) == vid_size);
    f_meta.close();
    read_file(prefix + ".vertex.bin", vertices, n_vertices+1);
    read_file(prefix + ".edge.bin", edges, n_edges);
    read_file(prefix + ".label.bin", L, n_vertices);
    f_out<<"t 0 "<<n_vertices<<std::endl;
    for(int i=0;i<n_vertices;++i){
      f_out<<"v "<<i<<" "<<L[i]<<std::endl;
    }
    for(int i=0;i<n_vertices;++i){
      for(int j=vertices[i];j<vertices[i+1];++j){
	uint64_t to = edges[j];
	if(to>i)
	  f_out<<"e "<<i<<" "<<to<<" 0"<<std::endl;
      }
    }
    f_out.close();
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

