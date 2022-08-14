#include "def.h"
#include <iostream>
#include <iterator>
MultiRestPlan::MultiRestPlan(int dep,bool l) :depth(dep),labelled(l),labels(){
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
  
//   st/d::cout << rp;
  add_rest_plan(rp);
}

void MultiRestPlan::add_rest_plan(RestPlan& rp){
  plans.push_back(rp);
  MultiRestPlan* curr = this;
  int dep = 0;
  for(;dep<rp.loopons.size()-1;++dep){
    std::cout<<"putting into "<<dep<<"\n";
    std::copy(rp.depends[dep].begin(),rp.depends[dep].end(),std::ostream_iterator<RestSet>(std::cout, " "));
    
    //add all the dependencies, atlev is a set so it doesn't matter if there are repeats
    curr->atlev.insert(rp.depends[dep].begin(),rp.depends[dep].end());
    RestSet lopon = rp.loopons[dep];
    //make the child if it doesn't exist
    
    if(curr->children.find(lopon)==curr->children.end()){
      curr->children[lopon] = new MultiRestPlan(dep+1,labelled); 
    }
    //progress to child
    curr = curr->children[lopon];

  }
  //add counter, and atlevs
  curr->atlev.insert(rp.depends[dep].begin(),rp.depends[dep].end());
  if(curr->counters.find(rp.loopons[dep])==curr->counters.end()){
    curr->counters[rp.loopons[dep]] = npls;
  }
  else{
    std::cout<< "DUPLICATE PLANS ENCOUNTERED"<<std::endl;
  }   
  npls = std::max(npls,1+rp.id);
}

std::pair<std::string, std::string> MultiRestPlan::to_code(int mpdepth, int stdepth,int mode){
  std::ostringstream oss1, oss2;
  append_to_stream(oss1, oss2,mpdepth,stdepth,mode);
  return { oss1.str(), oss2.str() };
}

void MultiRestPlan::append_to_stream(std::ostream &oss_head, std::ostream &oss,int fordepth, int start,int mode){
  //std:/:cout<<labelled<<" "<<depth<<std::endl;
  bool justcounting = (LIST_FLAG & mode) != LIST_FLAG;
  bool batching = (mode&BATCH_FLAG) == BATCH_FLAG;
  bool streaming = (mode&STREAM_FLAG) == STREAM_FLAG;
  if(batching)justcounting = false;
  std::cout<<streaming<<" streaming "<<depth<<" "<<depth<<" "<<(batching||streaming)<<std::endl;
  int dynsize = 64;
  if(depth == 0) {
    //NOGPU(
    oss << "const int n_counters = " << npls << ";\n";
    oss << "std::vector<std::vector<uint64_t>> global_counters(omp_get_max_threads());\n";
    oss << "const vidType vertices = g.V();\n";
    oss << "#pragma omp parallel\n{\n";
    oss << "  std::vector<uint64_t> &counter = global_counters.at(omp_get_thread_num());\n";
    oss << "  counter.resize(n_counters, 0);\n";
    
    //if(depth == 1) oss << "  #pragma omp for schedule(dynamic,"<<dynsize<<") nowait\n";
    //oss << "  for(vidType v0 = 0; v0 < vertices; v0++) {\n";
  }
  //print all the sets we use
  std::ostringstream tabdepthoss;
  tabdepthoss<<"    ";
  for(int i=0;i<depth;++i) tabdepthoss<<"  ";
  std::string tabdep = tabdepthoss.str();
  for(auto restset : atlev){
    oss<<tabdep;
    restset.append_calc_to_stream(oss,-1,atlev);
  }
  //update all our counters
  for(auto counterpair : counters){
    oss<<tabdep;
    counterpair.first.append_calc_to_stream(oss,counterpair.second,atlev);  
  }
  //go through all the loops
  for(auto child : children ){
    if(depth==0)oss<<tabdep <<"#pragma omp for schedule(dynamic,"<<dynsize<<") nowait\n";
    oss<<tabdep;
    child.first.append_iter_to_stream(oss,depth);
    oss<<tabdep<<"{\n";
    child.second->append_to_stream(oss_head,oss,fordepth,start,mode);
    oss<<tabdep<<"}\n";
  }
  //  if(fordepth == depth+1)oss<<tabdep<<"#pragma omp barrier\n";
  if(depth == 0){
    oss << "}//217\n";//pragma omp parallel
    oss << "const uint64_t data_complexity = " << data_complexity() << ";\n";
    oss << "const uint64_t time_complexity = " << time_complexity() << ";\n";
    
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
  
  for(auto d:counters){
    //for(auto m:d.second){
    if(atlev.count(d.first)==0){
      res+=comploop*d.first.time_complexity_ignoring_restrictions(true,atlev);
    }
    //}
  }
  //pair
  for(auto d: children){
    
    if(atlev.count(d.first)==0&&counters.count(d.first)==0){
      res+=comploop*d.first.time_complexity_ignoring_restrictions(true,atlev);
    }
    //update restrictions to be processed by the child
    restrictions.push_back(d.first.restriction());
    multcounts.push_back(0);
    
    res+=d.second->rec_time_complexity(loopedness*d.first.data_complexity_ignoring_restrictions(),restrictions,multcounts);
    
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

