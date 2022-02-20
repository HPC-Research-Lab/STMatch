#pragma once

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <tuple>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>


namespace libra {

  struct Graph {

    size_t nnodes = 0, nedges = 0;
    int* vertex_label_;
    size_t* rowptr_;
    size_t* colidx_;

    Graph* to_gpu() {
      Graph g;
      g.nnodes = nnodes;
      g.nedges = nedges;

      cudaMalloc(&g.vertex_label_, sizeof(int) * nnodes);
      cudaMalloc(&g.rowptr_, sizeof(size_t) * (nnodes + 1));
      cudaMalloc(&g.colidx_, sizeof(size_t) * nedges);
      cudaMemcpy(g.vertex_label_, vertex_label_, sizeof(int) * nnodes, cudaMemcpyHostToDevice);
      cudaMemcpy(g.rowptr_, rowptr_, sizeof(size_t) * (nnodes + 1), cudaMemcpyHostToDevice);
      cudaMemcpy(g.colidx_, colidx_, sizeof(size_t) * nedges, cudaMemcpyHostToDevice);

      Graph *gpu_g;
      cudaMalloc(&gpu_g, sizeof(Graph));
      cudaMemcpy(gpu_g, &g, sizeof(Graph), cudaMemcpyHostToDevice);
      return gpu_g;
    }
  };

  struct GraphPreprocessor {

    Graph g_;

    GraphPreprocessor(std::string filename) {
      readfile(filename);
    }


    void readfile(std::string& filename) {

      std::ifstream fin(filename);
      std::string line;
      while (std::getline(fin, line) && (line[0] == '#'));
      g_.nnodes = 0;
      std::vector<int> vertex_labels;
      do {
        std::istringstream sin(line);
        char tmp;
        int v;
        int label;
        sin >> tmp >> v >> label;
        //assert(tmp == 'v');
        vertex_labels.push_back(label);
        g_.nnodes++;
      } while (std::getline(fin, line) && (line[0] == 'v'));
      std::vector<std::vector<int>> adj_list(g_.nnodes);
      do {
        std::istringstream sin(line);
        char tmp;
        int v1, v2, label;
        sin >> tmp >> v1 >> v2 >> label;
        //assert(tmp == 'e');
        adj_list[v1].push_back(v2);
        adj_list[v2].push_back(v1);
      } while (getline(fin, line));

      assert(vertex_labels.size() == g_.nnodes);

      g_.vertex_label_ = new int[vertex_labels.size()];
      memcpy(g_.vertex_label_, vertex_labels.data(), sizeof(int) * vertex_labels.size());

      g_.rowptr_ = new size_t[g_.nnodes + 1];
      g_.rowptr_[0] = 0;

      std::vector<size_t> colidx;

      for (int i = 0; i < g_.nnodes; i++) {
        sort(adj_list[i].begin(), adj_list[i].end());
        int pos = 0;
        for (int j = 1; j < adj_list[i].size(); j++) {
          if (adj_list[i][j] != adj_list[i][pos]) adj_list[i][++pos] = adj_list[i][j];
        }

        if (adj_list[i].size() > 0)
          colidx.insert(colidx.end(), adj_list[i].data(), adj_list[i].data() + pos + 1);  // adj_list is sorted

        adj_list[i].clear();
        g_.rowptr_[i + 1] = colidx.size();
      }
      g_.nedges = colidx.size();
      g_.colidx_ = new size_t[colidx.size()];

      memcpy(g_.colidx_, colidx.data(), sizeof(size_t) * colidx.size());

      std::cout << "Graph read complete. Number of vertex: " << g_.nnodes << std::endl;
    }
  };
} 