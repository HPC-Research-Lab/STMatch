#pragma once

#include <cstddef>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>
#include "config.h"


namespace STMatch {

  typedef struct {

    graph_node_t nnodes = 0;
    graph_edge_t nedges = 0;
    bitarray32* vertex_label;
    graph_edge_t* rowptr;
    graph_node_t* colidx;
  } Graph;

  struct GraphPreprocessor {

    Graph g;

    GraphPreprocessor(std::string filename) {
      readfile(filename);
    }

    Graph* to_gpu() {
      Graph gcopy = g;

      cudaMalloc(&gcopy.vertex_label, sizeof(bitarray32) * g.nnodes);
      cudaMalloc(&gcopy.rowptr, sizeof(graph_edge_t) * (g.nnodes + 1));
      cudaMalloc(&gcopy.colidx, sizeof(graph_node_t) * g.nedges);
      cudaMemcpy(gcopy.vertex_label, g.vertex_label, sizeof(bitarray32) * g.nnodes, cudaMemcpyHostToDevice);
      cudaMemcpy(gcopy.rowptr, g.rowptr, sizeof(graph_edge_t) * (g.nnodes + 1), cudaMemcpyHostToDevice);
      cudaMemcpy(gcopy.colidx, g.colidx, sizeof(graph_node_t) * g.nedges, cudaMemcpyHostToDevice);

      Graph* gpu_g;
      cudaMalloc(&gpu_g, sizeof(Graph));
      cudaMemcpy(gpu_g, &gcopy, sizeof(Graph), cudaMemcpyHostToDevice);
      return gpu_g;
    }

    // TODO: dryadic graph format 

    void readfile(std::string& filename) {
      //read_lg_file(filename);
      read_bin_file(filename);

    }

    void read_lg_file(std::string& filename) {
      std::ifstream fin(filename);
      std::string line;
      while (std::getline(fin, line) && (line[0] == '#'));
      g.nnodes = 0;
      std::vector<int> vertex_labels;
      do {
        std::istringstream sin(line);
        char tmp;
        int v;
        int label;
        sin >> tmp >> v >> label;
        vertex_labels.push_back(label);
        g.nnodes++;
      } while (std::getline(fin, line) && (line[0] == 'v'));
      std::vector<std::vector<graph_node_t>> adj_list(g.nnodes);
      do {
        std::istringstream sin(line);
        char tmp;
        int v1, v2;
        int label;
        sin >> tmp >> v1 >> v2 >> label;
        adj_list[v1].push_back(v2);
        adj_list[v2].push_back(v1);
      } while (getline(fin, line));

      assert(vertex_labels.size() == g.nnodes);

      g.vertex_label = new bitarray32[vertex_labels.size()];
      for (int i = 0; i < g.nnodes; i++) {
        g.vertex_label[i] = (1 << vertex_labels[i]);
      }
      // memcpy(g.vertex_label, vertex_labels.data(), sizeof(int) * vertex_labels.size());

      g.rowptr = new graph_edge_t[g.nnodes + 1];
      g.rowptr[0] = 0;

      std::vector<graph_node_t> colidx;

      for (graph_node_t i = 0; i < g.nnodes; i++) {
        sort(adj_list[i].begin(), adj_list[i].end());
        int pos = 0;
        for (graph_node_t j = 1; j < adj_list[i].size(); j++) {
          if (adj_list[i][j] != adj_list[i][pos]) adj_list[i][++pos] = adj_list[i][j];
        }

        if (adj_list[i].size() > 0)
          colidx.insert(colidx.end(), adj_list[i].data(), adj_list[i].data() + pos + 1);  // adj_list is sorted

        adj_list[i].clear();
        g.rowptr[i + 1] = colidx.size();
      }
      g.nedges = colidx.size();
      g.colidx = new graph_node_t[colidx.size()];

      memcpy(g.colidx, colidx.data(), sizeof(graph_node_t) * colidx.size());

     // std::cout << "Graph read complete. Number of vertex: " << g.nnodes << std::endl;
    }


    template<typename T>
    void read_subfile(std::string fname, T*& pointer, size_t elements) {
      pointer = (T*)malloc(sizeof(T) * elements);
      assert(pointer);
      std::ifstream inf(fname.c_str(), std::ios::binary);
      if (!inf.good()) {
        std::cerr << "Failed to open file: " << fname << "\n";
        exit(1);
      }
      inf.read(reinterpret_cast<char*>(pointer), sizeof(T) * elements);
      inf.close();
    }


    void read_bin_file(std::string& filename) {
      std::ifstream f_meta((filename + ".meta.txt").c_str());
      assert(f_meta);

      graph_node_t n_vertices;
      graph_edge_t n_edges;
      int vid_size;
      graph_node_t max_degree;
      f_meta >> n_vertices >> n_edges >> vid_size >> max_degree;
      assert(sizeof(graph_node_t) == vid_size);
      f_meta.close();

      g.nnodes = n_vertices;
      g.nedges = n_edges;
      read_subfile(filename + ".vertex.bin", g.rowptr, n_vertices + 1);
      read_subfile(filename + ".edge.bin", g.colidx, n_edges);

      int* lb = new int[n_vertices];
      memset(lb, 1, n_vertices * sizeof(int));
      g.vertex_label = new bitarray32[n_vertices];
      if(LABELED) {
        read_subfile(filename + ".label.bin", lb, n_vertices);
      }
      for (int i = 0; i < n_vertices; i++) {
        g.vertex_label[i] = (1 << lb[i]);
      }
      delete[] lb;
    }

  };
}