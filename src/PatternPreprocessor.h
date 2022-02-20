
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <cassert>
#include "Trie.h"
#include "Constants.h"


namespace libra {

  struct Pattern {

    int nnodes = 0, nedges = 0;

    int vertex_label_[MAX_PAT_SIZE];
    int partial_[MAX_PAT_SIZE];

    // dependency_[i][0] is the size of dependency_[i]
    // dependency_[i][2j+1] is the index of set in level i-1 that need to be computed with the current neighbor set
    // dependency_[i][2j+2] is the op (intersection or diff)
    int dependency_[MAX_PAT_SIZE][2 * MAX_PAT_SIZE + 1];

    // the index of the candidate set in stk
    int candidate_idx_[MAX_PAT_SIZE];
    int first_positive_[MAX_PAT_SIZE];

    int dependency_partial_[MAX_PAT_SIZE][MAX_PAT_SIZE][MAX_PAT_SIZE];

    Pattern* to_gpu() {
      Pattern* pat;
      cudaMalloc(&pat, sizeof(Pattern));
      cudaMemcpy(pat, this, sizeof(Pattern), cudaMemcpyHostToDevice);
      return pat;
    }
  };

  struct PatternPreprocessor {

    Pattern pat_;

    int adj_matrix_[MAX_PAT_SIZE][MAX_PAT_SIZE];
    int vertex_order_[MAX_PAT_SIZE];
    int order_map_[MAX_PAT_SIZE];
    std::vector<std::vector<int>> L_adj_matrix_;

    PatternPreprocessor(std::string filename) {
      readfile(filename);
      get_matching_order();
      get_partial_order();
      code_motion();
      std::cout << "Pattern read complete. Pattern size: " << pat_.nnodes << std::endl;
    }

    void readfile(std::string& filename) {
      std::cout << filename << std::endl;

      std::ifstream fin(filename);
      std::string line;
      while (std::getline(fin, line) && (line[0] == '#'));
      pat_.nnodes = 0;
      std::vector<int> vertex_labels;
      do {
        std::istringstream sin(line);
        char tmp;
        int v;
        int label;
        sin >> tmp >> v >> label;
        vertex_labels.push_back(label);
        pat_.nnodes++;
      } while (std::getline(fin, line) && (line[0] == 'v'));

      memset(adj_matrix_, 0, sizeof(adj_matrix_));
      do {
        std::istringstream sin(line);
        char tmp;
        int v1, v2, label;
        sin >> tmp >> v1 >> v2 >> label;
        adj_matrix_[v1][v2] = label;
        adj_matrix_[v2][v1] = label;
      } while (getline(fin, line));
    }


    void get_matching_order() {
      int root = 0;
      int max_degree = 0;
      for (int i = 0; i < pat_.nnodes; i++) {
        int d = 0;
        for (int j = 0; j < pat_.nnodes; j++) {
          if (adj_matrix_[i][j] > 0) d++;
        }
        if (d > max_degree) {
          root = i;
          max_degree = d;
        }
      }

      std::vector<int> q;
      q.push_back(root);
      int i = 0;
      std::vector<int> visited(pat_.nnodes, 0);
      while (!q.empty()) {
        int a = q.back();
        q.pop_back();
        if (!visited[a]) {
          vertex_order_[i++] = a;
        }
        visited[a] = 1;
        for (int b = 0; b < pat_.nnodes; b++) {
          if (adj_matrix_[a][b] > 0 && !visited[b])
            q.push_back(b);
        }
      }
    }

    void _permutation(
      std::vector<std::vector<int>>& all,
      std::vector<int>& a, int l, int r) {
      // Base case
      if (l == r)
        all.push_back(a);
      else {
        // Permutations made
        for (int i = l; i <= r; i++) {
          // Swapping done
          std::swap(a[l], a[i]);
          // Recursion called
          _permutation(all, a, l + 1, r);
          // backtrack
          std::swap(a[l], a[i]);
        }
      }
    }

    void get_partial_order() {

      std::vector<int> p1;
      for (int i = 0; i < pat_.nnodes; i++) {
        p1.push_back(i);
      }
      std::vector<std::vector<int>> permute, valid_permute;
      _permutation(permute, p1, 0, pat_.nnodes - 1);

      for (int i = 0; i < pat_.nnodes; i++)
        order_map_[vertex_order_[i]] = i;

      for (auto pp : permute) {
        std::vector<std::vector<int>> adj_tmp(pat_.nnodes);
        for (int i = 0; i < pat_.nnodes; i++) {
          std::set<int> tp;
          for (int j = 0; j < pat_.nnodes; j++) {
            if (adj_matrix_[i][j] > 0) {
              tp.insert(pp[j]);
            }
            adj_tmp[pp[i]] = std::vector<int>(tp.begin(), tp.end());
          }
          bool valid = true;
          for (int i = 0; i < pat_.nnodes; i++) {
            int d = 0;
            for (int j = 0; j < pat_.nnodes; j++) {
              if (adj_matrix_[i][j] > 0) d++;
            }
            if (d == adj_tmp[i].size()) {
              for (int j : adj_tmp[i]) {
                if (adj_matrix_[i][j] <= 0) {
                  valid = false;
                  break;
                }
              }
            }
            else {
              valid = false;
            }
            if (!valid) break;
          }
          if (valid) {
            valid_permute.push_back(pp);
          }
        }
      }

      L_adj_matrix_.resize(pat_.nnodes, std::vector<int>(pat_.nnodes, 0));
      std::set<std::pair<int, int>> L;
      for (int i = 0; i < pat_.nnodes; i++) {
        int v = vertex_order_[i];
        std::vector<std::vector<int>> stabilized_aut;
        for (auto& x : valid_permute) {
          if (x[v] == v) {
            stabilized_aut.push_back(x);
          }
          else {
            L_adj_matrix_[order_map_[v]][order_map_[x[v]]] = 1;
          }
        }
        valid_permute = stabilized_aut;
      }

      memset(pat_.partial_, -1, sizeof(pat_.partial_));
      int pivot = -1;
      for (int level = 0; level < pat_.nnodes; level++) {
        pivot = -1;
        for (int j = level - 1; j >= 0; j--) {
          if (L_adj_matrix_[j][level] == 1) {
            pivot = j;
            break;
          }
        }
        pat_.partial_[level] = pivot;
      }
    }

    void code_motion() {
      // JIANG: this stores the last vertex from level 0 to l-1 that is connected to the vertex at level l
      std::vector<bool> v0_positive(pat_.nnodes);

      // JIANG: this stores the original set ops (without code motion) for each level, positive means intersection, negative means difference 
      std::vector<std::vector<int>> set_ops(pat_.nnodes);

      for (int level = 1; level < pat_.nnodes; level++) {
        for (int k = 0; k <= level - 1; k++) {
          if (adj_matrix_[vertex_order_[k]][vertex_order_[level]] > 0) {
            if (k == 0) v0_positive[level] = true;
            set_ops[level].push_back(k);
          }
          else {
            if (k == 0) v0_positive[level] = false;
            set_ops[level].push_back(-k);
          }
        }
      }

      std::vector<std::vector<std::vector<int> > > partial_level_dependency(pat_.nnodes);
      std::vector<std::vector<std::pair<int, int> > > trie_level_dependency(pat_.nnodes);
      std::vector<int> trie_candidate_index(pat_.nnodes);

      Trie trie(pat_.nnodes);
      trie.L_adj_matrix_ = &L_adj_matrix_;

      for (int i = 0; i < set_ops.size(); i++) {
        trie.insert(set_ops[i], v0_positive[i]);
      }
      trie.trie2vec(partial_level_dependency, trie_level_dependency, trie_candidate_index);
      memcpy(pat_.candidate_idx_, trie_candidate_index.data(), sizeof(int) * trie_candidate_index.size());

      memset(pat_.dependency_, -1, sizeof(pat_.dependency_));

      for (int i = 0; i < trie_level_dependency.size(); i++) {
        pat_.dependency_[i][0] = trie_level_dependency[i].size();
        for (int j = 0; j < trie_level_dependency[i].size(); j++) {
          std::pair<int, int>& k = trie_level_dependency[i][j];
          pat_.dependency_[i][2 * j + 1] = k.first;
          pat_.dependency_[i][2 * j + 2] = k.second;
        }
      }

      std::vector<int> trie_left_mark(pat_.nnodes, 2);
      trie_left_mark[0] = 0;
      for (int i = 1; i < trie_level_dependency.size(); i++) {
        if (i == 1) {
          if (trie_level_dependency[1].size() == 2) {
            trie_left_mark[i] = 0;
            continue;
          }
          else {
            break;
          }
        }
        if (trie_level_dependency[i][0].second < 0) {
          trie_left_mark[i] = 0;
        }
        else {
          trie_left_mark[i] = 1;
          break;
        }
      }

      memcpy(pat_.first_positive_, trie_left_mark.data(), trie_left_mark.size() * sizeof(int));

      memset(pat_.dependency_partial_, -1, sizeof(pat_.dependency_partial_));

      for (int i = 0; i < partial_level_dependency.size(); i++) {
        for (int j = 0; j < partial_level_dependency[i].size(); j++) {
          auto& slot = partial_level_dependency[i][j];
          memcpy(&pat_.dependency_partial_[i][j][0], slot.data(), sizeof(int) * slot.size());
        }
      }
    }
  };
}