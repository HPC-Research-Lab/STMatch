

#include <string>
#include <chrono>
#include <iostream>
#include <dirent.h>
#include <string>
#include "src/GraphPreprocessor.h"
#include "src/PatternPreprocessor.h"
#include "src/KernelLauncher.cuh"

using namespace std;

void scan_dir(std::string dir_path, std::vector<string>& path_list) {
  DIR* dir;
  struct dirent* ptr;
  dir = opendir(dir_path.c_str());

  while ((ptr = readdir(dir)) != NULL) {
    string file_name = std::string(ptr->d_name);
    if (file_name == "." || file_name == "..") continue;
    string file_path = dir_path + file_name;
    path_list.push_back(file_path);
  }
  closedir(dir);
}

int main(int argc, char* argv[]) {

  libra::GraphPreprocessor gp(argv[1]);
  libra::PatternPreprocessor pp(argv[2]);

  libra::KernelLauncher kl(gp.g_, pp.pat_);
  kl.start();
  return 0;
}