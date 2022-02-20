 
#pragma once

#include <chrono>
#include <iostream>
#include <vector>
#include <set>
#include <array>
 
 namespace libra { 
  class Timer {
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    std::chrono::time_point<std::chrono::high_resolution_clock> end;
    double duration;

  public:
    Timer() : duration(0.0) {}

    void start() { begin = std::chrono::high_resolution_clock::now(); duration = 0; }

    void stop() {
      end = std::chrono::high_resolution_clock::now();
      duration += (std::chrono::time_point_cast<std::chrono::microseconds>(end)
        .time_since_epoch()
        .count() -
        std::chrono::time_point_cast<std::chrono::microseconds>(begin)
        .time_since_epoch()
        .count()) *
        1e-6;
    }

    double get() { return duration; }
  };
 }