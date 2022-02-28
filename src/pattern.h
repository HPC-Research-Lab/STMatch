
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <cassert>
#include "config.h"


namespace libra {

  typedef struct {

    pattern_node_t nnodes = 0;
    label_t vertex_label[PAT_SIZE];
    bitarray32 partial[PAT_SIZE][PAT_SIZE];
    set_op_t set_ops[PAT_SIZE][PAT_SIZE];
  } Pattern;


  struct PatternPreprocessor {

    Pattern pat;

    int adj_matrix_[PAT_SIZE][PAT_SIZE];
    int vertex_order_[PAT_SIZE];
    int order_map_[PAT_SIZE];
    std::vector<std::vector<int>> L_adj_matrix_;
    std::vector<std::vector<int>> board;

    int length[PAT_SIZE];
    int edge[PAT_SIZE][PAT_SIZE];

    PatternPreprocessor(std::string filename) {
      readfile(filename);
      get_matching_order();
      get_partial_order();
      get_set_ops();
      propagate_partial_order();

      // TODO: analyze pattern and fill in pat
      std::cout << "Pattern read complete. Pattern size: " << (int)pat.nnodes << std::endl;
    }

    Pattern* to_gpu() {
      Pattern* patcopy;
      cudaMalloc(&patcopy, sizeof(Pattern));
      cudaMemcpy(patcopy, &pat, sizeof(Pattern), cudaMemcpyHostToDevice);
      return patcopy;
    }

    void readfile(std::string& filename) {
      std::cout << filename << std::endl;

      std::ifstream fin(filename);
      std::string line;
      while (std::getline(fin, line) && (line[0] == '#'));
      pat.nnodes = 0;
      std::vector<label_t> vertex_labels;
      do {
        std::istringstream sin(line);
        char tmp;
        int v;
        int label;
        sin >> tmp >> v >> label;
        vertex_labels.push_back(label);
        pat.nnodes++;
      } while (std::getline(fin, line) && (line[0] == 'v'));

      memset(adj_matrix_, 0, sizeof(adj_matrix_));
      do {
        std::istringstream sin(line);
        char tmp;
        int v1, v2;
        int label;
        sin >> tmp >> v1 >> v2 >> label;
        adj_matrix_[v1][v2] = label;
        adj_matrix_[v2][v1] = label;
      } while (getline(fin, line));
    }


    // TODO: change this to dryadic/cuTS
    void get_matching_order() {
      int root = 0;
      int max_degree = 0;
      for (int i = 0; i < pat.nnodes; i++) {
        int d = 0;
        for (int j = 0; j < pat.nnodes; j++) {
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
      std::vector<int> visited(pat.nnodes, 0);
      while (!q.empty()) {
        int a = q.back();
        q.pop_back();
        if (!visited[a]) {
          vertex_order_[i++] = a;
        }
        visited[a] = 1;
        for (int b = 0; b < pat.nnodes; b++) {
          if (adj_matrix_[a][b] > 0 && !visited[b])
            q.push_back(b);
        }
      }

      for (int i = 0; i < pat.nnodes; i++)
        order_map_[vertex_order_[i]] = i;
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

    void get_set_ops() {

      board.resize(pat.nnodes, std::vector<int>(pat.nnodes, 0));
      board[0][0] = 1;

      for (int i = 1; i < pat.nnodes - 1; i++) {
        int ops = 0;
        for (int j = 0; j <= i; j++) {
          if (adj_matrix_[order_map_[i + 1]][order_map_[j]]) ops |= (1 << (i - j));
        }
        board[i][0] = ops;
      }

      memset(length, 0, sizeof(length));
      for (int i = 0; i < pat.nnodes; i++) length[i] = 1;

      memset(pat.set_ops, 0, sizeof(pat.set_ops));
      for (int j = 0; j < pat.nnodes - 1; j++) {
        for (int i = pat.nnodes - 2 - j; i >= 0; i--) {
          // 0 means empty slot in board
          if (board[i][j] == 0) continue;

          int op1 = board[i][j] & 1;
          int op2 = (board[i][j] >> 1);

          if (op2 > 0) {
            bool exist = false;
            // k starts from 1 to make sure candidate sets are not used for computing non-candidates sets
            int start_k = ((j == 0 || pat.partial[i][0] == -1) ? 0 : 1);
            for (int k = start_k; k < length[i - 1]; k++) {
              if (op2 == board[i - 1][k]) {
                exist = true;
                pat.set_ops[i][j] += k;
                pat.set_ops[i][j] += (op1 << 5);
                break;
              }
            }
            if (!exist) {
              pat.set_ops[i][j] += length[i - 1];
              pat.set_ops[i][j] += (op1 << 5);
              board[i - 1][length[i - 1]++] = op2;
            }
          }
          else {
            pat.set_ops[i][j] |= 0x10;
          }
        }
      }
      // mark the end of slot
      for (int i = 0; i < pat.nnodes - 1; i++) {
        pat.set_ops[i][length[i]] |= 0x80;
      }
    }

    void get_partial_order() {

      std::vector<int> p1;
      for (int i = 0; i < pat.nnodes; i++) {
        p1.push_back(i);
      }
      std::vector<std::vector<int>> permute, valid_permute;
      _permutation(permute, p1, 0, pat.nnodes - 1);

      for (auto pp : permute) {
        std::vector<std::vector<int>> adj_tmp(pat.nnodes);
        for (int i = 0; i < pat.nnodes; i++) {
          std::set<int> tp;
          for (int j = 0; j < pat.nnodes; j++) {
            if (adj_matrix_[i][j] > 0) {
              tp.insert(pp[j]);
            }
            adj_tmp[pp[i]] = std::vector<int>(tp.begin(), tp.end());
          }
          bool valid = true;
          for (int i = 0; i < pat.nnodes; i++) {
            int d = 0;
            for (int j = 0; j < pat.nnodes; j++) {
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

      L_adj_matrix_.resize(pat.nnodes, std::vector<int>(pat.nnodes, 0));
      std::set<std::pair<int, int>> L;
      for (int i = 0; i < pat.nnodes; i++) {
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

      memset(pat.partial, 0, sizeof(pat.partial));
      int pivot = -1;
      for (int level = 1; level < pat.nnodes; level++) {
        pivot = -1;
        for (int j = level - 1; j >= 0; j--) {
          if (L_adj_matrix_[j][level] == 1) {
            pivot = j;
            break;
          }
        }
        pat.partial[level - 1][0] = (1 << pivot);
      }
    }

    int bitidx(bitarray32 a) {
      for (int i=0; i<32; i++) {
        if (a & (1 << i)) return i;
      }
      return -1;
    }

    void propagate_partial_order() {
      // propagate partial order of candiate sets to all slots
      memset(edge, 0, sizeof(edge));
      for (int i = 0; i < pat.nnodes - 1; i++) {
        int t = bitidx(pat.partial[i][0]);
        if (t != -1) {
          edge[i][t] = 1;
          if (t >= 1) {
            for (int k = 0; k < pat.nnodes; k++) {
              if (edge[t - 1][k] != 0)
                edge[i][k] = 1;
            }
          }
        }
      }
      for (int i = pat.nnodes - 3; i >= 0; i--) {
        for (int j = 1; j < length[i]; j++) {
          int m = 0;
          // for all slots in the next level, 
          for (int k = 0; k < length[i + 1]; k++) {
            // if the slot depends on the current slot and the operation is intersection
            if (((pat.set_ops[i + 1][k] & 0xF) == j) && ((pat.set_ops[i + 1][k] & 0x20))) {
              // we add the upper bound of that slot to the current slot
              // the upper bound has to be vertex above level i 
              m |= (pat.partial[i + 1][k] & ((1 << (i+1) -1 )));
            }
          }
          pat.partial[i][j] = m;
        }
      }
    }
  };
}