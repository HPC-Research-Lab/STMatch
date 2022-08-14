#include "def.h"
#include<iostream>

//restrictions should be the restrction vector of an execution plan
RestSet::RestSet(const std::vector<int>& i, const std::vector<int>& o, const std::vector<int>& restrictions) : ins(i), out(o) {
  depth =0;
  if(i.size()>0) depth = std::max(depth, i[i.size()-1]);
  if(o.size()>0) depth = std::max(depth, o[o.size()-1]);
  //std::copy(restrictions.begin(), restrictions.begin()+depth+2, back_inserter(restrict));
  restrict = restrictions;
  res_chain = std::vector<int>(depth+1, -1);
  int t = depth+1;
  while(restrictions[t] != -1) {
//     std::cerr << "t: " << t << std::endl;
    res_chain[t-1] = restrictions[t];
//     std::cerr << "res_chain[t-1]: " << res_chain[t-1] << std::endl;
    t = res_chain[t-1];
    if(t <= 0) break;
  }
//   std::cout << "res_chain: (";
//   std::copy(res_chain.begin(), res_chain.end(), std::ostream_iterator<int>(std::cout, " "));
//   std::cout << ")\n";
  varname = var_name();
}

bool RestSet::operator<(const RestSet& other) const{

  //ins size, outs size, ins values, outs values, restriction.
  if(other.ins.size() < ins.size())return true;
  if(other.ins.size() > ins.size())return false;
  if(other.out.size() < out.size())return true;
  if(other.out.size() > out.size())return false;
  if(other.res_chain.size() < res_chain.size()) return true;
  if(other.res_chain.size() > res_chain.size()) return false;

  for(int i=0;i<ins.size();++i){
    if(other.ins.at(i) < ins.at(i)) return true;
    if(other.ins.at(i) > ins.at(i)) return false;
  }
  for(int i=0;i<out.size();++i){
    if(other.out.at(i) < out.at(i)) return true;
    if(other.out.at(i) > out.at(i)) return false;
  }
  for(int i=0;i<res_chain.size();++i) {
    if(res_chain.at(i) < other.res_chain.at(i)) return true;
    if(res_chain.at(i) > other.res_chain.at(i)) return false;
  }
  return false; 
}

std::string RestSet::var_name(){
  
  std::ostringstream oss;
  int ini =0;
  int oui =0;
  int i=0;
  for(i=0;i<ins.size()+out.size();++i){
    if(oui<out.size() &&out[oui]==i){
      oss<<"n"<<i;
      ++oui;
    }
    else{
      oss<<"y"<<i;
      ++ini;
    }
    if(res_chain[i] >= 0) {
      oss<<"f"<<res_chain[i];
    }
  }
  return oss.str();
}

int RestSet::restriction() const{
  return res_chain[depth];
}

//adjusting this function can change how performance ends up going.
RestSet RestSet::parent() const{

  RestSet parent = *this;
  if(!parent.tranResChain){
    parent.tranResChain = true;
    std::vector<int> t(parent.res_chain.size(), -1);
    for(auto r : parent.res_chain)
      if(r!= -1)
        t[r] = r;
    parent.res_chain = t;
  }

  //std::cout<<"CALLING PARENT FOR "<<varname<<std::endl;
  //highest variable value remaining
  int lastvar = ins[ins.size()-1];//
  if(out.size()>0)lastvar = std::max(lastvar,out[out.size()-1]);
  if(ins[ins.size()-1] == lastvar){
    parent.ins.pop_back();
    parent.res_chain.pop_back();
    parent.depth--;
    parent.varname = parent.var_name();
    return parent;
  }
  else {
    parent.out.pop_back();
    parent.res_chain.pop_back();
    parent.depth--;
    parent.varname = parent.var_name();
    return parent;
  }
}

bool RestSet::valid(){
  return ins.size()>0;
}


//available is what is available at that level already
void RestSet::append_calc_to_stream(std::ostream& oss,int index,std::set<RestSet> &available) const{
  static int transient_id = 100;
  RestSet par = parent();
  //not sure whether this will be executed in the new version
  if(-1 == index && res_chain[depth]!=-1){
    std::vector<int> rs(restrict);
    RestSet testpar(ins,out,rs);
    testpar.res_chain[depth] = -1;
    testpar.varname = testpar.var_name();
    if(available.count(testpar)){
      oss<< "VertexSet " << varname << " = bounded("<<testpar.varname<<",v"<<res_chain[depth]<<");\n";
      return;
    }
  }
  //std::cout<<varname<<" is a child of "<<par.varname<<std::endl;
  if(par.ins.size() == 0) { // has transient unnamed intermediate sets
    //bounded for 0
    if(par.out.size()==0){ 
      //oss << "VertexSet " << varname << " = ";
      if(res_chain[depth] ==0) {
        oss << "VertexSet y0 = g.N(v0); ";
        oss<< "VertexSet " << varname << " = bounded(y0,v0);\n";
      }
      else {
        oss << "VertexSet y0 = g.N(v0);\n";
      }
      return;
    }
    //we can't use the parent, it doesn't exist
    //just compute ourselves
    if(varname == "n0y1") {
      std::cerr << "strange res_chain: ";
      std::copy(res_chain.begin(), res_chain.end(), std::ostream_iterator<int>(std::cerr, " "));
      std::cerr << std::endl;
    }
      
    if(out.size() != 0)
      oss<<"VertexSet "<<varname<< "; ";
    if(index>=0)
      oss<<"counter["<<index<<"] += ";
    for(int i=0;i<out.size();++i) {
      if(index != -1 && i==0) {
        oss << "difference_num(";
      }
      else {
        oss << "difference_set(" << varname << ",";
        if(varname == "n0y1")
          std::cerr << "difference_set(" << varname << ",";
      }
    }
    oss<< "y" << ins[0];
    if(varname == "n0y1")
      std::cerr << "y" << ins[0];
    for(int i=0;i<out.size();++i){
      oss<<", y"<<out[i]<<"";
      if(varname == "n0y1")
        std::cerr << ", y" << out[i]<<"";
      if(i==0 && res_chain[depth]!=-1) {
        oss<<", v"<<res_chain[depth];
        if(varname == "n0y1")
          std::cerr << ", v" << out[i]<<"";
      }
      IFGPU(oss << ", local_indices, local_search, &manager, work_group");
      oss<<")";
        if(varname == "n0y1")
          std::cerr << ")\n";
    }
    if(index!=-1 && out.size()==0)oss<<".size()";
    oss<<";\n";
    return;
  }

  if(index== -1) {
    oss << "VertexSet " << varname << " = ";
  } else {
    oss<<"counter["<<index<<"] += ";
  }
  //this set is a difference
  if(out.size()!=par.out.size()){
    oss<< "difference_"<< (index!=-1 ? "num(" : "set(")
       << par.varname << ", y" << out[out.size()-1];
    if(res_chain[depth]!=-1){
      oss<<", v"<<res_chain[depth]; 
    }
    IFGPU(oss << ", local_indices, local_search, &manager, work_group");
    oss<<");\n";
    return;
  }
  //this set is an intersection
  if(ins.size()!=par.ins.size()){
    oss<< "intersection_" << (index!=-1?"num(":"set(")
       <<par.varname<<", y"<<ins[ins.size()-1];
    if(res_chain[depth]!=-1){
      oss<<", v"<<res_chain[depth]; 
    }
    IFGPU(oss << ", local_indices, local_search, &manager, work_group");
    oss<<");\n";
    return;
  }  
}

double RestSet::data_complexity_ignoring_restrictions() const{
  std::set<int> inset(ins.begin(),ins.end());
  std::set<int> outset(out.begin(),out.end());
  std::pair<std::set<int>,std::set<int>> dar(inset,outset);
  double xp = expected_size(dar);
  if(std::isnan(xp)){
    std::cout<<"Nan from size" <<ins.size()<<" "<<out.size()<<" "<<res_chain[depth]<<std::endl;
  }
  return expected_size(dar);
}

double RestSet::time_complexity_ignoring_restrictions() const {
  std::set<RestSet> emp;
  return time_complexity_ignoring_restrictions(false,emp);
}
double RestSet::time_complexity_ignoring_restrictions(bool numb,std::set<RestSet> &available) const{
  /*std::set<int> inset(ins.begin(),ins.end());
  std::set<int> outset(out.begin(),out.end());
  std::pair<std::set<int>,std::set<int>> dar(inset,outset);
  */
  if(!numb && res_chain[depth]!=-1){
    std::vector<int> rs(restrict);
    rs[depth] = -1;
    RestSet testpar(ins,out,rs);
    if(available.count(testpar)==1){
      //basically nothing, we just do it bounded
      return 0;//std::min(64,restset.data_complexity_ignoring_restrictions());
    }
  }
  RestSet par = parent();
  if(par.ins.size()==0){
    //we compute here.
    //global average degree time 1 + expected size one intersect + expected size 2+ ... expected size num outs -1
    // = (global average degree - expected size numouts intersect)/(1-(1-p))
    double p = (double)GLOBAL_AVERAGE_DEGREE/(double)GLOBAL_VERTEX_COUNT;
    return GLOBAL_AVERAGE_DEGREE*(1-pow(1-p, out.size()))/p; 
  }
  
  //  std::cout<<data_complexity_ignoring_restrictions();
  return parent().data_complexity_ignoring_restrictions();
}

