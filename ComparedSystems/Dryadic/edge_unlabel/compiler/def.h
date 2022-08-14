#pragma once
#include <vector>
#include <set>
#include <tuple>
#include <algorithm>
#include <iterator>
#include <iostream>

#include <sstream>
#include <cassert>

#include <cmath>

#define ERR_STR "INVALID\n"
#define GLOBAL_AVERAGE_DEGREE 5.0
#define GLOBAL_VERTEX_COUNT 1000.

#ifdef GPU_GENCODE
  #define IFGPU(expression) expression
  #define NOGPU(expression)
#else
  #define IFGPU(expression)
  #define NOGPU(expression) expression
#endif


double expected_size(const std::pair<std::set<int>, std::set<int>> &clause);

class Graphlet;
class DirectedGraphlet;
class ExecutionPlan;
class MultiPlan;
class HyperPlan;
class MultiRed;
class RestPlan;
class MultiRestPlan;

class Graphlet {
public:
  int vertices;
  std::vector<std::set<int>> adjacency;
  Graphlet(int n);
  void add_edge(int, int);
  void add_edge(std::pair<int, int>);
  void add_all(const Graphlet&);
  bool connected() const;
  bool isomorphic(const Graphlet&) const;
  int multiplicity() const;
  Graphlet swapped(int, int) const;
  static std::vector<Graphlet> all_connected(int);
  static Graphlet clique(int);
  static Graphlet from_file(std::string);
  std::string toString() const;
  friend class DirectedGraphlet;
};

class DirectedGraphlet {
public:
  const int vertices;
  std::vector<std::pair<std::set<int>, std::set<int>>> adjacency;
  DirectedGraphlet();
  DirectedGraphlet(int);
  DirectedGraphlet(const DirectedGraphlet&);
  DirectedGraphlet(const Graphlet&);
  DirectedGraphlet(const ExecutionPlan&);
  bool is_edge_t(int, int) const;
  bool is_edge_f(int, int) const;
  void add_edge(int, int, bool);
  void generate_complement();
  std::vector<std::tuple<int, int, bool>> edgelist() const;
  void reverse_edge(int, int);
  bool isomorphic(const DirectedGraphlet&) const;
  bool cyclic() const;
  bool operator ==(const DirectedGraphlet&) const;
  bool undir_automorphism(std::vector<int>) const;
  bool root_symmetric() const;
  int multiplicity(bool) const;
  DirectedGraphlet root_mirror() const;
  DirectedGraphlet canonical();
  std::vector<DirectedGraphlet> acyc_can_vars() const;
  std::string toString() const;
  bool good() const;
};

class ExecutionPlan {
public:
  const int vertices;
  //restrictions[i] means i's ID should be less than j'd ID. That is, i'd degree should be
  //larger than j's degree.
  std::vector<int> restrictions;//-1 wherever it doesn't exist
  //depend indicates the structure of the graphlet
  //the first set determines the set intersections to perform and the second set the
  //difference operations.
  //For example, ({0,1,3}, {2}) means N(v0)&N(v1)&N(v3)-N(v2)
  std::vector<std::pair<std::set<int>, std::set<int>>> depend;
  ExecutionPlan();
  ExecutionPlan(DirectedGraphlet&);
  ExecutionPlan(DirectedGraphlet&,std::vector<int>);
  std::string toString() const;
  std::string toCode() const;
  double time_complexity() const;
  double data_complexity() const;
};

class MultiPlan {
public:
  std::vector<ExecutionPlan> plans;
  MultiPlan(const std::vector<ExecutionPlan>&);
  void add(ExecutionPlan);
  static int divergence_point(const ExecutionPlan&, const ExecutionPlan&);
  static std::string varname(std::pair<std::set<int>, std::set<int>>);
  static std::vector<std::pair<std::set<int>, std::set<int>>> all_priors(
        std::pair<std::set<int>, std::set<int>>);
  static std::pair<std::string, double> compute_from_priors(
        const std::pair<std::set<int>, std::set<int>>&,
        const std::set<std::pair<std::set<int>, std::set<int>>>&);
  template<typename T>
  static T& indent(T&, int);
  template<typename stream_type, typename scope_type>
  double gen_codeblock(stream_type&, int,
                       const std::vector<bool>&,
                       scope_type&) const;
  template<typename stream_type, typename dp_type, typename scope_type>
  double gen_level(stream_type&, int,
                 const std::vector<bool>&,
                 const dp_type&,
                 const scope_type&) const;
  std::pair<std::string, double> gen_code() const;
};

class HyperPlan {
  
  //std::map<std::tuple<int, int, int, int>, std::string> candidate_implementations;
  //std::vector<std::pair<std::tuple<int, int, int, int>, std::string>> all_implementations;
  std::string prefix;
  int global_plan_idx;
public:
  std::vector<std::vector<ExecutionPlan>> candidate_plans;
  void add_plan(int, const ExecutionPlan&);
  int sweep(std::string);
  //void generate_pareto(std::string) const;
  std::string greedy() const;
private:
  void output_plan(std::pair<std::tuple<int, int, int, int>, std::string>);
  int sweep_helper(const std::vector<int> selections, int begin);
};

class MultiRed {
public:
  Graphlet g;
  int n;
  std::vector<ExecutionPlan>* plans;
  std::vector<std::vector<int>> automorphisms;
  MultiRed(Graphlet&,std::vector<ExecutionPlan> &);
  //void list_automorphisms(std::vector<std::vector<int>>);
  void autos(std::vector<int>&, std::set<int>&);
  void reduced(std::vector<int>&,std::vector<int>&);
  void process_all(std::vector<int>&,std::vector<std::vector<int>>&,std::vector<int>&,std::set<int>&,std::set<int>&);
  //these are functions defined in multip_red_dg
  //void multiplicity_reduced_graph(DirectedGraphlet&,std::vector<ExecutionPlan>&);
};

class RestSet{
public:
  int depth;
  //increasing order
  std::vector<int> ins;
  //increasing order
  std::vector<int> out;
  std::vector<int> restrict;
  std::vector<int> res_chain;
  bool tranResChain = false;
  RestSet(const std::vector<int>&, const std::vector<int>&, const std::vector<int>&);
  bool operator<(const RestSet&) const;
  std::string varname;
  bool valid();
  //get the restriction at the current level
  int restriction() const;
  //computes a parent, if it exists, otherwise returns self;
  RestSet parent() const;
  void append_calc_to_stream(std::ostream&,int index,std::set<RestSet>&)const;
  
  double data_complexity_ignoring_restrictions() const;
  double time_complexity_ignoring_restrictions() const;
  double time_complexity_ignoring_restrictions(bool,std::set<RestSet>&) const;
  friend std::ostream & operator<<(std::ostream & os, RestSet& rs) ;
private:
  std::string var_name();
};

class RestPlan {
public:
  int id, vertices;
  std::vector<int> rest;
  RestPlan(ExecutionPlan&,int);
  //ExecutionPlan* parent;
  //first element is the set for v1
  std::vector<RestSet> loopons;
  //first element is the list of sets needed from v0
  std::vector<std::set<RestSet>> depends;
  double data_complexity();
  double time_complexity();
  friend std::ostream & operator<<(std::ostream & os, RestPlan & rp);

};

#include<map>
class MultiRestPlan {
public:
  int depth=0;
  //npls only matters for the root
  //# of plans
  int npls=0;
  //make a multirestplan of this depth
  MultiRestPlan(int);
  ~MultiRestPlan();
  //sets to track at this level
  std::set<RestSet> atlev;
  //things to do below
  std::map<RestSet,MultiRestPlan*> children;
  std::map<RestSet,int> counters;
  //add an execution plan with a given id;
  void add_rest_plan(RestPlan&);
  void add_ex_plan(ExecutionPlan&,int);
  std::pair<std::string, std::string> to_code();
  double data_complexity();
  double time_complexity();
private:
  std::vector<RestPlan> plans;
  void append_to_stream(std::ostream&, std::ostream&);
  double rec_data_complexity(double,std::vector<int> &,std::vector<int>&);
  double rec_time_complexity(double,std::vector<int> &,std::vector<int>&);
};

template<class T>
void print_vector(std::vector<T> &v);

template<class T>
void print_vector(const std::vector<T> &v);

template<class T>
void print_set(std::set<T> &v);

std::ostream & operator<<(std::ostream & os, const RestSet& rs) ;
std::ostream & operator<<(std::ostream & os, RestSet& rs) ;

std::ostream & operator<<(std::ostream & os, RestPlan & rp);
