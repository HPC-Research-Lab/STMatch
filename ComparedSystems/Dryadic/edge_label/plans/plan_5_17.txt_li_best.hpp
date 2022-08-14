const int n_counters = 1;
std::vector<std::vector<uint64_t>> global_counters(omp_get_max_threads());
const vidType vertices = g.V();
#pragma omp parallel
{
  std::vector<uint64_t> &counter = global_counters.at(omp_get_thread_num());
  counter.resize(n_counters, 0);
    VertexSet l0 = g.L(0);
    VertexSet l1 = g.L(1);
    VertexSet l2 = g.L(2);
    VertexSet l3 = g.L(3);
    VertexSet l4 = g.L(4);
    #pragma omp for schedule(dynamic,64) nowait
    for(vidType v0:l0)    {
      VertexSet y0l1 = g.N(v0,1);
      VertexSet y0l2 = g.N(v0,2);
      VertexSet y0l3 = g.N(v0,3);
      VertexSet y0l4 = g.N(v0,4);
      for(vidType v1:y0l1)      {
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        for(vidType v2:y0y1l2)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1n2l4 = difference_set(y0y1l4, y2l4);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[0] += difference_num(y0y1n2l4, y3l4);
          }
        }
      }
      for(vidType v1:y0l2)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0n1l4 = difference_set(y0l4, y1l4);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0n1y2l4 = intersection_set(y0n1l4, y2l4);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += difference_num(y0n1y2l4, y3l4);
          }
        }
      }
      for(vidType v1:y0l3)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0n1l4 = difference_set(y0l4, y1l4);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0n1y2l4 = intersection_set(y0n1l4, y2l4);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += difference_num(y0n1y2l4, y3l4);
          }
        }
      }
      for(vidType v1:y0l4)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0n1l2 = difference_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0n1l3 = difference_set(y0l3, y1l3);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0n1y2l2 = intersection_set(y0n1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0n1y2l3 = intersection_set(y0n1l3, y2l3);
          for(vidType v3:y0n1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            counter[1] += intersection_num(y0n1y2l3, y3l3);
          }
        }
      }
    }
    #pragma omp for schedule(dynamic,64) nowait
    for(vidType v0:l1)    {
      VertexSet y0l0 = g.N(v0,0);
      VertexSet y0l2 = g.N(v0,2);
      VertexSet y0l3 = g.N(v0,3);
      VertexSet y0l4 = g.N(v0,4);
      for(vidType v1:y0l0)      {
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        for(vidType v2:y0y1l2)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1n2l4 = difference_set(y0y1l4, y2l4);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += difference_num(y0y1n2l4, y3l4);
          }
        }
      }
      for(vidType v1:y0l2)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0n1l4 = difference_set(y0l4, y1l4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0n1y2l4 = intersection_set(y0n1l4, y2l4);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += difference_num(y0n1y2l4, y3l4);
          }
        }
      }
      for(vidType v1:y0l3)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0n1l4 = difference_set(y0l4, y1l4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0n1y2l4 = intersection_set(y0n1l4, y2l4);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += difference_num(y0n1y2l4, y3l4);
          }
        }
      }
      for(vidType v1:y0l4)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0n1l2 = difference_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0n1l3 = difference_set(y0l3, y1l3);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0n1y2l2 = intersection_set(y0n1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0n1y2l3 = intersection_set(y0n1l3, y2l3);
          for(vidType v3:y0n1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            counter[1] += intersection_num(y0n1y2l3, y3l3);
          }
        }
      }
    }
    #pragma omp for schedule(dynamic,64) nowait
    for(vidType v0:l2)    {
      VertexSet y0l0 = g.N(v0,0);
      VertexSet y0l1 = g.N(v0,1);
      VertexSet y0l3 = g.N(v0,3);
      VertexSet y0l4 = g.N(v0,4);
      for(vidType v1:y0l0)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet n0y1l4; difference_set(n0y1l4,y1l4, y0l4);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet n0y1y2l4 = intersection_set(n0y1l4, y2l4);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += difference_num(n0y1y2l4, y3l4);
          }
        }
      }
      for(vidType v1:y0l1)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet n0y1l4; difference_set(n0y1l4,y1l4, y0l4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet n0y1y2l4 = intersection_set(n0y1l4, y2l4);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += difference_num(n0y1y2l4, y3l4);
          }
        }
      }
      for(vidType v1:y0l3)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l4 = g.N(v1,4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet n0n1y2l4; difference_set(n0n1y2l4,difference_set(n0n1y2l4,y2l4, y0l4), y1l4);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += intersection_num(n0n1y2l4, y3l4);
          }
        }
      }
    }
    #pragma omp for schedule(dynamic,64) nowait
    for(vidType v0:l3)    {
      VertexSet y0l0 = g.N(v0,0);
      VertexSet y0l1 = g.N(v0,1);
      VertexSet y0l2 = g.N(v0,2);
      VertexSet y0l4 = g.N(v0,4);
      for(vidType v1:y0l0)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet n0y1l4; difference_set(n0y1l4,y1l4, y0l4);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet n0y1y2l4 = intersection_set(n0y1l4, y2l4);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += difference_num(n0y1y2l4, y3l4);
          }
        }
      }
      for(vidType v1:y0l1)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet n0y1l4; difference_set(n0y1l4,y1l4, y0l4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet n0y1y2l4 = intersection_set(n0y1l4, y2l4);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += difference_num(n0y1y2l4, y3l4);
          }
        }
      }
      for(vidType v1:y0l2)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l4 = g.N(v1,4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet n0n1y2l4; difference_set(n0n1y2l4,difference_set(n0n1y2l4,y2l4, y0l4), y1l4);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l4 = g.N(v3,4);
            counter[1] += intersection_num(n0n1y2l4, y3l4);
          }
        }
      }
    }
    #pragma omp for schedule(dynamic,64) nowait
    for(vidType v0:l4)    {
      VertexSet y0l0 = g.N(v0,0);
      VertexSet y0l1 = g.N(v0,1);
      VertexSet y0l2 = g.N(v0,2);
      VertexSet y0l3 = g.N(v0,3);
      for(vidType v1:y0l0)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet n0y1l2; difference_set(n0y1l2,y1l2, y0l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet n0y1l3; difference_set(n0y1l3,y1l3, y0l3);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet n0y1y2l2 = intersection_set(n0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet n0y1y2l3 = intersection_set(n0y1l3, y2l3);
          for(vidType v3:n0y1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            counter[1] += intersection_num(n0y1y2l3, y3l3);
          }
        }
      }
      for(vidType v1:y0l1)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet n0y1l2; difference_set(n0y1l2,y1l2, y0l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet n0y1l3; difference_set(n0y1l3,y1l3, y0l3);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet n0y1y2l2 = intersection_set(n0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet n0y1y2l3 = intersection_set(n0y1l3, y2l3);
          for(vidType v3:n0y1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            counter[1] += intersection_num(n0y1y2l3, y3l3);
          }
        }
      }
    }
}//217
const uint64_t data_complexity = 1.72811e+09;
const uint64_t time_complexity = 1.42651e+09;
