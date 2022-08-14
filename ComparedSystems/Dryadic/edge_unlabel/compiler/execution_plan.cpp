#include "def.h"

#include <iostream>
/*#include <iostream>
#include <fstream>
#include <iomanip>

#include <cmath>

#include <omp.h>*/

ExecutionPlan::ExecutionPlan() : vertices(0) {}

ExecutionPlan::ExecutionPlan(DirectedGraphlet &g) : vertices(g.vertices), restrictions(g.vertices,-1), depend(g.vertices)  {
  for(auto t : g.edgelist()) {
    int src, dst;
    bool exists;
    std::tie(src, dst, exists) = t;
    auto &d = depend.at(dst);
    if(exists) d.first.insert(src);
    else d.second.insert(src);
  }
}

ExecutionPlan::ExecutionPlan(DirectedGraphlet &g, std::vector<int> rest) : vertices(g.vertices), restrictions(rest),depend(g.vertices) {
  for(auto t : g.edgelist()) {
    int src, dst;
    bool exists;
    std::tie(src, dst, exists) = t;
    auto &d = depend.at(dst);
    if(exists) d.first.insert(src);
    else d.second.insert(src);
  }
  /*
     for(int i=0;i<vertices;++i){
     std::cout<<i<<":";
     for(int x : depend.at(i).first)std::cout<<"+"<<x;
     for(int x : depend.at(i).second)std::cout<<"-"<<x;
     std::cout<<std::endl;
     }
     */
}

std::string ExecutionPlan::toString() const {
  std::ostringstream oss;
  oss << "s0 = vertices\n";
  for(int i = 0; i < vertices-1; i++) {
    for(int j = 0; j < i; j++) oss << "  ";
    oss << "for(v" << i << " : s" << i << "){\n";
    //indentation
    for(int j = 0; j < i+1; j++) oss << "  ";
    const auto &d = depend.at(i+1);
    oss << "s" << i+1 << " = ";
    if(d.first.size() == 0) return ERR_STR;
    for(auto it = d.first.begin(); it != d.first.end(); ++it) {
      if(it != d.first.begin()) oss << " & ";
      oss << "N(v" << *it << ")";
    }
    for(auto it = d.second.begin(); it != d.second.end(); ++it) {
      oss << " - N(v" << *it << ")";
    }
    oss << "\n";
  }
  for(int j = 0; j < vertices-1; j++) oss << "  ";
  oss << "counter += cardinality(s" << vertices-1 << ")\n";
  for(int j=vertices-1;j>=0;--j){
    for(int i=j; i ; --i) oss << "  ";
    oss<<"}\n";
  }
  return oss.str();
}

std::string ExecutionPlan::toCode() const {
  std::ostringstream oss;
  oss<<"uint64_t count = 0;"<<std::endl;
  oss<<"const vidType vertices = g.V();\n{\n";
  oss<<"#pragma omp parallel for reduction(+:count) schedule(dynamic,64)\n";
  for(int i = 0; i < vertices-1; i++) {
    for(int j = 0; j < i; j++) oss << "  ";
    if(i>0)oss << "for(vidType v" << i << " : s" << i << "){\n";
    else oss << "for(vidType v0 = 0; v0 < vertices; ++v0) {\n";
    //indentation
    for(int j = 0; j < i+1; j++) oss << "  ";
    const auto &d = depend.at(i+1);
    if(i == vertices-2) oss << "count += "; 
    else oss << "VertexSet s" << i+1 << " = ";
    bool count = (i==vertices-2);
    for(auto it = d.second.begin();it!=d.second.end();++it){
      oss<<"difference_"<<(count?"num(":"set(");
      count=false;
    }
    if(d.first.size() == 0) return ERR_STR;
    for(auto it = d.first.begin(); it != d.first.end(); ++it) {
      if(it != d.first.begin()){
        oss << "intersection_"<<(count?"num(":"set(");
        count=false;
      }
      //oss << "N(v" << *it << ")"
    }
    //if(d.first.size() == 0) return ERR_STR;
    int rest = restrictions.at(i+1);
    bool delayedrest = false;
    for(auto it = d.first.begin(); it != d.first.end(); ++it) {
      if(it != d.first.begin()) oss << ", ";
      if(i==0 && *it == rest){
        //only one thing
        oss<<"bounded(g.N(v"<<*it<<"),"<< "v0)";
        if(i==vertices-2)oss<<".size()";
        continue;
      }
      oss << "g.N(v" << *it << ")";
      if(it != d.first.begin()){
        //i+1 is restricted by this one
        if(*it == rest){
          oss<<",v"<<rest;
        }
        //i+1 is restricted by the original
        if(delayedrest){
          oss<<",v"<<rest;
          delayedrest=false;
        }
        oss << ")";
      }
      else if(*it == rest){
        delayedrest=  true;
      }
    }
    for(auto it = d.second.begin(); it != d.second.end(); ++it) {
      oss << ", g.N(v" << *it << ")";
      //i+1 is restricted by this one
      if(*it == rest){
        oss<<",v"<<rest;
      }
      //i+1 is restricted by the original
      if(delayedrest){
        oss<<",v"<<rest;
        delayedrest=false;
      }
      oss << ")";
    }
    oss<<";\n";
  }
  for(int j=vertices-2;j>=0;--j){
    for(int i=j; i ; --i) oss << "  ";
    oss<<"}\n";
  }
  oss << "}std::cout<<\"Counted \"<< count <<\" instances.\";";
  return oss.str();
}
double ExecutionPlan::time_complexity() const {
  double c = 1.0;
  for(int i = 0; i < vertices; i++) {
    c *= expected_size(depend.at(i));
  }
  return c;
}

std::set<std::pair<std::set<int>, std::set<int>>> prefixes(
      const std::pair<std::set<int>, std::set<int>> &clause) {
  std::set<std::pair<std::set<int>, std::set<int>>> ps;
  int largest = clause.first.size() + clause.second.size() - 1;
  auto copy = clause;
  for(int i = largest; i >= 0; i--) {
    copy.first.erase(i);
    copy.second.erase(i);
    int t_size = copy.first.size();
    int f_size = copy.second.size();
    int both = t_size + f_size;
    if(both <= 1 || t_size <= 0) break;
    ps.insert(copy);
  }
  return ps;
}

double data_cost(const std::pair<std::set<int>, std::set<int>> &expr,
      const std::set<std::pair<std::set<int>, std::set<int>>> &priors) {
  // find first available prior and compute with it
  std::pair<std::set<int>, std::set<int>> selected_prior,
        current_expr = expr, remaining_expr = expr;
  //bool using_prior = false;
  int largest = expr.first.size() + expr.second.size() - 1;
  while(current_expr.first.size() > 0) {
    current_expr.first.erase(largest);
    current_expr.second.erase(largest);
    if(priors.find(current_expr) != priors.end()) {
      selected_prior = current_expr;
      //using_prior = true;
      break;
    }
    largest--;
  }
  for(int x : selected_prior.first) remaining_expr.first.erase(x);
  for(int x : selected_prior.second) remaining_expr.second.erase(x);
  return expected_size(selected_prior) +
        GLOBAL_AVERAGE_DEGREE * remaining_expr.first.size() +
        GLOBAL_AVERAGE_DEGREE * remaining_expr.second.size();
}

double ExecutionPlan::data_complexity() const {
  std::set<std::pair<std::set<int>, std::set<int>>> stored_sets;
  for(int i = 2; i < vertices; i++) {
    stored_sets.insert(depend.at(i));
    auto ps = prefixes(depend.at(i));
    stored_sets.insert(ps.begin(), ps.end());
  }
  double complexity = data_cost(depend.at(vertices-1), stored_sets);
  for(int i = vertices-2; i >= 0; i--) {
    complexity += data_cost(depend.at(i), stored_sets);
    if(i > 0) complexity *= expected_size(depend.at(i-1));
  }
  return complexity;
}

