#include <string>
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
  if(argc != 2) {
    cerr << "usage: ./readBin <fname>\n";
    return 1;
  }
  ifstream inf(argv[1], ios::binary);
  uint32_t src, dst;
  for(int i = 0; i < 10; i++) {
    inf.read(reinterpret_cast<char*>(&src), sizeof(uint32_t));
    inf.read(reinterpret_cast<char*>(&dst), sizeof(uint32_t));
    cout << src << "\t" << dst << "\tor\t" << __builtin_bswap32(src) << "\t" << __builtin_bswap32(dst) << "\n";
  }
  cout << "...\n";
  return 0;
}
