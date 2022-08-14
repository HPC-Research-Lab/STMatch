#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include <cmath>
#include <iterator>
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

#ifdef batch
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
private: // memory managed regions for per-thread intermediates
  static thread_local std::vector<vidType*> buffers_exist, buffers_avail;
public:
  static void release_buffers();
  static vidType MAX_DEGREE;
  vidType *ptr;
private:
  const bool pooled;
  vidType set_size, vid;
public:
  VertexSet() : set_size(0), vid(-1), pooled(true) {
    if(buffers_avail.size() == 0) { 
      vidType *p = custom_alloc_local<vidType>(MAX_DEGREE);
      buffers_exist.push_back(p);
      buffers_avail.push_back(p);
    }
    ptr = buffers_avail.back();
    buffers_avail.pop_back();
  }
  VertexSet(vidType *p, vidType s, vidType id) : ptr(p), set_size(s), vid(id), pooled(false) {}
  VertexSet(const VertexSet&)=delete;
  VertexSet& operator=(const VertexSet&)=delete;
  VertexSet(VertexSet&&)=default;
  VertexSet& operator=(VertexSet&&)=default;
  ~VertexSet() {
    if(pooled) {
      buffers_avail.push_back(ptr);
    }
  }
  vidType size() const { return set_size; }
  VertexSet operator &(const VertexSet &other) const {
    VertexSet out;
    vidType idx_l = 0, idx_r = 0;
    while(idx_l < set_size && idx_r < other.set_size) {
      vidType left = ptr[idx_l];
      vidType right = other.ptr[idx_r];
      if(left <= right) idx_l++;
      if(right <= left) idx_r++;
      if(left == right) out.ptr[out.set_size++] = left;
    }
    return out;
  }

  void print() const {
    std::copy(ptr, ptr+set_size, std::ostream_iterator<vidType>(std::cout, " "));
  }
    

  vidType difference_buf(vidType *outBuf, const VertexSet &other) const {
#ifdef EDGE_INDUCED
    vidType ind = bs(ptr,set_size,other.vid);
    //not contained
    VertexSet out;
    for(vidType i=0;i<ind;++i){
      outBuf[i] = ptr[i];
    }
    vidType offset = 0;
    //we have other vid present
    if(ind!=set_size && ptr[ind]==other.vid) {
      offset = 1;
    }
    for(vidType i=ind;i<set_size-offset;++i){
      outBuf[i] = ptr[i+offset];
    }
    return set_size-offset;
#else
    vidType idx_l = 0, idx_r = 0;
    vidType return_set_size = 0;
    while(idx_l < set_size && idx_r < other.set_size) {
      vidType left = ptr[idx_l];
      vidType right = other.ptr[idx_r];
      if(left <= right) idx_l++;
      if(right <= left) idx_r++;
      if(left < right && left != other.vid) {
        assert(idx_l>return_set_size);
        outBuf[return_set_size++] = left;
      }
    }
    while(idx_l < set_size) {
      vidType left = ptr[idx_l];
      idx_l++;
      if(left != other.vid) {
        assert(idx_l>return_set_size);
        outBuf[return_set_size++] = left;
      }
    }
    return return_set_size;
#endif
  }

  VertexSet operator -(const VertexSet &other) const {
    VertexSet out;
    out.set_size = difference_buf(out.ptr, other); 
    return out;
//  		VertexSet out;
//     vidType idx_l = 0, idx_r = 0;
//     while(idx_l < set_size && idx_r < other.set_size) {
//       vidType left = ptr[idx_l];
//       vidType right = other.ptr[idx_r];
//       if(left <= right) idx_l++;
//       if(right <= left) idx_r++;
//       if(left < right && left != other.vid) out.ptr[out.set_size++] = left;
//     }
//     while(idx_l < set_size) {
//       vidType left = ptr[idx_l];
//       idx_l++;
//       if(left != other.vid) {
//         out.ptr[out.set_size++] = left;
//       }
//     }
//     return out; 
  }

  VertexSet& difference(VertexSet& dst, const VertexSet &other) {

//     std::cout << "difference with upper: inputs:\n";
//     this->print();
//     std::cout << "\n";
//     other.print();
//     std::cout << "\n";
    dst.set_size = difference_buf(dst.ptr, other);
//     std::cout << "output:\n";
//     dst.print();
//     std::cout << "\n";
    return dst;
  }

  VertexSet intersect(const VertexSet &other, vidType upper) const {
    VertexSet out;
    vidType idx_l = 0, idx_r = 0;
    while(idx_l < set_size && idx_r < other.set_size) {
      vidType left = ptr[idx_l];
      vidType right = other.ptr[idx_r];
      if(left >= upper) break;
      if(right >= upper) break;
      if(left <= right) idx_l++;
      if(right <= left) idx_r++;
      if(left == right) out.ptr[out.set_size++] = left;
    }
    return out;
  }
  vidType intersect_ns(const VertexSet &other, vidType upper) const {
    vidType idx_l = 0, idx_r = 0, idx_out = 0;
    while(idx_l < set_size && idx_r < other.set_size) {
      vidType left = ptr[idx_l];
      vidType right = other.ptr[idx_r];
      if(left >= upper) break;
      if(right >= upper) break;
      if(left <= right) idx_l++;
      if(right <= left) idx_r++;
      if(left == right) idx_out++;
    }
    return idx_out;
  }

  //outBuf may be the same as this->ptr
  vidType difference_buf(vidType *outBuf, const VertexSet &other, vidType upper) {
#ifdef EDGE_INDUCED
    vidType indo = bs(ptr,set_size,other.vid);
    vidType indu = bs(ptr,set_size,upper);
    //not contained
    VertexSet out;
    if(indu<=indo){
      for(vidType i=0;i<indu;++i){
	outBuf[i] = ptr[i];
      }
      return indu;
    }
    for(vidType i=0;i<indo;++i){
      outBuf[i] = ptr[i];
    }
    //else, we go up to but not including indu anyway, but that is after indo
    vidType offset = 0;
    //we have other vid present
    if(indo!=set_size && ptr[indo]==other.vid) {
      offset = 1;
    }
    //should actually just be indu TODO verify this
    vidType stop = std::min(indu,set_size);//we go up to, but not including indu
    out.set_size = stop-offset;
    for(vidType i=indo;i<stop-offset;++i){
      outBuf[i] = ptr[i+offset];
    }
    return stop-offset;
#else
    vidType idx_l = 0, idx_r = 0;
    vidType return_set_size = 0;
    while(idx_l < set_size && idx_r < other.set_size) {
      vidType left = ptr[idx_l];
      vidType right = other.ptr[idx_r];
      if(left >= upper) break;
      if(right >= upper) break;
      if(left <= right) idx_l++;
      if(right <= left) idx_r++;
      if(left < right && left != other.vid) outBuf[return_set_size++] = left;
    }
    while(idx_l < set_size) {
      vidType left = ptr[idx_l];
      if(left >= upper) break;
      idx_l++;
      if(left != other.vid) {
        outBuf[return_set_size++] = left;
      }
    }
    return return_set_size;
#endif
  }

  VertexSet difference(const VertexSet &other, vidType upper) {
    VertexSet out;
    out.set_size = difference_buf(out.ptr, other, upper);
    return out;
  }

  VertexSet& difference(VertexSet& dst, const VertexSet &other, vidType upper) {
    dst.set_size = difference_buf(dst.ptr, other, upper);
    return dst;
  }

  vidType difference_ns(const VertexSet &other, vidType upper) const {
#ifdef EDGE_INDUCED
    vidType indo = bs(ptr,set_size,other.vid);
    vidType indu = bs(ptr,set_size,upper);
    //other.vid is not in the bounded set

    if(indu<=indo||indo==set_size||ptr[indo]!=other.vid)return indu;
    else return indu-1;
#else    
    vidType idx_l = 0, idx_r = 0, idx_out = 0;
    while(idx_l < set_size && idx_r < other.set_size) {
      vidType left = ptr[idx_l];
      vidType right = other.ptr[idx_r];
      if(left >= upper) break;
      if(right >= upper) break;
      if(left <= right) idx_l++;
      if(right <= left) idx_r++;
      if(left < right && left != other.vid) idx_out++;
    }
    while(idx_l < set_size) {
      vidType left = ptr[idx_l];
      if(left >= upper) break;
      idx_l++;
      if(left != other.vid) {
        idx_out++;
      }
    }
    return idx_out;
#endif
  }

  VertexSet bounded(vidType up) const{

    if(set_size > 64){
      vidType idx_l = -1;
      vidType idx_r = set_size;
      while(idx_r-idx_l > 1){
        vidType idx_t = (idx_l+idx_r)/2;
        if(ptr[idx_t] < up)idx_l = idx_t;
        else idx_r = idx_t;
      }
      return VertexSet(ptr,idx_l+1,vid);
    }else{
      vidType idx_l = 0;

      while(idx_l < set_size && ptr[idx_l] < up) ++idx_l;
      return VertexSet(ptr,idx_l,vid);
    }
  }
  const vidType *begin() const { return ptr; }
  const vidType *end() const { return ptr+set_size; }
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
  Graph(std::string prefix) {//: scale_accesses(30, 0) {
    //size_t pos;
    //if((pos = prefix.rfind("_b")) != std::string::npos && pos == prefix.size()-2) {
      VertexSet::release_buffers();
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
  VertexSet N(vidType vid) {
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
  }
  vidType V() { return n_vertices; }
  size_t E() { return n_edges; }
};

// VertexSet static members
thread_local std::vector<vidType*> VertexSet::buffers_exist(0);
thread_local std::vector<vidType*> VertexSet::buffers_avail(0);
vidType VertexSet::MAX_DEGREE = 1;
void VertexSet::release_buffers() {
  buffers_avail.clear();
  while(buffers_exist.size() > 0) {
    delete[] buffers_exist.back();
    buffers_exist.pop_back();
  }
}


VertexSet difference_set(const VertexSet& a, const VertexSet& b){
  return a-b;
}

VertexSet& difference_set(VertexSet& dst, const VertexSet& a, const VertexSet& b){
  return a.difference(dst, b);
}

VertexSet difference_set(const VertexSet& a, const VertexSet& b,vidType up){
  return a.difference(b,up);
}

VertexSet& difference_set(VertexSet& dst, const VertexSet& a, const VertexSet& b,vidType up){
  return a.difference(dst, b,up);
}

uint64_t difference_num(const VertexSet& a, const VertexSet& b){
  return (a-b).size();
}
uint64_t difference_num(const VertexSet& a, const VertexSet& b,vidType up){
  return a.difference_ns(b,up);
}
VertexSet intersection_set(const VertexSet& a, const VertexSet& b){
  return a & b;
}
VertexSet intersection_set(const VertexSet& a, const VertexSet& b,vidType up){
  return a.intersect(b,up);
}
uint64_t intersection_num(const VertexSet& a, const VertexSet& b){
  return (a & b).size();
}
uint64_t intersection_num(const VertexSet& a, const VertexSet& b,vidType up){
  return a.intersect_ns(b,up);
}
VertexSet bounded(const VertexSet&a ,vidType up){
  return a.bounded(up);
}
