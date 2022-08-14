
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
    std::ofstream f_out((prefix+".turbup").c_str(),std::ios::out);
    std::ifstream f_in((prefix + ".upd").c_str());
    int vid_size=4;
    assert(sizeof(vidType) == vid_size);
    uint64_t nedges;
    f_in>>nedges;
    while(nedges-->0){
      vidType v0,v1;
      int boo;
      f_in>>v0>>v1>>boo;
      if(boo){
	f_out<<"e "<<v0<<" "<<v1<<" 0"<<std::endl;
	f_out<<"e "<<v1<<" "<<v0<<" 0"<<std::endl;
      }else{
	f_out<<"e -"<<1+v0<<" -"<<1+v1<<" 0"<<std::endl;
	f_out<<"e -"<<1+v1<<" -"<<1+v0<<" 0"<<std::endl;
      }
    }
    f_in.close();
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

