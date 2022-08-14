#include "def.h"
#include <iostream>

RestPlan::RestPlan(ExecutionPlan& ep, int _id): id(_id), vertices(ep.vertices), rest(ep.restrictions) {
  //construct the plans at each level for what we loop on
  //first element on loopon is the set for v1
  //first construct what they depend on, put them in 
  //This loop implements single-plan set operation motion
  for(int i= 1; i < ep.vertices;++i){
    std::vector<int> ins(ep.depend[i].first.begin(),ep.depend[i].first.end());
    std::vector<int> out(ep.depend[i].second.begin(),ep.depend[i].second.end());
//     std::vector<int> rs(ep.restrictions.begin()+1, ep.restrictions.end());
//     loopons.emplace_back(ins,out,rs);
    loopons.emplace_back(ins,out,ep.restrictions);
    depends.emplace_back();
    //don't include the last one
    //    std::cout<<i<<" "<<ep.vertices<<std::endl;
    if(i!=ep.vertices-1) depends[i-1].insert(loopons[i-1]);
    
    RestSet curr = loopons[i-1].parent();
    while(curr.ins.size()>0){
      depends[curr.depth].insert(curr);
      curr = curr.parent();
    }
    //    std::cout<<depends[i-1].size()<<std::endl;
  }
}


//expected memory allocated to vertex sets
double RestPlan::data_complexity(){
  //go through depends and loopons, iterating down
  std::vector<int> multcounts(vertices);
  //for(int i=0;i<vertices;++i)multcounts[0] = 0;
  double res=0;

  //expected times a loop happens is
  // (normal expected times) / product (multcounts)
  double expectedloopness = GLOBAL_VERTEX_COUNT;
  for(int i=0;i<vertices-1;++i){
    //go down on loopon
    int mc = i;
    while(mc!=-1){
      ++multcounts[mc];
      mc=rest[mc];
    }
    double inexpect = expectedloopness;
    for(int j=0;j<=i;++j) inexpect/=multcounts[j];
    for(RestSet s: depends[i]){
      res+=s.data_complexity_ignoring_restrictions()*inexpect;
    }
    expectedloopness*=loopons[i].data_complexity_ignoring_restrictions();
  }
  //delete[] multcounts;
  //std::cout<<res<<std::endl;
  return res;
}

//expected time to compute
double RestPlan::time_complexity(){
  //go through depends and loopons, iterating down
  std::vector<int> multcounts(vertices);
  //int* multcounts = new int[vertices];
  for(int i=0;i<vertices;++i)multcounts[i] = 0;
  double res=0;
  
  //expected times a loop happens is
  // (normal expected times) / product (multcounts)
  double expectedloopness = GLOBAL_VERTEX_COUNT;
  for(int i=0;i<vertices-1;++i){
    //go down on loopon
    int mc = i;
    while(mc!=-1){
      ++multcounts[mc];
      mc=rest[mc];
    }
    double inexpect = expectedloopness;
    for(int j=0;j<=i;++j)
      {
	//std::cout<<j<<"has mult "<<multcounts[j]<<" at step "<<i<<std::endl;
	inexpect/=multcounts[j];
      }
    double oldres=res;
    for(RestSet s: depends[i]){
      res+=s.time_complexity_ignoring_restrictions()*inexpect;
      
    }
    if(i==vertices-2){
      //std::cout<<" Adding complexity of the loop "<<std::endl;
      res+=loopons[i].time_complexity_ignoring_restrictions()*inexpect;
    }
    //    std::cout<<i<<" makes " <<oldres<<" res "<<res<<" "<<res-oldres<<std::endl; 
    expectedloopness*=loopons[i].data_complexity_ignoring_restrictions();
    
  }

  return res;
}
