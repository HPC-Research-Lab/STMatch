#include "def.h"
#include <iostream>
/**
 * Inserts the reduced execution plan into the vector
 * Should be a way to have the automorphism already integrated at this point,
 */
void MultiRed::reduced(std::vector<int> &ord,std::vector<int> &restrictions){
  /*DirectedGraphlet d = dg.reorder(d);
    for(int i=0;i<n;++i){
  //reorder dg
  }*/
  /*
     for(int x : ord)
     std::cout<<x<<",";
     std::cout<<std::endl;

     std::cout<<std::endl;
     */  
  //apply ord onto restrictions
  DirectedGraphlet dg(n);
  //add a bunch of edges
  std::vector<int> applied_restrictions(n,-1);
  for(int i=0;i<n;++i){
    for(int j=0;j<n;++j){
      if(i==j)continue;
      //restriction application
      if(ord[i]==restrictions[ord[j]]){
        applied_restrictions[j] = i;
        assert(i<j);
        //std::cout<<"added restriction "<< j << " "<<i<<std::endl;
      }
      //edges
      if(i<j){
        if(g.adjacency[ord[i]].find(ord[j])!=g.adjacency[ord[i]].end()){
          dg.add_edge(j,i,true);
          //std::cout<<"added edge "<<i<<" "<<j<<std::endl;	  
        }
        else {
          dg.add_edge(j,i,false);
        }
      }
    }
  }
  //dg.generate_complement();
  ExecutionPlan ex(dg,applied_restrictions);
  //print ordering and restrictions for now
  plans->push_back(ex);
}
/**
 * restrictions is based on our g, used for space reduction 
 * can be translated into restrictions for output
 * 
 */
void MultiRed::process_all(std::vector<int> &restrictions,
			   std::vector<std::vector<int>> &remaining_auto,
			   std::vector<int> &partial,
			   std::set<int> &used, std::set<int> &remaining){
  /*
    std::cout<<"partial";
    for(int i:partial)std::cout<<","<<i;
    std::cout<<std::endl;
    std::cout<<"remaining";
    for(int i:remaining)std::cout<<","<<i;
    std::cout<<std::endl;
    std::cout<<"remauto: "<<remaining_auto.size()<<" elements";
    std::cout<<std::endl;
  */
  if(remaining.size()==0 && partial.size() == n){
    //we've processed, add to the plans
    reduced(partial,restrictions);
    return;
  }
  //which ones we have already processed;
  bool* remcare = new bool[n];
  for(int i=0;i<n;++i)remcare[i] = false;
  //go in increasing order
  for(int x : remaining){
    //already processed
    if(remcare[x])continue;
    std::vector<int> rests(restrictions);
    std::vector<std::vector<int>> rem_auts;
    //generate equivalence class
    for(std::vector<int> aut : remaining_auto){
      //member of the class, mark it as taken care of
      remcare[aut[x]] = true;
      //stabilizer group
      if(aut[x]==x) rem_auts.push_back(aut);
      //restriction
      else rests[aut[x]] = x;
    }
    std::set<int> nused(used);
    std::set<int> rem(remaining);
    std::vector<int> part(partial);
    part.push_back(x);
    nused.insert(x);
    rem.erase(x);
    //add neighbors to remaining
    for(int j:g.adjacency.at(x)){
      //if it hasn't already been processed, add it to the list
      if(used.find(j)==used.end()){
        rem.insert(j);
      }
    }
    //recurse
    process_all(rests,rem_auts,part,nused,rem);
  }
  delete[] remcare;
}

/**
 */
void MultiRed::autos(std::vector<int> &partial,
		     std::set<int> &remaining){
  int curr = partial.size();
  //printf("E1 %d %d %d\n",partial.size(),remaining.size(),*remaining.begin());
  if(remaining.size()==0 && partial.size() == n){
    automorphisms.push_back(partial);
    return;
  }
  //go in increasing order
  std::set<int> remcop(remaining);
  //for all candidates for the next
  for(int x : remaining){
    //check if mapping curr to x is a valid plan
    //equal degree
    if(g.adjacency.at(x).size()!=g.adjacency.at(curr).size())continue;
    //only works for undirected right now
    bool bad = false;
    //must have the correct edges to ancestors
    for(int y : g.adjacency.at(curr)){
      if(y<curr){//y has been mapped already
	//the permuted y and x must be adjacent
	//or x would not be a valid next candidate
	if(g.adjacency.at(x).find(partial[y])==g.adjacency.at(x).end()){
	  bad=true;
	  break;
	}
      }
    }
    if(bad) continue;
    //if x is valid as a next
    partial.push_back(x);
    remcop.erase(x);
    autos(partial,remcop);
    partial.pop_back();
    remcop.insert(x);
  }

}

//This generates all distinct restricted plans
MultiRed::MultiRed(Graphlet& d, std::vector<ExecutionPlan>& p): g(d),n(d.vertices),plans(&p){
  //make the automorphisms
  std::set<int> autorem;
  for(int i=0;i<n;++i)autorem.insert(i);
  std::vector<int> init;
  autos(init,autorem);
  //printf("entered \n");
  //use the automorphisms
  bool* cared = new bool[n];
  for(int i=0;i<n;++i)cared[i] = false;
  for(int i=0;i<n;++i){
    //already processed under another thing
    if(cared[i])continue;
    
    std::vector<int> rests (n,-1);
    std::vector<std::vector<int>> rem_auts;
    for(std::vector<int> aut : automorphisms){
      cared[aut[i]] = true;
      if(aut[i]==i)rem_auts.push_back(aut);
      else rests[aut[i]] = i; 
    }
    std::set<int> used;
    std::set<int> rem;
    std::vector<int> partial;
    partial.push_back(i);
    used.insert(i);
    //have to fill rem with connected to rem, in dg.
    for(int j:g.adjacency.at(i)){
      rem.insert(j);
    }
    // did not have time to process without recursion,
    // is probably inefficient, who knows.
    process_all(rests,rem_auts,partial,used,rem);
  }
  delete[] cared;
}

/*
void multiplicity_reduced_graph(DirectedGraphlet& dg,std::vector<ExecutionPlan>& planlist){
  //find automorphisms
  int n = dg.vertices;
  std::vector<std::vector<int>> automorphisms;
  //start with an initial partial permutation
  list_automorphisms(automorphisms);
  //see if the partial is ok
  //add stuff
  //schedule reduction
  

  
  //apply all permutations - already permutation reduced
  

}
*/
