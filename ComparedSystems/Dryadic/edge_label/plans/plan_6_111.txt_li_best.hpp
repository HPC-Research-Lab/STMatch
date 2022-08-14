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
    VertexSet l5 = g.L(5);
    #pragma omp for schedule(dynamic,64) nowait
    for(vidType v0:l0)    {
      VertexSet y0l1 = g.N(v0,1);
      VertexSet y0l2 = g.N(v0,2);
      VertexSet y0l3 = g.N(v0,3);
      VertexSet y0l4 = g.N(v0,4);
      VertexSet y0l5 = g.N(v0,5);
      for(vidType v1:y0l1)      {
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l2)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[0] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l2)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l3)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l4)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l5)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l4 = g.N(v4,4);
              counter[1] += intersection_num(y0y1y2y3l4, y4l4);
            }
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
      VertexSet y0l5 = g.N(v0,5);
      for(vidType v1:y0l0)      {
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l2)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l2)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l3)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l4)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l5)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l4 = g.N(v4,4);
              counter[1] += intersection_num(y0y1y2y3l4, y4l4);
            }
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
      VertexSet y0l5 = g.N(v0,5);
      for(vidType v1:y0l0)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l1)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l3)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l3)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l4)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l5)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l4 = g.N(v4,4);
              counter[1] += intersection_num(y0y1y2y3l4, y4l4);
            }
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
      VertexSet y0l5 = g.N(v0,5);
      for(vidType v1:y0l0)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l1)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l2)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l4)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l4)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l2 = g.N(v3,2);
            VertexSet y0y1y2y3l2 = intersection_set(y0y1y2l2, y3l2);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l2)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l5)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l2 = g.N(v3,2);
            VertexSet y0y1y2y3l2 = intersection_set(y0y1y2l2, y3l2);
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            for(vidType v4:y0y1y2y3l2)            {
              VertexSet y4l4 = g.N(v4,4);
              counter[1] += intersection_num(y0y1y2y3l4, y4l4);
            }
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
      VertexSet y0l5 = g.N(v0,5);
      for(vidType v1:y0l0)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l1)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l2)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l3)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l5 = g.N(v1,5);
        VertexSet y0y1l5 = intersection_set(y0l5, y1l5);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l5 = g.N(v2,5);
          VertexSet y0y1y2l5 = intersection_set(y0y1l5, y2l5);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l2 = g.N(v3,2);
            VertexSet y0y1y2y3l2 = intersection_set(y0y1y2l2, y3l2);
            VertexSet y3l5 = g.N(v3,5);
            VertexSet y0y1y2y3l5 = intersection_set(y0y1y2l5, y3l5);
            for(vidType v4:y0y1y2y3l2)            {
              VertexSet y4l5 = g.N(v4,5);
              counter[1] += intersection_num(y0y1y2y3l5, y4l5);
            }
          }
        }
      }
      for(vidType v1:y0l5)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l2 = g.N(v3,2);
            VertexSet y0y1y2y3l2 = intersection_set(y0y1y2l2, y3l2);
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            for(vidType v4:y0y1y2y3l2)            {
              VertexSet y4l3 = g.N(v4,3);
              counter[1] += intersection_num(y0y1y2y3l3, y4l3);
            }
          }
        }
      }
    }
    #pragma omp for schedule(dynamic,64) nowait
    for(vidType v0:l5)    {
      VertexSet y0l0 = g.N(v0,0);
      VertexSet y0l1 = g.N(v0,1);
      VertexSet y0l2 = g.N(v0,2);
      VertexSet y0l3 = g.N(v0,3);
      VertexSet y0l4 = g.N(v0,4);
      for(vidType v1:y0l0)      {
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        for(vidType v2:y0y1l1)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l4 = g.N(v4,4);
              counter[1] += intersection_num(y0y1y2y3l4, y4l4);
            }
          }
        }
      }
      for(vidType v1:y0l1)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          for(vidType v3:y0y1y2l2)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l4 = g.N(v4,4);
              counter[1] += intersection_num(y0y1y2y3l4, y4l4);
            }
          }
        }
      }
      for(vidType v1:y0l2)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            for(vidType v4:y0y1y2y3l3)            {
              VertexSet y4l4 = g.N(v4,4);
              counter[1] += intersection_num(y0y1y2y3l4, y4l4);
            }
          }
        }
      }
      for(vidType v1:y0l3)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l4 = g.N(v1,4);
        VertexSet y0y1l4 = intersection_set(y0l4, y1l4);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l4 = g.N(v2,4);
          VertexSet y0y1y2l4 = intersection_set(y0y1l4, y2l4);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l2 = g.N(v3,2);
            VertexSet y0y1y2y3l2 = intersection_set(y0y1y2l2, y3l2);
            VertexSet y3l4 = g.N(v3,4);
            VertexSet y0y1y2y3l4 = intersection_set(y0y1y2l4, y3l4);
            for(vidType v4:y0y1y2y3l2)            {
              VertexSet y4l4 = g.N(v4,4);
              counter[1] += intersection_num(y0y1y2y3l4, y4l4);
            }
          }
        }
      }
      for(vidType v1:y0l4)      {
        VertexSet y1l0 = g.N(v1,0);
        VertexSet y0y1l0 = intersection_set(y0l0, y1l0);
        VertexSet y1l1 = g.N(v1,1);
        VertexSet y0y1l1 = intersection_set(y0l1, y1l1);
        VertexSet y1l2 = g.N(v1,2);
        VertexSet y0y1l2 = intersection_set(y0l2, y1l2);
        VertexSet y1l3 = g.N(v1,3);
        VertexSet y0y1l3 = intersection_set(y0l3, y1l3);
        for(vidType v2:y0y1l0)        {
          VertexSet y2l1 = g.N(v2,1);
          VertexSet y0y1y2l1 = intersection_set(y0y1l1, y2l1);
          VertexSet y2l2 = g.N(v2,2);
          VertexSet y0y1y2l2 = intersection_set(y0y1l2, y2l2);
          VertexSet y2l3 = g.N(v2,3);
          VertexSet y0y1y2l3 = intersection_set(y0y1l3, y2l3);
          for(vidType v3:y0y1y2l1)          {
            VertexSet y3l2 = g.N(v3,2);
            VertexSet y0y1y2y3l2 = intersection_set(y0y1y2l2, y3l2);
            VertexSet y3l3 = g.N(v3,3);
            VertexSet y0y1y2y3l3 = intersection_set(y0y1y2l3, y3l3);
            for(vidType v4:y0y1y2y3l2)            {
              VertexSet y4l3 = g.N(v4,3);
              counter[1] += intersection_num(y0y1y2y3l3, y4l3);
            }
          }
        }
      }
    }
}//217
const uint64_t data_complexity = 3.22726e+09;
const uint64_t time_complexity = 3.64154e+09;
