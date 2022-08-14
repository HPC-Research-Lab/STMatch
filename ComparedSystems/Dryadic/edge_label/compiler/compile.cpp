#include "def.h"

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>

#include <cmath>
#include <cstdio>

#include <omp.h>

#define HERE //std::cerr << "HERE at " << __FILE__ << ":" << __LINE__ << "\n";

std::ostream& operator<<(std::ostream &stream, const std::vector<bool> &vb) {
  for(bool x : vb) stream << x;
  return stream;
}

struct TimeStamp {
  std::chrono::high_resolution_clock::time_point t;
  TimeStamp() {
    t = std::chrono::high_resolution_clock::now();
  }
  double operator-(const TimeStamp &start) const {
    return std::chrono::duration<double, std::ratio<1,1>>(t - start.t).count();
  }
};

void usage_error() {
  fprintf(stderr, "usage: ./compile [ --clique <n_vertices> ] [ --motif <n_vertices> ] [ --pattern <file> ] [ --pattern-list <file>] \n");
  exit(1);
}

void makeIter(MultiRestPlan& mrp, std::vector<Graphlet> all,int& total){
  std::vector<std::vector<ExecutionPlan>> planss;
  std::vector<std::vector<std::vector<ExecutionPlan>>> iterplanss;
  for(int i=0; i< all.size(); ++i){
    std::vector<ExecutionPlan> plans;
    std::vector<std::vector<ExecutionPlan>> iterplans;
    //DirectedGraphlet dg(all.at(i));
    MultiRed mr(all.at(i),plans,iterplans);
    //multiplicity_reduced_graph(dg,plans);
    planss.push_back(plans);
    iterplanss.push_back(iterplans);
  }
  for(int i=0;i<iterplanss.size();++i){
    std::cout<<i<<" whoop"<<std::endl;
    std::vector<std::vector<ExecutionPlan>> iterplans = iterplanss[i];
    for(int k=0;k<iterplans.size();++k){
      std::vector<ExecutionPlan> plans = iterplans[k];
      double bestcomplex = std::numeric_limits<double>::infinity();
      int bindex = 0;
      for(int j=0;j<plans.size();++j){
	RestPlan test(plans[j],i);
	double comple = test.time_complexity();
	std::cout<<"plan "<<j<<" for graphlet "<<i<<" has expected complexity "<<comple<<std::endl;
	if(comple<bestcomplex){
	  bindex=j;
	  bestcomplex = comple;
	}
      }
      std::cout<<"Chose plan "<<bindex<<" as a best for graphlet "<<i<<std::endl;
      mrp.add_ex_plan(plans[bindex],i);
    }
  }
}

void makeNorm(MultiRestPlan& mrp, std::vector<Graphlet> all,int& total){
  std::vector<std::vector<ExecutionPlan>> planss;
  for(int i=0; i< all.size(); ++i){
    std::vector<ExecutionPlan> plans;
    MultiRed mr(all.at(i),plans);
    planss.push_back(plans);
    total += plans.size();
  }
  for(int i=0;i<planss.size();++i){
    std::vector<ExecutionPlan> plans = planss.at(i);
    printf("%lu plans for graph %d = \n",plans.size(),i);
    double bestcomplex = std::numeric_limits<double>::infinity();
    int bindex = 0;
    for(int j=0;j<plans.size();++j){
      RestPlan test(plans[j],i);
      double comple = test.time_complexity();
      std::cout<<"plan "<<j<<" for graphlet "<<i<<" has expected complexity "<<comple<<std::endl;
      if(comple<bestcomplex){
        bindex=j;
        bestcomplex = comple;
      }
    }
    std::cout<<"Chose plan "<<bindex<<" as best for graphlet "<<i<<std::endl;
    mrp.add_ex_plan(plans[bindex],i);
  }
}

int main(int argc, char **argv) {
  if(argc != 3) usage_error();
  int openmpdepth=0;
  int startdepth = 0;
  int mode = 0;
  std::string arg1(argv[1]);
  std::vector<Graphlet> all;
  std::string plan_name;
  bool labelled = false;
  bool edge = false;
  bool iter = false;
  if(arg1 == "--clique") {
    int graphlet_size = atoi(argv[2]);
    printf("Using clique-%d\n", graphlet_size);
    all.push_back(Graphlet::clique(graphlet_size));
    std::ostringstream oss;
    oss << "clique_" << graphlet_size;
    plan_name = oss.str();
  } else if(arg1 == "--motif") {
    int graphlet_size = atoi(argv[2]);
    printf("Using motif-%d\n", graphlet_size);
    TimeStamp t1;
    all = Graphlet::all_connected(graphlet_size);
    TimeStamp t2;
    printf("Generated all %lu connected in %f\n",all.size(),t2-t1);
    std::ostringstream oss;
    oss << "mr_motif_" << graphlet_size;
    plan_name = oss.str();
  } else if(arg1 == "--pattern") {
    std::string fname(argv[2]);
    all.push_back(Graphlet::from_file(fname));
    size_t last_slash = fname.find_last_of('/');
    //if(last_slash == std::string::npos) last_slash = 0;
    std::string temp = fname.substr(last_slash+1);
    plan_name = temp.substr(0, temp.size());
    printf("Using pattern: %s\n", plan_name.c_str());
  }else if(arg1 == "--pattern-list"){
    std::string fname(argv[2]);
    std::ifstream infile(fname.c_str());
    labelled=false;
    all = parseGraphList(infile,labelled);
    infile.close();
    size_t last_slash = fname.find_last_of('/');
    //if(last_slash == std::string::npos) last_slash = 0;
    std::string temp = fname.substr(last_slash+1);
    plan_name = temp.substr(0, temp.size());
    printf("Using pattern list: %s\n", plan_name.c_str());
  }else if(arg1 == "--pattern-list-i"){
    edge=true;
    std::string fname(argv[2]);
    std::ifstream infile(fname.c_str());
    labelled=false;
    all = parseGraphList(infile,labelled);
    infile.close();
    size_t last_slash = fname.find_last_of('/');
    //if(last_slash == std::string::npos) last_slash = 0;
    std::string temp = fname.substr(last_slash+1);
    plan_name = temp.substr(0, temp.size())+"_i";
    printf("Using pattern list: %s\n", plan_name.c_str());
    iter = true;
  }else if(arg1 == "--pattern-list-l"){
    std::string fname(argv[2]);
    std::ifstream infile(fname.c_str());
    labelled=true;
    all = parseGraphList(infile,labelled);
    infile.close();
    size_t last_slash = fname.find_last_of('/');
    //if(last_slash == std::string::npos) last_slash = 0;
    std::string temp = fname.substr(last_slash+1);
    plan_name = temp.substr(0, temp.size())+"_l";
    printf("Using pattern list: %s\n", plan_name.c_str());
  }else if(arg1 == "--pattern-list-li"){
    edge=true;
    std::string fname(argv[2]);
    std::ifstream infile(fname.c_str());
    labelled=true;
    iter=true;
    all = parseGraphList(infile,labelled);
    infile.close();
    size_t last_slash = fname.find_last_of('/');
    //if(last_slash == std::string::npos) last_slash = 0;
    std::string temp = fname.substr(last_slash+1);
    plan_name = temp.substr(0, temp.size())+"_li";
    printf("Using pattern list: %s\n", plan_name.c_str());
  }else if(arg1 == "--pattern-list-lic"){
    mode = 4;
    edge=true;
    std::string fname(argv[2]);
    std::ifstream infile(fname.c_str());
    labelled=true;
    iter=true;
    all = parseGraphList(infile,labelled);
    infile.close();
    size_t last_slash = fname.find_last_of('/');
    //if(last_slash == std::string::npos) last_slash = 0;
    std::string temp = fname.substr(last_slash+1);
    plan_name = temp.substr(0, temp.size())+"_lic";
    printf("Using pattern list: %s\n", plan_name.c_str());
  }else if(arg1 == "--pattern-list-ic"){
    mode = 4;
    edge=true;
    std::string fname(argv[2]);
    std::ifstream infile(fname.c_str());
    labelled=false;
    iter=true;
    all = parseGraphList(infile,labelled);
    infile.close();
    size_t last_slash = fname.find_last_of('/');
    //if(last_slash == std::string::npos) last_slash = 0;
    std::string temp = fname.substr(last_slash+1);
    plan_name = temp.substr(0, temp.size())+"_ic";
    printf("Using pattern list: %s\n", plan_name.c_str());
  }else usage_error();

  TimeStamp t3;
  int total = 0;
  if(iter)startdepth=openmpdepth=2;
  //best plans
  MultiRestPlan mrp(0,labelled);
  //worst plans
  if(iter)makeIter(mrp,all,total);
  else    makeNorm(mrp,all,total);
  //MultiRestPlan mrw(0,labelled);
  TimeStamp t4;
  printf("Generated all %d plans in %f\n",total,t4-t3);
  
  //Don't worry about combining them for now
  //auto plan = hp.greedy();
  //TimeStamp t4;
  {
    std::ostringstream fname;
    fname<<"../plans/" IFGPU(<< "gpu_") << "plan_" << plan_name <<"_best.hpp";
    std::cout << "Writing best plan to " << fname.str() << "\n";
    std::ofstream planfile(fname.str());
    std::string header, code;
    std::tie(header, code) = mrp.to_code(openmpdepth,startdepth,mode);
    planfile << code;
    planfile.close();
    fname << ".h";
    std::ofstream headfile(fname.str());
    
    if(labelled)
      headfile<<"#define LABELLED\n";
    if(edge)
      headfile<<"#define EDGE_INDUCED\n";
    headfile.flush();
    headfile.close();
      //);
  }
  /*
  {
    std::ostringstream fname;
    fname<<"../plans/" IFGPU(<< "gpu_") << "plan_" << plan_name <<"_worst.hpp";
    std::cout << "Writing worst plan to " << fname.str() << "\n";
    std::ofstream planfile(fname.str());
    std::string header, code;
    std::tie(header, code) = mrw.to_code();
    planfile << code;
    planfile.close();
    fname << ".h";
    std::ofstream headfile(fname.str());
    //headfile << header;
    if(labelled)
      headfile<<"#define LABELLED\n";
    if(edge)
      headfile<<"#define EDGE_INDUCED\n";
    headfile.flush();
    headfile.close();
    
    }*/
  return 0;
}

