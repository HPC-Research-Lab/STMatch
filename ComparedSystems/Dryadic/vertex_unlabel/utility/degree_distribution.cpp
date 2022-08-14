#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

#include <cstdio>
#include <cassert>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>

#include <omp.h>

typedef uint32_t vidType;

size_t filesize(std::string fname) {
  struct stat st;
  stat(fname.c_str(), &st);
  return st.st_size;
}

std::string human_size(double len, int base=1024) {
  std::vector<std::string> sizes = { "", "K", "M", "G", "T" };
  int order = 0;
  while(len >= base && order < sizes.size() - 1) {
    order++;
    len = len/base;
  }
  std::ostringstream oss;
  oss << std::setprecision(3) << len << sizes[order];
  return oss.str();
}

template<typename T>
T sum(const std::vector<T> &vec) {
  T s = 0;
  for(const T &v : vec) s += v;
  return s;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    fprintf(stderr, "usage: ./dist <graph prefix>\n");
    exit(1);
  }

  std::string prefix(argv[1]);
  std::ifstream f_meta((prefix + ".meta.txt").c_str(), std::ios::binary);
  assert(f_meta);
  vidType n_vertices;
  uint64_t n_edges;
  int vt_bytes;
  f_meta >> n_vertices >> n_edges >> vt_bytes;
  assert(vt_bytes == sizeof(vidType));
  f_meta.close();

  printf("%s\n%s vertices, %s edges\n%sB vertex data, %sB edge data\n\n", argv[1],
         human_size(n_vertices, 1000).c_str(),
         human_size(n_edges, 1000).c_str(),
         human_size(n_vertices*sizeof(uint64_t)).c_str(),
         human_size(n_edges*sizeof(vidType)).c_str());

  const size_t fv_size = (n_vertices+1)*sizeof(uint64_t);
  assert(filesize(prefix + ".vertex.bin") == fv_size);
  int f_vertex = open((prefix + ".vertex.bin").c_str(), O_RDONLY, 0);
  assert(f_vertex);
  const size_t fe_size = n_edges*sizeof(vidType);
  assert(filesize(prefix + ".edge.bin") == fe_size);
  int f_edge = open((prefix + ".edge.bin").c_str(), O_RDONLY, 0);
  assert(f_edge);

  /*uint64_t *vertices = (uint64_t*)mmap(nullptr, fv_size, PROT_READ, MAP_SHARED, f_vertex, 0);
  assert(vertices != MAP_FAILED);
  vidType *edges = (vidType*)mmap(nullptr, fe_size, PROT_READ, MAP_SHARED, f_edge, 0);
  assert(edges != MAP_FAILED);*/
  uint64_t *vertices = new uint64_t[n_vertices+1];
  assert(vertices);
  vidType *edges = new vidType[n_edges];
  assert(edges);
  size_t fv_pos = 0;
  while(fv_pos < fv_size) {
    int bytes;
    bytes = read(f_vertex, vertices, fv_size-fv_pos);
    assert(bytes > 0);
    fv_pos += bytes;
  }
  size_t fe_pos = 0;
  while(fe_pos < fe_size) {
    int bytes;
    bytes = read(f_edge, edges, fe_size);
    assert(bytes > 0);
    fe_pos += bytes;
  }

  uint64_t degree_n=0, degree_sum=0;
  double degree_mean=0, degree_M2=0;
  for(vidType v = 0; v < n_vertices; v++) {
    uint64_t val = vertices[v+1] - vertices[v];
    degree_n++;
    degree_sum += val;
    double delta = val - degree_mean;
    degree_mean += delta / degree_n;
    degree_M2 += delta * (val - degree_mean);
  }
  double degree_variance = degree_M2 / degree_n;
  double degree_std_dev = sqrt(degree_variance);

  printf("degree statistics:\naverage = %f\nstd_dev = %f\n", degree_mean, degree_std_dev);

  const std::vector<double> factors = { 0, 0.5, 1, 1.5, 2, 2.5, 3.0, 5.0, 10.0 };
  const size_t nf = factors.size();
  const std::vector<uint64_t> seed(omp_get_max_threads());
  std::vector<uint64_t> threshold(nf);
  std::vector<std::vector<uint64_t>> count_ll(nf, seed), count_lh(nf, seed), count_hh(nf, seed),
                                     count_low(nf, seed), count_high(nf, seed),
                                     storage_low(nf, seed), storage_high(nf, seed),
                                     cost_low(nf, seed), cost_high(nf, seed);
  for(int f = 0; f < nf; f++) {
    threshold.at(f) = degree_mean + degree_std_dev * factors.at(f);
  }
  #pragma omp parallel for schedule(dynamic,64)
  for(vidType v = 0; v < n_vertices; v++) {
    if(v % 10000 == 0) {
      printf("\r%s vertices processed", human_size(v, 1000).c_str());
      fflush(stdout);
    }
    int tid = omp_get_thread_num();
    uint64_t deg_l = vertices[v+1] - vertices[v];
    for(int f = 0; f < nf; f++) {
      if(deg_l > threshold.at(f)) {
        count_high.at(f).at(tid)++;
        storage_high.at(f).at(tid) += deg_l;
        cost_high.at(f).at(tid) += deg_l * deg_l;
      } else {
        count_low.at(f).at(tid)++;
        storage_low.at(f).at(tid) += deg_l;
        cost_low.at(f).at(tid) += deg_l * deg_l;
      }
    }
    for(uint64_t pos = vertices[v]; pos < vertices[v+1]; ++pos) {
      vidType u = edges[pos];
      uint64_t deg_r = vertices[u+1] - vertices[u];
      for(int f = 0; f < nf; f++) {
        bool high_l = deg_l > threshold.at(f);
        bool high_r = deg_r > threshold.at(f);
        if(!high_l && !high_r) count_ll.at(f).at(tid)++;
        else if(high_l && high_r) count_hh.at(f).at(tid)++;
        else count_lh.at(f).at(tid)++;
      }
    }
  }
  printf("\r                                \r");
  for(int f = 0; f < nf; f++) {
    double pct_ll = 100.0 * (double)sum(count_ll.at(f)) / n_edges;
    double pct_lh = 100.0 * (double)sum(count_lh.at(f)) / n_edges;
    double pct_hh = 100.0 * (double)sum(count_hh.at(f)) / n_edges;
    printf("\nThreshold of %.2f (degree > %llu):\n",
           factors.at(f), threshold.at(f));
    printf("Edges: %.2f%% low-low, %.2f%% low-high, %.2f%% high-high\n", pct_ll, pct_lh, pct_hh);
    printf("%llu vertices (%llu edges) are low (%.2f%%) (%sB) (%s accesses)\n",
           sum(count_low.at(f)), sum(storage_low.at(f)),
           100.0*(double)sum(storage_low.at(f))/n_edges,
           human_size(sum(storage_low.at(f))*sizeof(vidType)).c_str(),
           human_size(sum(cost_low.at(f)), 1000).c_str());
    printf("%llu vertices (%llu edges) are high (%.2f%%) (%sB) (%s accesses)\n",
           sum(count_high.at(f)), sum(storage_high.at(f)),
           100.0*(double)sum(storage_high.at(f))/n_edges,
           human_size(sum(storage_high.at(f))*sizeof(vidType)).c_str(),
           human_size(sum(cost_high.at(f)), 1000).c_str());
  }

  //munmap(vertices, fv_size);
  //munmap(edges, fe_size);
  delete[] vertices, edges;

  close(f_vertex);
  close(f_edge);
}
