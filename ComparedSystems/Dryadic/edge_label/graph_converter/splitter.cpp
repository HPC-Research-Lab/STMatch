#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <set>
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
void missing_edge(vidType e1, vidType e2){
  std::cout<<"Tried to remove nonexistent edge "<< e1 << " " << e2 <<std::endl;
  exit(0);
}
void snapSplit(string fname) {
  constexpr int inc = 65536;
  ifstream infile(fname.c_str());
  ofstream oos((fname + ".start").c_str());
  ofstream ood((fname + ".del").c_str());
  ofstream ooa((fname + ".add").c_str());
  
  //ofstream outfile((fname + "-" + fname2+".bin").c_str(), ios::binary);
  //ofstream out_rev((fname + "-" + fname2+".rev.bin").c_str(), ios::binary);
  //ofstream deg_out((fname + "-" + fname2+".raw.degree.bin").c_str(), ios::binary);
  if(!infile || !ood || !oos || !ooa) {
    cout << "File not available\n"<<endl;
    throw 1;
  }
  const std::set<vidType> defa();
  std::vector<std::set<vidType>> graph(inc,std::set<vidType>());
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
    max_id = std::max(edge[0], max_id);
    max_id = std::max(edge[1], max_id);
    if(max_id >= graph.size()) graph.resize(max_id + inc,std::set<vidType>());
    graph[edge[0]].insert(edge[1]);
  }
  infile.close();
  
  std::vector<uint64_t> degrees(graph.size(),0);
  int en = 0;
  for(size_t i = 0; i < graph.size();++i){
    for(int z: graph[i]){
      //edge[0] = i;
      //edge[1] = z;
      //++degrees.at(edge[0]);
      //++degrees.at(edge[1]);
      //edge[0] = __builtin_bswap32(edge[0]); // conversion to big-endian
      //edge[1] = __builtin_bswap32(edge[1]);
      //outfile.write(reinterpret_cast<const char*>(edge), 2*sizeof(vidType));
      //std::swap(edge[0], edge[1]);
      //out_rev.write(reinterpret_cast<const char*>(edge), 2*sizeof(vidType));
      ++en;
      en%=5;
      switch(en){
      case 0:
	//delete, leave in original
	oos<<i<<" "<<z<<std::endl;
	ood<<i<<" "<<z<<std::endl;
	break;
      case 1:
	//add
	ooa<<i<<" "<<z<<std::endl;
	break;
      default:
	//just in original
	oos<<i<<" "<<z<<std::endl;
	break;
      }
    }
  }  
  //outfile.close();
  oos.close();
  ooa.close();
  ood.close();
  //cout << "\nwriting degrees\n" << std::flush;
  //deg_out.write(reinterpret_cast<const char*>(degrees.data()), (max_id + 1) * sizeof(uint64_t));
  //deg_out.close();
}

int main(int argc, char** argv) {
  if(argc != 2) {
    cerr << "usage: ./splitter <snap file>\n";
    return 1;
  }
  string fname = argv[1];
  //string fname2 = argv[2];
  snapSplit(fname);
  cout << "snap split done\n" << std::flush;
  return 0;
}

