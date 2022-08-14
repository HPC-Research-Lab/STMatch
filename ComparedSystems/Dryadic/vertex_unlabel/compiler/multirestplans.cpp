#include "def.h"
#include <iostream>
MultiRestPlan::MultiRestPlan(int dep) :depth(dep){
  //everything else should be initiated automatically
}
MultiRestPlan::~MultiRestPlan(){
  //delete children
  for(auto smsm: children){
    delete smsm.second;
  }
}
void MultiRestPlan::add_ex_plan(ExecutionPlan& ep, int id){
  RestPlan rp(ep,id);
//   std::cout << rp;
  add_rest_plan(rp);
}

void MultiRestPlan::add_rest_plan(RestPlan& rp){
  plans.push_back(rp);
  // DO what is needed to be done
  MultiRestPlan* curr = this;
  int dep = 0;
  for(;dep<rp.loopons.size()-1;++dep){
//     std::cout << "at level: " << dep << " inserts the following restsets:\n";
//     for(auto rs : rp.depends[dep])
//       std::cout << rs << "\n";
    curr->atlev.insert(rp.depends[dep].begin(),rp.depends[dep].end());
//     std::cout << "at level: " << dep << " after inserts the restsets, atlev is:\n";
//     for(auto rs : curr->atlev)
//       std::cout << rs << "\n";
    RestSet lopon = rp.loopons[dep];
    if(curr->children.find(lopon)==curr->children.end()){
      curr->children[lopon] = new MultiRestPlan(dep+1); 
//       std::cout << lopon;
//       std::cout << "\ncreating a new multi rest plan\n";
    }
    curr = curr->children[lopon];
  }
  //std::cout<<rp.depends.size()<<" "<<rp.loopons.size()<<" "<<dep<<std::endl;
  // at the last level, we add a counter
  /*for(RestSet s : rp.depends[dep]){
    curr->atlev.insert(s);
    }*/

  if(curr->counters.find(rp.loopons[dep])==curr->counters.end()){
    curr->counters[rp.loopons[dep]] = npls;
  }
  else{
    std::cout<< "DUPLICATE PLANS ENCOUNTERED"<<std::endl;
  }
  ++npls;
}

std::pair<std::string, std::string> MultiRestPlan::to_code(){
  std::ostringstream oss1, oss2;
  append_to_stream(oss1, oss2);
  return { oss1.str(), oss2.str() };
}

void MultiRestPlan::append_to_stream(std::ostream &oss_head, std::ostream &oss){
  int fordepth = 0;
  int dynsize = 1;
  if(depth == 0) {
    NOGPU(
      oss << "const int n_counters = " << plans.size() << ";\n";
      oss << "std::vector<std::vector<uint64_t>> global_counters(omp_get_max_threads());\n";
      //oss << "  uint64_t counter[" << plans.size() << "];\n";
      //oss << "  for(int i = 0; i < " << plans.size() << "; i++) counter[i] = 0;\n";
      oss << "const vidType vertices = g.V();\n";
      oss << "#pragma omp parallel\n{\n";
      //oss << " reduction(+:counter[0:" << plans.size() << "])";
      //oss << "\n{\n";
      oss << "  std::vector<uint64_t> &counter = global_counters.at(omp_get_thread_num());\n";
      oss << "  counter.resize(n_counters, 0);\n";
      if(fordepth == depth) oss << "  #pragma omp for schedule(dynamic,"<<dynsize<<") nowait\n";
      oss << "  for(vidType v0 = 0; v0 < vertices; v0++) {\n";
    );
    IFGPU(oss_head << "constexpr int n_counters = " << plans.size() << ";\n");
  }
  //print all the sets we use
  std::ostringstream tabdepthoss;
  tabdepthoss<<"    ";
  for(int i=0;i<depth;++i) tabdepthoss<<"  ";
  std::string tabdep = tabdepthoss.str();

  oss<<tabdep;
  bool first = true;
  for(auto restset : atlev ){
    if(first) first = false;
    else oss<<tabdep;
    restset.append_calc_to_stream(oss,-1,atlev);
  }
  //update all our counters
  first = true;
  for(auto counterpair : counters){
    if(first) first = false;
    else oss<<tabdep;
    //NOGPU(oss<<"counter["<<counterpair.second<<"] += ");
    IFGPU(oss << "uint64_t local_counter_" << counterpair.second << " = ");
    counterpair.first.append_calc_to_stream(oss,counterpair.second,atlev);
    IFGPU(oss << tabdep << "if(work_group.thread_rank() == 0) atomicAdd(counter + " << counterpair.second << ", local_counter_" << counterpair.second << ");\n");
  }
  //go through all the loops
  for(auto m : children ){
    if(fordepth == depth+1)oss<<tabdep <<"#pragma omp for schedule(dynamic,"<<dynsize<<") nowait\n";
    oss<<tabdep;
    oss<<"for(vidType idx"<<(depth+1)<<" = 0; idx" << (depth+1) << " < " << m.first.varname << ".size(); idx" << (depth+1) << "++) {\n";
    NOGPU(oss << tabdep << "  vidType v" << (depth+1) << " = " << m.first.varname << ".begin()[idx" << (depth+1) << "];\n");
    NOGPU(oss << tabdep << "  VertexSet y" << (depth+1) << " = g.N(v" << (depth+1) << ");\n");
    m.second->append_to_stream(oss_head, oss);
    
    oss<<tabdep<<"}\n";
  }
  
  //  if(fordepth == depth+1)oss<<tabdep<<"#pragma omp barrier\n";
  if(depth == 0){
    NOGPU(
      oss << "  }\n}\n";
      //oss << "for(int i=0;i<"<<plans.size()<<";++i){\n"
      //    <<"  std::cout<<\"result_\"<<i<<\" = \" << counter[i] << \"\\n\";\n}\n";
      //oss << "  std::cout<<\"Expected Data Complexity was \"<<"<<data_complexity()<<"<<\"\\n\";\n";
      //oss << "  std::cout<<\"Expected Time Complexity was \"<<"<<time_complexity()<<"<<\"\\n\";\n";
      oss << "const uint64_t data_complexity = " << data_complexity() << ";\n";
      oss << "const uint64_t time_complexity = " << time_complexity() << ";\n";
    );
  }
}

double MultiRestPlan::data_complexity(){
  std::vector<int> temp;
  std::vector<int> rest;
  rest.push_back(-1);
  temp.push_back(0);
  return rec_data_complexity(GLOBAL_VERTEX_COUNT,rest,temp);
}

double MultiRestPlan::rec_data_complexity(double loopedness,std::vector<int> &restrictions,std::vector<int> &multcounts){
  //std::cout<<"loopy"<<loopedness<<std::endl;
  double res = 0;
  double comploop = loopedness;
  {
    int temp=depth;
    while(temp!=-1){
      ++multcounts.at(temp);
      temp=restrictions[temp];
    }
  }
  for(int i: multcounts){
    comploop/=i;
  }
  for(RestSet s: atlev){
    //these would be time complexity if we are calculating time complexity
    res+=comploop*s.data_complexity_ignoring_restrictions();
  }
  //check for counts not calculated at the level
  for(auto m:counters){
    if(atlev.count(m.first)==0){
      res+=comploop*m.first.data_complexity_ignoring_restrictions();
    }
  }
  //pair
  for(auto m: children){
    if(atlev.count(m.first)==0){
      res+=comploop*m.first.data_complexity_ignoring_restrictions();
    }
    restrictions.push_back(m.first.restriction());
    multcounts.push_back(0);
    res+=m.second->rec_data_complexity(loopedness*m.first.data_complexity_ignoring_restrictions(),restrictions,multcounts);
    multcounts.pop_back();
    restrictions.pop_back();
  }
  {
    int temp=restrictions.back();
    while(temp!=-1){
      --multcounts.at(temp);
      temp=restrictions[temp];
    }
  }
  return res;
}

double MultiRestPlan::time_complexity(){
  std::vector<int> temp;
  std::vector<int> rest;
  rest.push_back(-1);
  temp.push_back(0);
  return rec_time_complexity(GLOBAL_VERTEX_COUNT,rest,temp);
}

double MultiRestPlan::rec_time_complexity(double loopedness,std::vector<int> &restrictions,std::vector<int> &multcounts){
  //std::cout<<"loopy"<<loopedness<<std::endl;
  double res = 0;
  double comploop = loopedness;
  //add the multiplicity calculation.
  {
    int temp=depth;
    while(temp!=-1){
      ++multcounts.at(temp);
      temp=restrictions[temp];
    }
  }
  //this is how many would be achieved
  for(int i: multcounts){
    comploop/=i;
  }
  for(RestSet s: atlev){
    //these would be time complexity if we are calculating time complexity
    res+=comploop*s.time_complexity_ignoring_restrictions(false,atlev);
  }
  
  for(auto m:counters){
    if(atlev.count(m.first)==0){
      res+=comploop*m.first.time_complexity_ignoring_restrictions(true,atlev);
    }
  }
  //pair
  for(auto m: children){
    if(atlev.count(m.first)==0&&counters.count(m.first)==0){
      res+=comploop*m.first.time_complexity_ignoring_restrictions(true,atlev);
    }
    //update restrictions to be processed by the child
    restrictions.push_back(m.first.restriction());
    multcounts.push_back(0);
    res+=m.second->rec_time_complexity(loopedness*m.first.data_complexity_ignoring_restrictions(),restrictions,multcounts);
    multcounts.pop_back();
    restrictions.pop_back();
    //undo the restrictions
  }
  {
    int temp=restrictions.back();
    while(temp!=-1){
      --multcounts.at(temp);
      temp=restrictions[temp];
    }
  }
  //return restrictions to how it was originally
  return res;
}

