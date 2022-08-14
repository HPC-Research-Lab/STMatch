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
      VertexSet y0f0 = g.N(v0);
      for(vidType v1:y0f0)      {
        VertexSet y1 = g.N(v1);
        VertexSet y0y1 = intersection_set(y0, y1);
        VertexSet y0f0y1f1 = intersection_set(y0f0, y1, v1);
        for(vidType v2:y0f0y1f1)        {
          VertexSet y2 = g.N(v2);
          VertexSet y0y1y2 = intersection_set(y0y1, y2);
          for(vidType v3:y0y1y2)          {
            VertexSet y3 = g.N(v3);
            VertexSet y0y1y2n3f3 = difference_set(y0y1y2, y3, v3);
            VertexSet y0y1y2y3f3 = intersection_set(y0y1y2, y3, v3);
            for(vidType v4:y0y1y2y3f3)            {
              VertexSet y4 = g.N(v4);
              VertexSet y0y1y2n3f3y4 = intersection_set(y0y1y2n3f3, y4);
              VertexSet y0y1y2y3f3n4f4 = difference_set(y0y1y2y3f3, y4, v4);
              for(vidType v5:y0y1y2y3f3n4f4)              {
                VertexSet y5 = g.N(v5);
                counter[0] += intersection_num(y0y1y2n3f3y4, y5, v3);
              }
            }
          }
        }
      }
    }
}//217
const uint64_t data_complexity = 1.44948e+07;
const uint64_t time_complexity = 1.93504e+07;
