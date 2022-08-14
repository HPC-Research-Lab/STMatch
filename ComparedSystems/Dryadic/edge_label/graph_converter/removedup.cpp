#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <algorithm>
using namespace std;

#include <omp.h>

#include "def.h"

inline bool nextSNAPline(ifstream &infile, string &line, istringstream &iss, vidType &src, vidType &dest) {
  do {
    if(!getline(infile, line))
      return false;
  } while(line.length() == 0 || line[0] == '#');
  iss.clear();
  iss.str(line);
  return !!(iss >> src >> dest);
}

void snapToBin(string fname) {
  constexpr int inc = 65536;
  ifstream infile(fname.c_str());
  ofstream outfile((fname + ".nodup").c_str());
  if(!infile || !outfile) {
    cout << "File not available\n";
    throw 1;
  }
  std::vector<uint64_t> degrees(inc);
  //vidType nextID = 0;
  //vector<vidType> idMap;
  //idMap.reserve(2048);
  vidType max_id = 0;
  string line;
  istringstream iss;
  vidType edge[2];
  size_t lineNum = 0;
  while(nextSNAPline(infile, line, iss, edge[0], edge[1])) {
    if(++lineNum % 1000000 == 0) LOG("%lu edges read", lineNum);
    //getID(idMap, edge[0], nextID);
    //getID(idMap, edge[1], nextID);
    if(edge[0]!=edge[1])outfile<<edge[0]<<" "<<edge[1]<<std::endl;
  }
  infile.close();
  outfile.close();
}

int main(int argc, char** argv) {
  if(argc != 2) {
    cerr << "usage: ./snapToBinNoRelabel <snap file>\n";
    return 1;
  }
  string fname = argv[1];
  snapToBin(fname);
  cout << "snapToBin done\n" << std::flush;
  return 0;
}

