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
  fprintf(stderr, "usage: ./compile [ --clique <n_vertices> ] [ --motif <n_vertices> ] [ --pattern <file> ]\n");
  exit(1);
}

int main(int argc, char **argv) {
  if(argc != 3) usage_error();
  std::string arg1(argv[1]);
  std::vector<Graphlet> all;
  std::string plan_name;
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
    plan_name = temp.substr(0, temp.size()-4);
    printf("Using pattern: %s\n", plan_name.c_str());
  } else usage_error();

  TimeStamp t3;
  int total = 0;
  std::vector<std::vector<ExecutionPlan>> planss;
  for(int i=0; i< all.size(); ++i){
    std::vector<ExecutionPlan> plans;
    MultiRed mr(all.at(i),plans);
    planss.push_back(plans);
    total += plans.size();
  }
  //best plans
  MultiRestPlan mrp(0);
  //worst plans
  MultiRestPlan mrw(0);
  TimeStamp t4;
  printf("Generated all %d plans in %f\n",total,t4-t3);
  
  for(int i=0;i<planss.size();++i){
    std::vector<ExecutionPlan> plans = planss.at(i);
    printf("%lu plans for graph %d = \n",plans.size(),i);
    //std::cout<<all[i].toString()<<std::endl;
    double bestcomplex = std::numeric_limits<double>::infinity();
    int bindex = 0;
    int windex = 0;
    double worstcomplex = 0;
    for(int j=0;j<plans.size();++j){
      RestPlan test(plans[j],i);
      double comple = test.time_complexity();
      //std::cout<<"plan "<<j<<" for graphlet "<<i<<" has expected complexity "<<comple<<std::endl;
      if(comple<bestcomplex){
        bindex=j;
        bestcomplex = comple;
      }
      if(comple>worstcomplex){
        windex=j;
        worstcomplex = comple;
      }
    }
    std::cout<<"Chose plan "<<bindex<<" as best for graphlet "<<i<<std::endl;
    mrp.add_ex_plan(plans[bindex],i);
    std::cout<<"Chose plan "<<windex<<" as worst for graphlet "<<i<<std::endl;
    mrw.add_ex_plan(plans[windex],i);
    /*
       int currp = 0;
       for(ExecutionPlan p:plans){
       std::ostringstream filename;
       filename<<"../plans/plan_s"<<graphlet_size<<"_t"<<i<<"_p"<<currp<<".hpp";
       std::ofstream planfile(filename.str());
       planfile << p.toCode()<<std::endl;
       planfile.close();
    //std::cout<<p.toCode()<<std::endl;
    ++currp;
    //only make one, to make things easier
    break;
    }
    */
    
  }
  //Don't worry about combining them for now
  //auto plan = hp.greedy();
  //TimeStamp t4;
  {
    std::ostringstream fname;
    fname<<"../plans/" IFGPU(<< "gpu_") << "plan_" << plan_name <<"_best.hpp";
    std::cout << "Writing best plan to " << fname.str() << "\n";
    std::ofstream planfile(fname.str());
    std::string header, code;
    std::tie(header, code) = mrp.to_code();
    planfile << code;
    planfile.close();
    IFGPU(
      fname << ".h";
      std::ofstream headfile(fname.str());
      headfile << header;
      headfile.close();
    );
  }
  {
    std::ostringstream fname;
    fname<<"../plans/" IFGPU(<< "gpu_") << "plan_" << plan_name <<"_worst.hpp";
    std::cout << "Writing worst plan to " << fname.str() << "\n";
    std::ofstream planfile(fname.str());
    std::string header, code;
    std::tie(header, code) = mrw.to_code();
    planfile << code;
    planfile.close();
    IFGPU(
      fname << ".h";
      std::ofstream headfile(fname.str());
      headfile << header;
      headfile.close();
    );
  }
  /*for(int p0=0;p0<2;++p0)
    for(int p1=0;p1<4;++p1)
      for(int p2=0;p2<7;++p2)
	for(int p3=0;p3<2;++p3)
	  for(int p4=0;p4<5;++p4){
	    MultiRestPlan m(0);
	    m.add_ex_plan(planss[0][p0],0);
	    m.add_ex_plan(planss[1][p1],1);
	    m.add_ex_plan(planss[2][p2],2);
	    m.add_ex_plan(planss[3][p3],3);
	    m.add_ex_plan(planss[4][p4],4);
	    m.add_ex_plan(planss[5][0],5);
	    
	      std::ostringstream faname;
	      faname<<"../plans/plan_mr_motif_4_"<<p0<<p1<<p2<<p3<<p4<<".hpp";
	      std::ofstream panfile(faname.str());
	      panfile << m.to_code().second;
	      panfile.close();
	    
	  }*/
  
  //std::cout << graphlet_size << "\t" << t2-t1 << "\n";
  //printf("%d\t%f\t%f\t%f\n", graphlet_size, t2-t1, t3-t2, t4-t3);
  return 0;
}

