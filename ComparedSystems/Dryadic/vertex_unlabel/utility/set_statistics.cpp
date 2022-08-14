#include <string>
#include <fstream>
#include <algorithm>
#include <tuple>
using namespace std;
#include <cassert>
#include <cstdio>

typedef uint32_t vidType;
vidType n_vertices, *edges;
uint64_t n_edges, *vertices;
void load_bin(const std::string prefix) {
	std::ifstream f_meta((prefix + ".meta.txt").c_str());
	assert(f_meta);
	int vid_size;
	vidType max_degree;
	f_meta >> n_vertices >> n_edges >> vid_size >> max_degree;
	assert(sizeof(vidType) == vid_size);
	f_meta.close();
	vertices = new uint64_t[n_vertices+1];
	edges = new vidType[n_edges];
	std::ifstream f_vertex((prefix + ".vertex.bin").c_str(), std::ios::binary);
	f_vertex.read(reinterpret_cast<char*>(vertices), (n_vertices+1)*sizeof(uint64_t));
	assert(f_vertex);
	f_vertex.close();
	std::ifstream f_edge((prefix + ".edge.bin").c_str(), std::ios::binary);
	f_edge.read(reinterpret_cast<char*>(edges), n_edges*sizeof(vidType));
	assert(f_edge);
	f_edge.close();
}

tuple<vidType, vidType, vidType, vidType> uid(uint64_t a_start, uint64_t a_end, uint64_t b_start, uint64_t b_end) {
  uint64_t a_idx = a_start;
  uint64_t b_idx = b_start;
  vidType si=0, sl=0, sr=0;
  while(a_idx < a_end && b_idx < b_end) {
    vidType na = edges[a_idx], nb = edges[b_idx];
    if(na < nb) {
      sl++;
      a_idx++;
    }
    if(nb < na) {
      sr++;
      b_idx++;
    }
    if(na == nb) {
      si++;
      a_idx++;
      b_idx++;
    }
  }
  sl += a_end - a_idx;
  sr += b_end - b_idx;
  return tuple<vidType, vidType, vidType, vidType>(si+sl+sr, si, sl, sr);
}

int main(int argc, char **argv) {
  assert(argc == 2);
  string prefix(argv[1]);
  load_bin(prefix);
  uint64_t s0=0, s1=0, s2=0, s3=0, s4=0, s5=0;
  uint64_t m0=0, m1=0, m2=0, m3=0, m4=0, m5=0;
  #pragma omp parallel for schedule(dynamic,64) reduction(+:s0) reduction(+:s1) reduction(+:s2) reduction(+:s3) reduction(+:s4) reduction(+:s5) reduction(max:m0) reduction(max:m1) reduction(max:m2) reduction(max:m3) reduction(max:m4) reduction(max:m5)
  for(vidType a = 0; a < n_vertices; a++) {
    uint64_t a_start = vertices[a];
    uint64_t a_end = vertices[a+1];
    s0 += a_end-a_start;
    m0 = max(m0, a_end-a_start);
    s5 += n_vertices-(a_end-a_start);
    m5 = max(m5, n_vertices-(a_end-a_start));
    for(uint64_t b_idx = a_start; b_idx < a_end; b_idx++) {
      vidType b = edges[b_idx];
      uint64_t b_start = vertices[b];
      uint64_t b_end = vertices[b+1];
      uint64_t su, si, sl, sr;
      tie(su, si, sl, sr) = uid(a_start, a_end, b_start, b_end);
      s1 += su;
      m1 = max(m1, su);
      s2 += si;
      m2 = max(m2, si);
      s3 += sl;
      m3 = max(m3, sl);
      s4 += sr;
      m4 = max(m4, sr);
    }
  }
  printf("%f\t%llu\n",(double)s0/n_vertices, m0);
  printf("%f\t%llu\n",(double)s1/n_edges, m1);
  printf("%f\t%llu\n",(double)s2/n_edges, m2);
  printf("%f\t%llu\n",(double)s3/n_edges, m3);
  printf("%f\t%llu\n",(double)s4/n_edges, m4);
  printf("%f\t%llu\n",(double)s5/n_vertices, m5);
}


