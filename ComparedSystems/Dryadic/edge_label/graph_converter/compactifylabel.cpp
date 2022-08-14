#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <parallel/numeric>
#include <cstring>
#include "def.h"

void compactify(std::string fname) {
  //required that there is no duplicates in the graph, this is guaranteed by compactify which makes these files
  const auto raw_vertex = MemoryMappedFile<uint64_t>::Read(fname + ".vertex.bin");
  auto raw_edge = MemoryMappedFile<vidType>::Read(fname + ".edge.bin");
  const auto raw_label = MemoryMappedFile<vidType>::Read(fname + ".label.bin");
  uint64_t n_edges = raw_edge.size();
  vidType n_vertices = raw_vertex.size() - 1;
  
  auto edge = MemoryMappedFile<vidType>::Write(fname + ".edge.label.bin", n_edges);
  //copy edges to a new file
#pragma omp parallel for schedule(dynamic,64)
  for(vidType v = 0; v < n_vertices; v++) {
    if(v % 100000 == 0) LOG("About %u vertices copied to new set", v);
    uint64_t out_low = raw_vertex[v], out_high = raw_vertex[v + 1];
    vidType deg = out_high - out_low;
    memcpy(edge.ptr(out_low), raw_edge.ptr(out_low), deg * sizeof(vidType));
  }
  //resort the edges, fill out the new degree array
  labelType n_labels = 0;
  for(int i=0;i<n_vertices;++i){
    n_labels = std::max(n_labels,1+raw_label[i]);
  }
  std::vector<std::vector<vidType>> labs(n_labels);
  for(int i=0;i<n_vertices;++i){
    labs[raw_label[i]].push_back(i);
  }
  uint64_t max_degree = 0;
  std::vector<uint64_t> degree(n_vertices*n_labels);
#pragma omp parallel for schedule(dynamic,64) reduction(max:max_degree)
  for(vidType v = 0; v < n_vertices; v++) {
    if(v % 100000 == 0) LOG("%u vertices sorted", v);
    uint64_t low = raw_vertex[v], high = raw_vertex[v + 1];
    auto start = edge.ptr(low);
    auto stop = edge.ptr(high);
    std::sort(start, stop,
	      //lambda sort method
	      [&](vidType a,vidType b) -> bool {
		//start by sorting by label, then sort by vid
		if(raw_label[a]!=raw_label[b])
		  return raw_label[a]<raw_label[b];
		return a<b;});
    auto curr = start;
    //how many are in each set is degree/
    for(int i=0;i<n_labels;++i){
      auto next = curr;
      while(next!=stop && raw_label[*next]==i){++next;}
      degree.at(v*n_labels+i) = next-curr;
      max_degree = std::max(max_degree,degree.at(v*n_labels+i));
      curr=next;
    }
  }
  std::cout << "\nStarting scan\n" << std::flush;
  auto vertex = MemoryMappedFile<uint64_t>::Write(fname + ".vertexset.label.bin", n_vertices*n_labels+1);
  vertex[0] = 0;
  __gnu_parallel::partial_sum(degree.begin(), degree.end(), vertex.ptr(1));
  LOG("%u vertices processed", n_vertices);
  //vertices grouped by labels
  auto vlist = MemoryMappedFile<vidType>::Write(fname+".label.vertices.bin",n_vertices);
  auto vlablist = MemoryMappedFile<vidType>::Write(fname+".label.indices.bin",n_labels+1);
  vidType currin=0;
  vlablist[0]=0;
  for(int i=0;i<n_labels;++i){
    std::copy(labs[i].begin(),labs[i].end(),vlist.ptr(currin));
    currin += labs[i].size();
    vlablist[i+1]=currin;
  }
  
 
  std::ofstream meta((fname + ".meta.label").c_str());
  //max degree is different for the labelled version  
  meta << n_labels << "\n" << max_degree << "\n";
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

