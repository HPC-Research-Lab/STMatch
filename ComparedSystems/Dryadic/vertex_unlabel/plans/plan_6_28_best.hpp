const int n_counters = 1;
std::vector<std::vector<uint64_t>> global_counters(omp_get_max_threads());
const vidType vertices = g.V();
#pragma omp parallel
{
  std::vector<uint64_t> &counter = global_counters.at(omp_get_thread_num());
  counter.resize(n_counters, 0);
  #pragma omp for schedule(dynamic,1) nowait
  for(vidType v0 = 0; v0 < vertices; v0++) {
    VertexSet y0 = g.N(v0);
    VertexSet y0f0 = bounded(y0,v0);
    for(vidType idx1 = 0; idx1 < y0f0.size(); idx1++) {
      vidType v1 = y0f0.begin()[idx1];
      VertexSet y1 = g.N(v1);
      VertexSet y0y1 = intersection_set(y0, y1);
      for(vidType idx2 = 0; idx2 < y0y1.size(); idx2++) {
        vidType v2 = y0y1.begin()[idx2];
        VertexSet y2 = g.N(v2);
        VertexSet y0y1n2f2 = difference_set(y0y1, y2, v2);
        VertexSet n0n1y2; difference_set(n0n1y2,difference_set(n0n1y2,y2, y0), y1);
        for(vidType idx3 = 0; idx3 < y0y1n2f2.size(); idx3++) {
          vidType v3 = y0y1n2f2.begin()[idx3];
          VertexSet y3 = g.N(v3);
          VertexSet n0n1n2y3; difference_set(n0n1n2y3,difference_set(n0n1n2y3,difference_set(n0n1n2y3,y3, y0), y1), y2);
          VertexSet n0n1y2n3 = difference_set(n0n1y2, y3);
          for(vidType idx4 = 0; idx4 < n0n1n2y3.size(); idx4++) {
            vidType v4 = n0n1n2y3.begin()[idx4];
            VertexSet y4 = g.N(v4);
            counter[0] += difference_num(n0n1y2n3, y4);
          }
        }
      }
    }
  }
}
const uint64_t data_complexity = 10399.9;
const uint64_t time_complexity = 13157.7;
