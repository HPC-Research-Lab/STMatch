#include "def.h"

// template<class T>
// void print_vector(const std::vector<T> &v)
// {
//   std::copy(v.begin(), v.end(), std::ostream_iterator<T>(std::cout, " "));
//   //std::cout << std::endl;
// }
// 
// void print_vector(std::vector<T> &v)
// {
//   std::copy(v.begin(), v.end(), std::ostream_iterator<T>(std::cout, " "));
//   //std::cout << std::endl;
// }

template<class T>
void print_set(std::set<T> &v)
{
  std::copy(v.begin(), v.end(), std::ostream_iterator<T>(std::cout, " "));
  std::cout << std::endl;
}

std::ostream & operator<<(std::ostream & os, const RestSet& rs) 
{
  os << "RestSet: ins: (";
  std::copy(rs.ins.begin(), rs.ins.end(), std::ostream_iterator<int>(std::cerr, " "));
  os << ") out: (";
  std::copy(rs.out.begin(), rs.out.end(), std::ostream_iterator<int>(std::cerr, " "));
  os << ") restriction: (";
  for(auto i : rs.res_chain)
    os << i << ",";
  os << ")";
  return os;
}

std::ostream & operator<<(std::ostream & os, RestSet& rs) 
{
  os << "RestSet: ins: (";
  std::copy(rs.ins.begin(), rs.ins.end(), std::ostream_iterator<int>(std::cerr, " "));
  os << ") out: (";
  std::copy(rs.out.begin(), rs.out.end(), std::ostream_iterator<int>(std::cerr, " "));
  os << ") restriction: (";
  for(auto i : rs.res_chain)
    os << i << ",";
  os << ")";
  return os;
}

std::ostream & operator<<(std::ostream & os, RestPlan & rp)
{
  std::cout << "rest: (";
  //print_vector<int>(rp.rest);
  std::cout << ") \n loopns: \n";
  for(auto rs : rp.loopons)
    std::cout << rs << "\n";
  //std::cout << "depends:\n";
  int i=0; 
  for(auto d : rp.depends) {
    std::cout << "depends[" << i++ << "]: ";
    for(auto rs : d) {
      std::cout << "[" << rs << "]";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  return os;
}
