#include "def.h"
#include <iostream>
#include <fstream>
#include <iomanip>

#include <cmath>

int substring_count(std::string large, std::string small) {
  int count = 0;
  std::string::size_type pos = 0;
  while((pos = large.find(small, pos)) != std::string::npos) {
    count++;
    pos += small.length();
  }
  return count;
}

void HyperPlan::add_plan(int id, const ExecutionPlan &p) {
  while(candidate_plans.size() <= id) {
    candidate_plans.push_back(std::vector<ExecutionPlan>(0));
  }
  candidate_plans.at(id).push_back(p);
}
int HyperPlan::sweep(std::string sweep_prefix="") {
  prefix = sweep_prefix;
  global_plan_idx = 0;
  //candidate_implementations.clear();
  return sweep_helper(std::vector<int>(candidate_plans.size(), 0), candidate_plans.size()-1);
}
/*void generate_pareto(std::string sweep_prefix="") const {
  std::cout << "\nPareto-optimal configs:\n";
  std::set<std::tuple<int, int, int, int>> opt_keys;
  for(const auto p : candidate_implementations) {
    auto my_k = p.first;
    std::set<std::tuple<int, int, int, int>> to_remove;
    bool is_opt = true;
    for(const auto co_k : opt_keys) {
      int my_0, my_1, my_2, my_3;
      std::tie(my_0, my_1, my_2, my_3) = my_k;
      int co_0, co_1, co_2, co_3;
      std::tie(co_0, co_1, co_2, co_3) = co_k;
      if(my_0 >= co_0 && my_1 >= co_1 &&
         my_2 >= co_2 && my_3 >= co_3) {
        is_opt = false;
      }
      if(my_0 <= co_0 && my_1 <= co_1 &&
         my_2 <= co_2 && my_3 <= co_3) {
        to_remove.insert(co_k);
      }
    }
    for(const auto co_k : to_remove) opt_keys.erase(co_k);
    if(is_opt) opt_keys.insert(my_k);
  }
  int plan_idx = 0;
  //for(const auto my_k : opt_keys) { // TODO here
  for(const auto my_p : all_implementations) {
    auto my_k = my_p.first; //
    auto my_v = my_p.second; //
    int n_for, n_set, n_int, n_sub;
    std::tie(n_for, n_set, n_int, n_sub) = my_k;
    std::ostringstream oss;
    oss << "  // Plan using:\t" << n_for << " for-loops\t" << n_set << " VertexSet's\t"
        << n_int << " intersections\t" << n_sub << " differences\n";
    //oss << candidate_implementations.at(my_k) << "\n";
    oss << my_v << "\n";
    if(sweep_prefix.length() > 0) {
      std::ostringstream fname;
      fname << sweep_prefix << plan_idx << ".hpp";
      std::ofstream outfile(fname.str().c_str());
      assert(outfile);
      outfile << oss.str();
      outfile.close();
    } else {
      std::cout << oss.str();
    }
    plan_idx++;
  }
}*/
void HyperPlan::output_plan(std::pair<std::tuple<int, int, int, int>, std::string> plan) {
  int local_idx;
  #pragma omp critical
  {
    local_idx = global_plan_idx++;
  }
  std::ostringstream fname;
  fname << prefix << local_idx << ".hpp";
  std::ofstream outfile(fname.str().c_str());
  assert(outfile);
  int n_for, n_set, n_int, n_sub;
  std::tie(n_for, n_set, n_int, n_sub) = plan.first;
  outfile << "  // Plan using:\t" << n_for << " for-loops\t" << n_set << " VertexSet's\t"
          << n_int << " intersections\t" << n_sub << " differences\n";
  outfile << plan.second;
  outfile.close();
}
int HyperPlan::sweep_helper(const std::vector<int> selections, int begin) {
  assert(begin >= 0);
  assert(candidate_plans.at(begin).size() > 0);
  int count = 0;
  #pragma omp parallel for reduction(+:count)
  for(int i = 0; i < candidate_plans.at(begin).size(); i++) {
    auto local_selections = selections;
    local_selections.at(begin) = i;
    if(0 == begin) {
      std::vector<ExecutionPlan> plans;
      std::vector<std::tuple<int, int, int>> relations;
      for(int j = 0; j < candidate_plans.size(); j++) {
        auto &p = candidate_plans.at(j).at(local_selections.at(j));
        plans.push_back(p);
        DirectedGraphlet dg(p);
        if(dg.root_symmetric()) {
          relations.push_back(std::make_tuple(plans.size()-1, -1, dg.multiplicity(false)/2));
        } else {
          DirectedGraphlet dgr = dg.root_mirror().canonical();
          ExecutionPlan pr(dgr);
          plans.push_back(pr);
          relations.push_back(std::make_tuple(plans.size()-2,
                              plans.size()-1, dg.multiplicity(false)));
        }
      }
      count++;
      MultiPlan mp(plans);
      std::ostringstream oss;
      auto p = mp.gen_code();
      oss << "// complexity is " << std::setprecision(40) << std::round(p.second) << "\n";
      for(int j = 0; j < plans.size(); j++) {
        oss << "  uint64_t counter_" << j << " = 0;\n";
      }
      oss << "  const vidType vertices = g.V();\n";
      oss << "#pragma omp parallel";
      for(int j = 0; j < plans.size(); j++) {
        oss << " reduction(+:counter_" << j << ")";
      }
      oss << "\n{\n";
      oss << "  #pragma omp for schedule(dynamic,64)\n";
      oss << p.first;
      //oss << "std::cout << \"thread \" << omp_get_thread_num() << \" is done\\n\";\n";
      oss << "}\n";
      for(int j = 0; j < relations.size(); j++) {
        int one, two, mul;
        std::tie(one, two, mul) = relations.at(j);
        if(two >= 0) {
          oss << "  uint64_t result_" << j << " = (counter_" << one
              << " + counter_" << two << ") / " << mul << ";\n";
        } else {
          oss << "  uint64_t result_" << j << " = counter_"
              << one << " / " << mul << ";\n";
        }
        //oss << "  std::cout << result_" << j << " << \"\\n\";\n";
      }
      oss << "\n";
      auto serialized = oss.str();
      int n_for = substring_count(serialized, "for(");
      int n_set = substring_count(serialized, "VertexSet");
      int n_int = substring_count(serialized, " & ");
      int n_sub = substring_count(serialized, " - ");
      //std::cout << "Plan using:\t" << n_for << " for-loops\t" << n_set << " VertexSet's\t"
      //          << n_int << " intersections\t" << n_sub << " differences\n";
      std::tuple<int, int, int, int> key = std::make_tuple(n_for, n_set, n_int, n_sub);
      //candidate_implementations[key] = serialized;
      //candidate_implementations.insert({key, serialized});
      //all_implementations.push_back({key, serialized});
      output_plan({key, serialized});
      //std::cout << serialized;
    } else {
      count += sweep_helper(local_selections, begin-1);
    }
  }
  return count;
}

std::string HyperPlan::greedy() const {
  std::vector<ExecutionPlan> plans;
  std::vector<std::tuple<int, int, int>> relations;
  for(int j = 0; j < candidate_plans.size(); j++) {
    //auto &p = candidate_plans.at(j).at(local_selections.at(j));
    auto &local =  candidate_plans.at(j);
    int best_idx = 0;
    double best_data = local.at(0).data_complexity();
    for(int k = 1; k < local.size(); k++) {
      double v = local.at(k).data_complexity();
      if(v < best_data) {
        best_idx = k;
      }
    }
    plans.push_back(local.at(best_idx));
    DirectedGraphlet dg(local.at(best_idx));
    if(dg.root_symmetric()) {
      relations.push_back(std::make_tuple(plans.size()-1, -1, dg.multiplicity(false)/2));
    } else {
      DirectedGraphlet dgr = dg.root_mirror().canonical();
      ExecutionPlan pr(dgr);
      plans.push_back(pr);
      relations.push_back(std::make_tuple(plans.size()-2,
                          plans.size()-1, dg.multiplicity(false)));
    }
  }
  MultiPlan mp(plans);
  std::ostringstream oss;
  auto p = mp.gen_code();
  oss << "// complexity is " << std::setprecision(40) << std::round(p.second) << "\n";
  oss << "  uint64_t counter[" << plans.size() << "];\n";
  oss << "  for(int i = 0; i < " << plans.size() << "; i++) counter[i] = 0;\n";
  /*for(int j = 0; j < plans.size(); j++) {
    oss << "  uint64_t counter_" << j << " = 0;\n";
  }*/
  oss << "  const vidType vertices = g.V();\n";
  oss << "#pragma omp parallel";
  /*for(int j = 0; j < plans.size(); j++) {
    oss << " reduction(+:counter_" << j << ")";
  }*/
  oss << " reduction(+:counter[0:" << plans.size() << "])";
  oss << "\n{\n";
  oss << "  #pragma omp for schedule(dynamic,64)\n";
  oss << p.first;
  //oss << "std::cout << \"thread \" << omp_get_thread_num() << \" is done\\n\";\n";
  oss << "}\n";
  for(int j = 0; j < relations.size(); j++) {
    int one, two, mul;
    std::tie(one, two, mul) = relations.at(j);
    if(two >= 0) {
      oss << "  uint64_t result_" << j << " = (counter[" << one
          << "] + counter[" << two << "]) / " << mul << ";\n";
    } else {
      oss << "  uint64_t result_" << j << " = counter["
          << one << "] / " << mul << ";\n";
    }
    oss << "  std::cout << result_" << j << " << \"\\n\";\n";
  }
  oss << "\n";
  auto serialized = oss.str();
  /*int n_for = substring_count(serialized, "for(");
  int n_set = substring_count(serialized, "VertexSet");
  int n_int = substring_count(serialized, " & ");
  int n_sub = substring_count(serialized, " - ");
  std::cout << "// Plan using:\t" << n_for << " for-loops\t" << n_set << " VertexSet's\t"
            << n_int << " intersections\t" << n_sub << " differences\n";
  std::cout << serialized << "\n";*/
  return serialized;
}

