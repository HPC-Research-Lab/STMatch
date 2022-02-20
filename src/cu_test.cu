

#include <string>
#include <chrono>
#include <iostream>
#include <dirent.h>
#include <string>
#include "src/GraphPreprocessor.h"
#include "src/PatternPreprocessor.h"
#include "src/KernelLauncher.cuh"

using namespace std;

int main(int argc, char* argv[]) {

  libra::GraphPreprocessor gp(argv[1]);
  libra::PatternPreprocessor pp(argv[2]);

  libra::KernelLauncher kl(gp, pp);
  kl.start();
  return 0;
}