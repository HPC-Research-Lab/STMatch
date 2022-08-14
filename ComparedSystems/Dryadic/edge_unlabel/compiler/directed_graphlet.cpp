#include "def.h"
#include <algorithm>

DirectedGraphlet::DirectedGraphlet() : vertices(0) {}

DirectedGraphlet::DirectedGraphlet(int n_vertices) : vertices(n_vertices), adjacency(n_vertices) {}

DirectedGraphlet::DirectedGraphlet(const DirectedGraphlet &other) :
      vertices(other.vertices), adjacency(other.vertices) {
  for(int src = 0; src < vertices; src++) {
    for(int dst = 0; dst < vertices; dst++) {
      if(other.is_edge_t(src, dst)) add_edge(src, dst, true);
      if(other.is_edge_f(src, dst)) add_edge(src, dst, false);
    }
  }
}

DirectedGraphlet::DirectedGraphlet(const Graphlet &other) :
      vertices(other.vertices), adjacency(other.vertices) {
  for(int src = 0; src < vertices; src++) {
    auto &d = other.adjacency.at(src);
    for(int dst : d) {
      if(src < dst) add_edge(src, dst, true);
    }
  }
  generate_complement();
}

DirectedGraphlet::DirectedGraphlet(const ExecutionPlan &other) :
      vertices(other.vertices), adjacency(other.vertices) {
  for(int src = 0; src < vertices; src++) {
    auto &d = other.depend.at(src);
    for(int dst : d.first) add_edge(src, dst, true);
    for(int dst : d.second) add_edge(src, dst, false);
  }
}

bool DirectedGraphlet::is_edge_t(int src, int dst) const {
  if(src >= vertices || dst >= vertices) return false;
  return adjacency.at(src).first.find(dst) != adjacency.at(src).first.end();
}

bool DirectedGraphlet::is_edge_f(int src, int dst) const {
  if(src >= vertices || dst >= vertices) return false;
  return adjacency.at(src).second.find(dst) != adjacency.at(src).second.end();
}

void DirectedGraphlet::add_edge(int src, int dst, bool exists=true) {
  assert(src != dst);
  assert(!is_edge_t(src, dst));
  assert(!is_edge_f(src, dst));
  assert(!is_edge_t(dst, src));
  assert(!is_edge_f(dst, src));
  if(exists) {
    adjacency.at(src).first.insert(dst);
  } else {
    adjacency.at(src).second.insert(dst);
  }
}

void DirectedGraphlet::generate_complement() {
  for(int src = 0; src < vertices; src++) {
    for(int dst = src+1; dst < vertices; dst++) {
      if(!is_edge_t(src, dst) && !is_edge_t(dst, src)) {
        if(!is_edge_f(src, dst) && !is_edge_f(dst, src)) {
          add_edge(src, dst, false);
        }
      }
    }
  }
}

std::vector<std::tuple<int, int, bool>> DirectedGraphlet::edgelist() const {
  std::vector<std::tuple<int, int, bool>> el;
  el.reserve(vertices * (vertices - 1) / 2);
  for(int src = 0; src < vertices; src++) {
    for(int dst = src + 1; dst < vertices; dst++) {
      if(is_edge_t(src, dst))
        el.push_back(std::make_tuple(dst, src, true));
      if(is_edge_t(dst, src))
        el.push_back(std::make_tuple(src, dst, true));
      if(is_edge_f(src, dst))
        el.push_back(std::make_tuple(dst, src, false));
      if(is_edge_f(dst, src))
        el.push_back(std::make_tuple(src, dst, false));
    }
  }
  assert(el.size() == vertices * (vertices - 1) / 2);
  return el;
}

void DirectedGraphlet::reverse_edge(int src, int dst) {
  generate_complement();
  if(is_edge_t(src, dst)) {
    adjacency.at(src).first.erase(adjacency.at(src).first.find(dst));
    adjacency.at(dst).first.insert(src);
    return;
  }
  if(is_edge_t(dst, src)) {
    adjacency.at(dst).first.erase(adjacency.at(dst).first.find(src));
    adjacency.at(src).first.insert(dst);
    return;
  }
  if(is_edge_f(src, dst)) {
    adjacency.at(src).second.erase(adjacency.at(src).second.find(dst));
    adjacency.at(dst).second.insert(src);
    return;
  }
  if(is_edge_f(dst, src)) {
    adjacency.at(dst).second.erase(adjacency.at(dst).second.find(src));
    adjacency.at(src).second.insert(dst);
    return;
  }
  assert(false);
}

bool DirectedGraphlet::isomorphic(const DirectedGraphlet &other) const {
  if(vertices != other.vertices) return false;
  std::vector<int> relabel(vertices);
  for(int i = 0; i < vertices; i++) relabel.at(i) = i;
  while(std::next_permutation(relabel.begin(), relabel.end())) {
    bool match = true;
    for(int i = 0; match && i < vertices; i++) {
      int local = i;
      int remote = relabel.at(i);
      auto &sets_l = adjacency.at(local);
      auto &sets_r = other.adjacency.at(remote);
      if(sets_l.first.size() != sets_r.first.size()) {
        match = false;
        continue;
      }
      if(sets_l.second.size() != sets_r.second.size()) {
        match = false;
        continue;
      }
      for(int v : sets_l.first) {
        if(!other.is_edge_t(remote, relabel.at(v))) {
          match = false;
          continue;
        }
      }
      for(int v : sets_l.second) {
        if(!other.is_edge_f(remote, relabel.at(v))) {
          match = false;
          continue;
        }
      }
    }
    if(match) return true;
  }
  return false;
}

bool DirectedGraphlet::cyclic() const {
  std::vector<std::set<int>> successors(vertices);
  for(int i = 0; i < vertices; i++) {
    successors.at(i).insert(adjacency.at(i).first.begin(), adjacency.at(i).first.end());
    successors.at(i).insert(adjacency.at(i).second.begin(), adjacency.at(i).second.end());
  }
  for(int round = 0; round < vertices; round++) {
    for(int i = 0; i < vertices; i++) {
      std::set<int> to_add;
      for(int v : successors.at(i)) {
        to_add.insert(adjacency.at(v).first.begin(), adjacency.at(v).first.end());
        to_add.insert(adjacency.at(v).second.begin(), adjacency.at(v).second.end());
      }
      if(to_add.find(i) != to_add.end()) return true;
      successors.at(i).insert(to_add.begin(), to_add.end());
    }
  }
  return false;
}

bool DirectedGraphlet::operator ==(const DirectedGraphlet &other) const {
  if(other.adjacency.size() != vertices) return false;
  for(int i = 0; i < vertices; i++) {
    if(adjacency.at(i) != other.adjacency.at(i)) return false;
  }
  return true;
}

bool DirectedGraphlet::undir_automorphism(std::vector<int> mapping) const {
  assert(mapping.size() >= vertices);
  std::vector<std::set<int>> original(vertices), mapped(vertices);
  for(int src = 0; src < vertices; src++) {
    for(int dst : adjacency.at(src).first) {
      original.at(src).insert(dst);
      original.at(dst).insert(src);
      mapped.at(mapping.at(src)).insert(mapping.at(dst));
      mapped.at(mapping.at(dst)).insert(mapping.at(src));
    }
  }
  for(int src = 0; src < vertices; src++) {
    if(original.at(src) != mapped.at(src)) {
      //std::cout << "mismatch at " << src << "\n";
      return false;
    }
  }
  return true;
}

bool DirectedGraphlet::root_symmetric() const {
  return multiplicity(true) >= 2;
}

int DirectedGraphlet::multiplicity(bool limit=false) const {
  assert(vertices >= 2);
  std::vector<int> mapping(vertices);
  int mul = 0;
  // for each edge bidirectionally
  int src, dst;
  bool exists;
  auto el = edgelist();
  if(limit) el = { std::make_tuple(0, 1, true) };
  for(auto edge : el) {
    std::tie(src, dst, exists) = edge;
    if(!exists) continue;
    std::vector<std::pair<int, int>> es = {
          std::make_pair(src, dst),
          std::make_pair(dst, src)
        };
    for(auto e : es) {
      mapping.at(0) = e.first;
      mapping.at(1) = e.second;
      int pos = 2;
      for(int i = 0; i < vertices; i++) {
        if(i != e.first && i != e.second) mapping.at(pos++) = i;
        assert(pos <= vertices);
      }
      assert(pos == vertices);
      bool has_one = false;
      do {
        if(undir_automorphism(mapping)) {
          mul++;
          has_one = true;
        }
      } while((!limit || !has_one) && std::next_permutation(mapping.begin()+2, mapping.end()));
    }
  }
  return mul;
}

DirectedGraphlet DirectedGraphlet::root_mirror() const {
  DirectedGraphlet temp(*this);
  temp.reverse_edge(0, 1);
  return temp.canonical();
}

DirectedGraphlet DirectedGraphlet::canonical() {
  std::vector<std::set<int>> successors(vertices);
  for(int i = 0; i < vertices; i++) {
    successors.at(i).insert(adjacency.at(i).first.begin(), adjacency.at(i).first.end());
    successors.at(i).insert(adjacency.at(i).second.begin(), adjacency.at(i).second.end());
  }
  for(int round = 0; round < vertices; round++) {
    for(int i = 0; i < vertices; i++) {
      std::set<int> to_add;
      for(int v : successors.at(i)) {
        to_add.insert(adjacency.at(v).first.begin(), adjacency.at(v).first.end());
        to_add.insert(adjacency.at(v).second.begin(), adjacency.at(v).second.end());
      }
      if(to_add.find(i) != to_add.end()) return DirectedGraphlet(0);
      successors.at(i).insert(to_add.begin(), to_add.end());
    }
  }
  DirectedGraphlet out(vertices);
  std::vector<int> map1(vertices, -1), map2(vertices, -1);
  for(int i = 0; i < vertices; i++) {
    int deg = successors.at(i).size();
    assert(deg < vertices);
    if(map1.at(deg) >= 0) return out;
    map1.at(deg) = i;
    map2.at(i) = deg;
  }
  for(int i = 0; i < vertices; i++) {
    int src = map2.at(i);
    for(int v : adjacency.at(i).first) out.add_edge(src, map2.at(v), true);
    for(int v : adjacency.at(i).second) out.add_edge(src, map2.at(v), false);
  }
  return out;
}

std::vector<DirectedGraphlet> DirectedGraphlet::acyc_can_vars() const {
  std::vector<DirectedGraphlet> vars;
  std::vector<int> order(vertices);
  for(int i = 0; i < vertices; i++) order.at(i) = i;
  do {
    DirectedGraphlet dg(vertices);
    for(int i = 0; i < vertices; i++) {
      for(int v : adjacency.at(i).first) {
        if(order.at(i) > order.at(v))
          dg.add_edge(i, v, true);
        else
          dg.add_edge(v, i, true);
      }
      for(int v : adjacency.at(i).second) {
        if(order.at(i) > order.at(v))
          dg.add_edge(i, v, false);
        else
          dg.add_edge(v, i, false);
      }
    }
    volatile bool is_unique = true;
    #pragma omp parallel for
    for(int i = 0; i < vars.size(); i++) {
      if(is_unique && dg.isomorphic(vars.at(i))) {
        is_unique = false;
      }
    }
    if(is_unique) {
      vars.push_back(dg.canonical());
    }
  } while(std::next_permutation(order.begin(), order.end()));
  return vars;
}

std::string DirectedGraphlet::toString() const {
  std::ostringstream oss;
  oss << "DirectedGraphlet(";
  auto el = edgelist();
  std::sort(el.begin(), el.end());
  for(auto t : el) {
    int src, dst;
    bool exists;
    std::tie(src, dst, exists) = t;
    oss << "(" << src << "->" << dst << (exists ? " true)" : " false)");
  }
  oss << " " << (cyclic() ? "Cyclic" : "Acyclic");
  bool rs = root_symmetric();
  oss << " " << (rs ? "RS" : "RA");
  int mul = multiplicity();
  //if(rs) mul /= 2;
  oss << " " << mul << ")\n";
  return oss.str();
}

bool DirectedGraphlet::good() const {
  for(auto &adj : adjacency) {
    if(adj.first.size() == 0 && adj.second.size() > 0) return false;
  }
  return true;
}

