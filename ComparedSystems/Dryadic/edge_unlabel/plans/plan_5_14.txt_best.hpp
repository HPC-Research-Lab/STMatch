const int n_counters = 1;
std::vector<std::vector<uint64_t>> global_counters(omp_get_max_threads());
const vidType vertices = g.V();
#pragma omp parallel
{
  std::vector<uint64_t> &counter = global_counters.at(omp_get_thread_num());
  counter.resize(n_counters, 0);
        #pragma omp for schedule(dynamic,64) nowait
    for(vidType v0 = 0; v0 < vertices;++v0)    {
      VertexSet y0 = g.N(v0);
      for(vidType v1:y0)      {
        VertexSet y1 = g.N(v1);
        VertexSet n0y1; difference_set(n0y1,y1, y0);
        VertexSet y0y1f1 = intersection_set(y0, y1, v1);
        for(vidType v2:y0y1f1)        {
          VertexSet y2 = g.N(v2);
          VertexSet n0n1y2; difference_set(n0n1y2,difference_set(n0n1y2,y2, y0), y1);
          VertexSet n0y1n2 = difference_set(n0y1, y2);
          for(vidType v3:n0y1n2)          {
            VertexSet y3 = g.N(v3);
            counter[0] += intersection_num(n0n1y2, y3);
          }
        }
      }
    }
}//217
const uint64_t data_complexity = 5.84858e+07;
const uint64_t time_complexity = 3.38387e+07;
