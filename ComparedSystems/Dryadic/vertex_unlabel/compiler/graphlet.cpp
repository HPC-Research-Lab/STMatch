#include "def.h"
#include <algorithm>
#include <bitset>
#include <iostream>
#include <fstream>

Graphlet::Graphlet(int n) : vertices(n), adjacency(n) {}

void Graphlet::add_edge(int src, int dst) {
  assert(src >= 0 && src < vertices);
  assert(dst >= 0 && dst < vertices);
  adjacency.at(src).insert(dst);
  adjacency.at(dst).insert(src);
}

void Graphlet::add_edge(std::pair<int, int> e) {
  add_edge(e.first, e.second);
}

void Graphlet::add_all(const Graphlet &other) {
  assert(vertices >= other.vertices);
  for(int s = 0; s < other.vertices; s++) {
    for(int d : other.adjacency.at(s)) {
      add_edge(s, d);
    }
  }
}

bool Graphlet::connected() const {
  std::set<int> reachable;
  reachable.insert(0);
  for(int i = 0; i < vertices; i++) {
    std::set<int> to_add;
    for(int s : reachable) {
      to_add.insert(adjacency.at(s).begin(), adjacency.at(s).end());
    }
    reachable.insert(to_add.begin(), to_add.end());
  }
  return reachable.size() == vertices;
}

bool Graphlet::isomorphic(const Graphlet &other) const {
  if(vertices != other.vertices) return false;
  std::vector<int> relabel(vertices);
  for(int i = 0; i < vertices; i++) relabel.at(i) = i;
  do {
    bool match = true;
    for(int i = 0; match && i < vertices; i++) {
      int local = i;
      int remote = relabel.at(i);
      auto &set_l = adjacency.at(local);
      auto &set_r = other.adjacency.at(remote);
      if(set_l.size() != set_r.size()) {
        match = false;
        continue;
      }
      for(int v : set_l) {
        if(0 == other.adjacency.at(remote).count(relabel.at(v))) {
          match = false;
          break;
        }
      }
    }
    if(match) return true;
  } while(std::next_permutation(relabel.begin(), relabel.end())); 
  return false;
}

long factorial(int x) {
  if(x <= 1) return 1;
  else return x * factorial(x-1);
}

int Graphlet::multiplicity() const {
  // indistinguishable sets
  std::vector<std::set<int>> indis(vertices);
  for(int i = 0; i < vertices; i++) {
    indis.at(i).insert(i);
  }
  for(int i = 0; i < vertices; i++) {
    if(indis.at(i).size() == 0) continue;
    for(int j = i+1; j < vertices; j++) {
      if(indis.at(j).size() == 0) continue;
      int u = *indis.at(i).begin();
      int v = *indis.at(j).begin();
      Graphlet other = swapped(u, v);
      if(adjacency == other.adjacency) {
        indis.at(i).insert(indis.at(j).begin(), indis.at(j).end());
        indis.at(j).clear();
      }
    }
  }
  std::vector<std::set<int>> temp_indis;
  std::copy_if(indis.begin(), indis.end(),
               std::inserter(temp_indis, temp_indis.end()),
               [](const auto& s) { return s.size() > 0; });
  indis = temp_indis;
  // interchangeable sets of indistinguishable vertices
  std::vector<std::vector<std::set<int>>> inter(indis.size());
  for(int i = 0; i < indis.size(); i++) {
    inter.at(i).push_back(indis.at(i));
  }
  for(int i = 0; i < inter.size(); i++) {
    if(inter.at(i).size() == 0) continue;
    for(int j = i+1; j < inter.size(); j++) {
      if(inter.at(j).size() == 0) continue;
      const std::set<int> &su = *inter.at(i).begin();
      const std::set<int> &sv = *inter.at(j).begin();
      if(su.size() != sv.size()) continue;
      std::vector<int> vec_u(su.begin(), su.end());
      std::vector<int> vec_v(sv.begin(), sv.end());
      int nverts = su.size();
      Graphlet other = *this;
      for(int k = 0; k < nverts; k++) {
        other = other.swapped(vec_u.at(k), vec_v.at(k));
      }
      if(adjacency == other.adjacency) {
        inter.at(i).insert(inter.at(i).end(), inter.at(j).begin(), inter.at(j).end());
        inter.at(j).clear();
      }
    }
  }
  std::vector<std::vector<std::set<int>>> temp_inter;
  std::copy_if(inter.begin(), inter.end(),
               std::inserter(temp_inter, temp_inter.end()),
               [](const auto& v) { return v.size() > 0; });
  inter = temp_inter;
  int multip = 1;
  for(const auto &s : indis) { // multiplicity from indistinguishable
    multip *= factorial(s.size());
  }
  for(const auto &v : inter) { // multiplicity from interchangeable
    multip *= factorial(v.size());
  }
  return multip;
}

Graphlet Graphlet::swapped(int u, int v) const {
  std::vector<int> remap(vertices);
  for(int k = 0; k < vertices; k++) {
    remap.at(k) = k;
    if(k == u) remap.at(k) = v;
    if(k == v) remap.at(k) = u;
  }
  Graphlet out(vertices);
  for(int i = 0; i < vertices; i++) {
    for(int j : adjacency.at(i)) {
      out.add_edge(remap.at(i), remap.at(j));
    }
  }
  return out;
}

std::vector<Graphlet> Graphlet::all_connected(int n) {
  if(n <= 0) return {};
  else if(n == 1) return { Graphlet(1) };
  else {
    std::vector<Graphlet> graphlets;
    for(Graphlet gs : all_connected(n-1)) {
      Graphlet gb(n);
      gb.add_all(gs);
      for(uint32_t mask = 1; mask < (1 << (n-1)); mask++) {
        std::bitset<32> target(mask);
        Graphlet g(gb);
        for(int i = 0; i < n-1; i++) {
          if(target[i]) g.add_edge(i, n-1);
        }
        volatile bool is_unique = true;
        #pragma omp parallel for
        for(int i = 0; i < graphlets.size(); i++) {
          if(is_unique && g.isomorphic(graphlets.at(i))) {
            is_unique = false;
          }
        }
        if(is_unique) {
          graphlets.push_back(g);
          //std::cerr << "found " << graphlets.size() << " of size " << n << "\n";
        }
      }
    }
    return graphlets;
  }
}

Graphlet Graphlet::clique(int N) {
  Graphlet out(N);
  for(int i = 0; i < N; i++) {
    for(int j = i+1; j < N; j++) {
      out.add_edge(i, j);
    }
  }
  return out;
}

Graphlet Graphlet::from_file(std::string fname) {
  std::ifstream infile(fname.c_str());
  int N, src, dst;
  infile >> N;
  Graphlet out(N);
  while(infile >> src >> dst) {
    out.add_edge(src, dst);
  }
  return out;
}

std::string Graphlet::toString() const {
  std::ostringstream oss;
  oss << "Graphlet(" << vertices << " [";
  for(int s = 0; s < vertices; s++) {
    for(int d : adjacency.at(s)) {
      if(s < d) oss << "(" << s << "," << d << ")";
    }
  }
  oss << "])";
  return oss.str();
}

