#include "def.h"
#include <cmath>

double expected_size(const std::pair<std::set<int>, std::set<int>> &clause) {
  double p = (double)GLOBAL_AVERAGE_DEGREE / (double)GLOBAL_VERTEX_COUNT;
  return GLOBAL_VERTEX_COUNT * pow(p, clause.first.size()) * pow(1-p, clause.second.size());
}

MultiPlan::MultiPlan(const std::vector<ExecutionPlan> &in) : plans(in) {}

void MultiPlan::add(ExecutionPlan p) {
  plans.push_back(p);
}

int MultiPlan::divergence_point(const ExecutionPlan &one, const ExecutionPlan &two) {
  int max_v = std::min(one.vertices, two.vertices);
  for(int v = 0; v < max_v; v++) {
    if(one.depend.at(v).first != two.depend.at(v).first) return v;
    if(one.depend.at(v).second != two.depend.at(v).second) return v;
  }
  return max_v;
}

std::string MultiPlan::varname(std::pair<std::set<int>, std::set<int>> expr) {
  if(expr.first.size() + expr.second.size() == 0) return "sV";
  std::ostringstream oss;
  int largest = expr.first.size() + expr.second.size();
  for(int i = 0; i < largest; i++) {
    if(expr.first.find(i) != expr.first.end()) oss << "y" << i;
    else if(expr.second.find(i) != expr.second.end()) oss << "n" << i;
    else assert(false);
  }
  return oss.str();
}

std::vector<std::pair<std::set<int>, std::set<int>>> MultiPlan::all_priors(
      std::pair<std::set<int>, std::set<int>> expr) {
  //std::cerr << varname(expr) << " has priors: ";
  std::vector<std::pair<std::set<int>, std::set<int>>> priors;
  int largest = expr.first.size() + expr.second.size() - 1;
  for(int i = largest; i > 0; i--) {
    expr.first.erase(i);
    if(expr.first.size() == 0) break;
    expr.second.erase(i);
    priors.push_back(expr);
  }
  //for(auto e : priors) std::cerr << varname(e) << ", ";
  //std::cerr << "\n";
  return priors;
}

size_t max_sets_in_scope = 0;

std::pair<std::string, double> MultiPlan::compute_from_priors(
      const std::pair<std::set<int>, std::set<int>> &expr,
      const std::set<std::pair<std::set<int>, std::set<int>>> &priors) {
  max_sets_in_scope = std::max(max_sets_in_scope, priors.size());
  // find first available prior and compute with it
  std::pair<std::set<int>, std::set<int>> selected_prior,
        current_expr = expr, remaining_expr = expr;
  bool using_prior = false;
  int largest = expr.first.size() + expr.second.size() - 1;
  while(current_expr.first.size() > 0) {
    current_expr.first.erase(largest);
    current_expr.second.erase(largest);
    if(priors.find(current_expr) != priors.end()) {
      selected_prior = current_expr;
      using_prior = true;
      break;
    }
    largest--;
  }
  for(int x : selected_prior.first) remaining_expr.first.erase(x);
  for(int x : selected_prior.second) remaining_expr.second.erase(x);
  std::ostringstream oss;
  double complexity = 0.0;
  if(using_prior) oss << varname(selected_prior);
  complexity += expected_size(selected_prior);
  for(auto it = remaining_expr.first.begin(); it != remaining_expr.first.end(); ++it) {
    if(using_prior || it != remaining_expr.first.begin()) oss << " & ";
    oss << "g.N(v" << *it << ")";
    complexity += GLOBAL_AVERAGE_DEGREE;
  }
  for(auto it = remaining_expr.second.begin(); it != remaining_expr.second.end(); ++it) {
    oss << " - g.N(v" << *it << ")";
    complexity += GLOBAL_AVERAGE_DEGREE;
  }
  auto s = oss.str();
  if(s.length() == 0) {
    return { "vertices", GLOBAL_VERTEX_COUNT };
  } else {
    return { s, complexity };
  }
}

template<typename T>
T& MultiPlan::indent(T& stream, int level) {
  for(int i = 0; i < level+1; i++) stream << "  ";
  return stream;
}

template<typename stream_type, typename scope_type>
double MultiPlan::gen_codeblock(stream_type &stream, int level,
                     const std::vector<bool> &active,
                     scope_type &scope) const {
  if(level == 0) return 0;
  scope_type target_expressions;
  for(int plan_id = 0; plan_id < plans.size(); plan_id++) {
    if(!active.at(plan_id)) continue; // find priors for active plans
    assert(level < plans.at(plan_id).vertices);
    for(int lvl_id = 0; lvl_id < plans.at(plan_id).vertices; lvl_id++) {
      auto expr = plans.at(plan_id).depend.at(lvl_id);
      auto expr_priors = all_priors(expr);
      for(auto &e : expr_priors) {
        if(e.first.size() + e.second.size() > 1) target_expressions.insert(e);
      }
    }
    target_expressions.insert(plans.at(plan_id).depend.at(level));
  }
  double complexity = 0.0;
  for(auto &expr : target_expressions) {
    if(scope.count(expr)) continue;
    if(expr.first.size() + expr.second.size() > level) continue;
    auto p = compute_from_priors(expr, scope);
    complexity += p.second;
    indent(stream, level) << "VertexSet " << varname(expr) << " = "
                          << p.first << ";\n";
    if(expr.first.size() + expr.second.size() > 1) scope.insert(expr);
  }
  return complexity;
}

template<typename stream_type, typename dp_type, typename scope_type>
double MultiPlan::gen_level(stream_type &stream, int level,
               const std::vector<bool> &active,
               const dp_type &div_pts,
               const scope_type &scope) const {
  std::vector<bool> processed(plans.size(), false);
  scope_type current_scope(scope);
  double complexity = gen_codeblock(stream, level, active, current_scope);
  while(processed != active) {
    int first=-1;
    std::vector<bool> mask(plans.size(), false), term_mask(plans.size(), false);
    for(int i = 0; i < plans.size(); i++) {
      if(active.at(i) && !processed.at(i)) {
        if(plans.at(i).vertices <= level+1) {
          //indent(std::cerr, level) << "adding " << i << " to term_mask\n";
          term_mask.at(i) = true;
          continue;
        }
        if(first < 0) {
          first = i;
        }
        if(div_pts.at(first).at(i) > level) {
          mask.at(i) = true;
        }
      }
    }
    std::vector<bool> all_mask(mask);
    for(int i = 0; i < plans.size(); i++) if(term_mask.at(i)) all_mask.at(i) = true;
    //gen_codeblock(stream, level, all_mask, current_scope);
    for(int i = 0; i < plans.size(); i++) {
      if(term_mask.at(i)) {
        indent(stream, level) << "counter[" << i << "] += "
                              << varname(plans.at(i).depend.at(level))
                              << ".size();\n";
      }
    }
    if(first >= 0) {
      if(level == 0) {
        indent(stream, level) << "for(vidType v0 = 0; v0 < vertices; v0++) {\n";
      } else {
        indent(stream, level) << "for(vidType v" << level << " : "
                              << varname(plans.at(first).depend.at(level)) << ") {\n";
      }
      if(level == 1) indent(stream, level+1) << "if(v0 > v1) continue;\n";
      scope_type inner_scope(current_scope);
      double c = gen_level(stream, level+1, mask, div_pts, inner_scope);
      complexity += c * expected_size(plans.at(first).depend.at(level));
      indent(stream, level) << "}\n";
    }
    for(int i = 0; i < plans.size(); i++) {
      processed.at(i) = processed.at(i) || mask.at(i) || term_mask.at(i);
    }
  }
  return complexity;
}

std::pair<std::string, double> MultiPlan::gen_code() const {
  std::ostringstream oss;
  std::vector<bool> full_mask(plans.size(), true);
  std::vector<std::vector<int>> div_pts(plans.size(), std::vector<int>(plans.size()));
  for(int i = 0; i < plans.size(); i++) {
    for(int j = 0; j < plans.size(); j++) {
      div_pts.at(i).at(j) = divergence_point(plans.at(i), plans.at(j));
    }
  }
  std::set<std::pair<std::set<int>, std::set<int>>> empty_scope;
  double complexity = gen_level(oss, 0, full_mask, div_pts, empty_scope);
  printf("number of sets active was %ld\n", max_sets_in_scope);
  return { oss.str(), complexity };
}

