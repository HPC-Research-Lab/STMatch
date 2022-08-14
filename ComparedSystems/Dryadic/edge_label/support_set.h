#pragma once
#include <vector>
#include <map>
#include <cmath>
#include <iterator>
#include <set>
#include <memory>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <cassert>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#ifdef USE_NUMA
  #include <numa.h>
  template<typename T>
  T* custom_alloc_local(size_t elements) {
    return (T*)numa_alloc_local(sizeof(T) * elements);
  }
  template<typename T>
  T* custom_alloc_global(size_t elements) {
    return (T*)numa_alloc_interleaved(sizeof(T) * elements);
  }
  template<typename T>
  void custom_free(T *ptr, size_t elements) {
    numa_free(ptr, sizeof(T)*elements);
  }
#else
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
#endif

typedef uint32_t vidType; // type of vertex id fields, 32-bit works up to 4 billion vertex graphs
constexpr bool map_edges = false; // use mmap() instead of read()
constexpr bool map_vertices = false; // use mmap() instead of read()

class ChangeList{
public:
  std::vector<std::tuple<vidType,vidType>> el;
  std::unordered_map<vidType, std::unordered_map<vidType,bool>*> changes;
  ChangeList(char* fileName): changes(),el(){
    //go through list, add changes and all the things that are required.
    std::ifstream inf(fileName);
    //preferably have the list be in plain text to make things easier to deal with
    vidType n; //number of added edges
    inf>>n;
    for(vidType i=0;i<n;++i){
      vidType a,b;
      int add;
      inf>>a>>b>>add;
      
      bool ad = (bool)add;
      //std::cout<<a<<" "<<b<<" "<<ad;
      if(0==changes.count(a))changes.emplace(a,new std::unordered_map<vidType,bool>());
      if(0==changes.count(b))changes.emplace(b,new std::unordered_map<vidType,bool>());
      el.push_back(std::make_pair(a,b));
      changes[b]->emplace(a,ad);
      changes[a]->emplace(b,ad);
    }
    inf.close();
  }
};
//return 0 if no edge, 1 if add, -1 if remove
inline int stat(ChangeList& cl,vidType a, vidType b){
  return cl.changes.count(a)!=0&&cl.changes[a]->count(b)!=0?
    (cl.changes[a]->at(b)?
     1:
     -1)
    :0;
}
//
inline bool comp(vidType a, vidType b, vidType x, vidType y){
  vidType a1 = std::min(a,b);
  vidType b1 = std::max(a,b);
  vidType x1 = std::min(x,y);
  vidType y1 = std::max(x,y);
  return a1<x1|| (a1==x1 && b1<y1);
}
#ifdef batch

#endif

#ifdef EDGE_INDUCED
inline vidType bs(vidType* ptr, vidType set_size, vidType o){
  vidType idx_l = -1;
  vidType idx_r = set_size;
  //guarantees in this area is that idx_l is before where we would put o 
  while(idx_r-idx_l > 1){
    vidType idx_t = (idx_l+idx_r)/2;
    if(ptr[idx_t] < o)idx_l = idx_t;
    else idx_r = idx_t;
  }
  return idx_l+1;
}
#endif
// Canonical labeling tuples
template<int div>
int compute_label(vidType id) {
  return id % div;
}
template<int div>
inline std::tuple<int, int> canon(vidType v0, vidType v1) {
  int l0 = compute_label<div>(v0);
  int l1 = compute_label<div>(v1);
  if(l0 > l1) std::swap(l0, l1);
  return std::make_tuple(l0, l1);
}
template<int div>
inline std::tuple<int, int, int> canon(vidType v0, vidType v1, vidType v2) {
  int l0 = compute_label<div>(v0);
  int l1 = compute_label<div>(v1);
  int l2 = compute_label<div>(v2);
  if(l0 > l1) std::swap(l0, l1);
  if(l0 > l2) std::swap(l0, l2);
  if(l1 > l2) std::swap(l1, l2);
  return std::make_tuple(l0, l1, l2);
}
template<int div>
inline std::tuple<int, int, int, int> canon(vidType v0, vidType v1, vidType v2, vidType v3) {
  int l0 = compute_label<div>(v0);
  int l1 = compute_label<div>(v1);
  int l2 = compute_label<div>(v2);
  int l3 = compute_label<div>(v3);
  if(l0 > l1) std::swap(l0, l1);
  if(l0 > l2) std::swap(l0, l2);
  if(l0 > l3) std::swap(l0, l3);
  if(l1 > l2) std::swap(l1, l2);
  if(l1 > l3) std::swap(l1, l3);
  if(l2 > l3) std::swap(l2, l3);
  return std::make_tuple(l0, l1, l2, l3);
}

class Graph;

class VertexSet {
public:
  static vidType MAX_DEGREE;
  std::set<vidType> set;
private:
  vidType vid;
public:
  VertexSet() : set(),vid(-1) {}
  VertexSet(vidType* f, vidType* l, vidType id) : set(f,l), vid(id) {}
  VertexSet(std::set<vidType>::iterator f, std::set<vidType>::const_iterator l) : set(f,l), vid(-1) {}
  VertexSet(const VertexSet&)=default;
  VertexSet& operator=(const VertexSet&)=delete;
  VertexSet(VertexSet&&)=default;
  VertexSet& operator=(VertexSet&&)=default;
  ~VertexSet() {
  }
  vidType size() const { return set.size(); }
  std::set<vidType>::iterator begin(){return set.begin();}
  const std::set<vidType>::iterator end(){return set.end();}
  VertexSet intersect(const VertexSet &other){
     VertexSet out;
     std::vector<vidType> res(std::min(set.size(),other.set.size())); 
     std::vector<vidType>::iterator it=
      std::set_intersection(set.begin(),set.end(),
			    other.set.begin(),other.set.end(),
			    res.begin());
    out.set.insert(res.begin(),it);
    return out;
  }
  
  VertexSet intersect(const VertexSet &other, vidType upper) const {
    VertexSet out;
    std::vector<vidType> res(std::min(set.size(),other.set.size()));
    std::vector<vidType>::iterator it=
      std::set_intersection(set.begin(),set.lower_bound(upper),
			    other.set.begin(),other.set.lower_bound(upper),
			    res.begin());
    out.set.insert(res.begin(),it);
    return out;
  }
  
  vidType intersect_ns(const VertexSet &other, vidType upper) const {
    return intersect(other,upper).set.size();
  }

  
  VertexSet difference(const VertexSet &other) const {
#ifdef EDGE_INDUCED
    std::set<vidType>::iterator ub,lb; 
    lb=set.lower_bound(other.vid);
    ub=set.upper_bound(other.vid);
    VertexSet out;
    if(ub==lb){
      out.set.insert(set.begin(),set.end());
    }
    else{
      out.set.insert(set.begin(),lb);
      out.set.insert(ub,set.end());
    }
    return out;
#else
    VertexSet out;
    std::vector<vidType> res(set.size());
    std::vector<vidType>::iterator it=
      std::set_difference(set.begin(),set.end(),
			  other.set.begin(),other.set.end(),
			  res.begin());
    out.set.insert(res.begin(),it);
    return out;
#endif
  }

  VertexSet difference(const VertexSet &other, vidType upper) {
#ifdef EDGE_INDUCED
    if(upper>other.vid){
      std::set<vidType>::iterator ub,lb; 
      lb=set.lower_bound(other.vid);
      ub=set.upper_bound(other.vid);
      VertexSet out;
      if(ub==lb){
	out.set.insert(set.begin(),set.end());
      }
      else{
	out.set.insert(set.begin(),lb);
	out.set.insert(ub,set.lower_bound(upper));
      } 
      return out;
    }else{
      return bounded(upper);
    }
#else
    VertexSet out;
    std::vector<vidType> res(set.size());
    std::vector<vidType>::iterator it=
      std::set_difference(set.begin(),set.lower_bound(upper),
			  other.set.begin(),other.set.lower_bound(upper),
			  res.begin());
    out.set.insert(res.begin(),it);
    return out;
#endif
  }
  vidType difference_ns(const VertexSet &other, vidType upper) const {
    return difference(other,upper).set.size();
  }
  VertexSet bounded(vidType up) const{
    return VertexSet(set.begin(),set.lower_bound(up));
  }
  friend class Graph;
};
 
/*
 * edges array contains successor lists
 * vertices array contains offsets
 * Vertex v successors list:
 * begin at edges[vertices[v]]
 * end at edges[vertices[v+1]]
 * lists are assumed to be pre-sorted
 */
class Graph {
private:
  vidType n_vertices, *edges;
  uint64_t n_edges, *vertices;
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
  template<typename T>
  static void map_file(std::string fname, T *& pointer, size_t elements) {
    int inf = open(fname.c_str(), O_RDONLY, 0);
    if(-1 == inf) {
      std::cerr << "Failed to open file: " << fname << "\n";
      exit(1);
    }
    pointer = (T*)mmap(nullptr, sizeof(T) * elements,
                       PROT_READ, MAP_SHARED, inf, 0);
    assert(pointer != MAP_FAILED);
    close(inf);
  }
  //std::vector<uint64_t> scale_accesses;
public:
  #ifdef LABELLED
  vidType* L;
#endif
  std::vector<VertexSet> vs;
  Graph(std::string prefix) {//: scale_accesses(30, 0) {
    //size_t pos;
    //if((pos = prefix.rfind("_b")) != std::string::npos && pos == prefix.size()-2) {
      std::ifstream f_meta((prefix + ".meta.txt").c_str());
      assert(f_meta);
      int vid_size;
      vidType max_degree;
      f_meta >> n_vertices >> n_edges >> vid_size >> max_degree;
      assert(sizeof(vidType) == vid_size);
      VertexSet::MAX_DEGREE = std::max(max_degree, VertexSet::MAX_DEGREE);
      f_meta.close();
      if(map_vertices) map_file(prefix + ".vertex.bin", vertices, n_vertices+1);
      else read_file(prefix + ".vertex.bin", vertices, n_vertices+1);
      if(map_edges) map_file(prefix + ".edge.bin", edges, n_edges);
      else read_file(prefix + ".edge.bin", edges, n_edges);
#ifdef LABELLED
      read_file(prefix + ".label.bin", L, n_vertices);
#endif
      for(int i=0;i<n_vertices;++i){
	vs.emplace_back(edges+vertices[i],edges+vertices[i+1],i);
      }
      //} else {
    //std::cerr << "Expected filename ending with _b, got: " << prefix << "\n";
    //exit(1);
    //}
  }
  ~Graph() {
    if(map_edges) {
      munmap(edges, n_edges*sizeof(vidType));
    } else {
      custom_free(edges, n_edges);
    }
    if(map_vertices) {
      munmap(vertices, (n_vertices+1)*sizeof(uint64_t));
    } else {
      custom_free(vertices, n_vertices+1);
    }
    /*for(int i = 0; i < 30; i++) {
      printf("%d\t%d\t%lu\n", i, 1<<i, scale_accesses.at(i));
    }*/
  }
  Graph(const Graph &)=delete;
  Graph& operator=(const Graph &)=delete;
  VertexSet& N(vidType vid) {
    return vs[vid];
    /*
    assert(vid >= 0);
    
    assert(vid < n_vertices);
    uint64_t begin = vertices[vid], end = vertices[vid+1];
    //scale_accesses.at((int)ceil(log2(end-begin)))++;
    assert(begin >= 0);
    if(begin > end) {
      fprintf(stderr, "vertex %u bounds error: [%lu, %lu)\n", vid, begin, end);
      exit(1);
    }
    assert(end <= n_edges);
    return VertexSet(edges + begin, end - begin, vid);
    */
  }
  vidType V() { return n_vertices; }
  size_t E() { return n_edges; }
};

// VertexSet static members
vidType VertexSet::MAX_DEGREE = 1;

VertexSet difference_set(const VertexSet& a, const VertexSet& b){
  return a.difference(b);
}

VertexSet difference_set(const VertexSet& a, const VertexSet& b,vidType up){
  return a.difference(b,up);
}

VertexSet& difference_set(VertexSet& dst,const VertexSet& a, const VertexSet& b){
  return dst=a.difference(b);
}

VertexSet& difference_set(VertexSet& dst,const VertexSet& a, const VertexSet& b,vidType up){
  return dst=a.difference(b,up);
}

uint64_t difference_num(const VertexSet& a, const VertexSet& b){
  return a.difference(b).size();
}
uint64_t difference_num(const VertexSet& a, const VertexSet& b,vidType up){
  return a.difference_ns(b,up);
}
VertexSet intersection_set(const VertexSet& a, const VertexSet& b){
  return a.intersect(b);
}
VertexSet intersection_set(const VertexSet& a, const VertexSet& b,vidType up){
  return a.intersect(b,up);
}
uint64_t intersection_num(const VertexSet& a, const VertexSet& b){
  return a.intersect(b).size();
}
uint64_t intersection_num(const VertexSet& a, const VertexSet& b,vidType up){
  return a.intersect_ns(b,up);
}
VertexSet bounded(const VertexSet&a ,vidType up){
  return a.bounded(up);
}
