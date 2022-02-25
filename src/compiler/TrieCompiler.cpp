#include <cassert>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <queue>
#include "TrieCompiler.h"

namespace libra{




TrieCompiler::TrieCompiler(){}
TrieCompiler::~TrieCompiler(){
    _clear_trie(root_);
}

void TrieCompiler::_clear_trie(TrieNode* _root){
    if(!_root) return;
    if(_root->left())  _clear_trie(_root->left());
    if(_root->right()) _clear_trie(_root->right());
    delete _root;
}

void TrieCompiler::_print_ops(){
    std::for_each(ops_.begin(), ops_.end(), [](auto& _op){
        std::for_each(_op.begin(), _op.end(), [](auto& _item){
            if(_item.first) std::cout<<"+";
            else std::cout<<"-";
            std::cout<<_item.second;
        });
        std::cout<<std::endl;
    });
}
void TrieCompiler::_print_trie(){
    std::queue<TrieNode*> q;
    q.push(root_);
    int level=0;
    while(!q.empty()){
        int width = q.size();
        
        for(int i=0; i<width; i++){
            TrieNode* a = q.front();
            q.pop();
            
            if(a->positive) printf("+%d ", a->val_);
            else printf("-%d ", a->val_);
            
            if(a->left()) q.push(a->left());
            if(a->right()) q.push(a->right());
        }
        printf("\n");
        level++;
    }
    printf("depth_:%lu, num_node_:%d\n", depth_, num_node_);
}
void TrieCompiler::print(){
    _print_ops();
    _print_trie();
}

void TrieCompiler::compile(StlPattern& _stl_pattern){
    _clear();
    stl_pattern_=&_stl_pattern;
    z_compiler_.compile(*stl_pattern_);
    _fill_ops();
    _build_trie();
}

void TrieCompiler::_fill_ops(){
    int num_vertex_ =  stl_pattern_->num_vertex();
    ops_.resize(num_vertex_);
    for(int level=1; level<num_vertex_; level++){
      for(int k=0; k<level; k++){
          if(stl_pattern_->connected(z_compiler_.vertex_at(k), z_compiler_.vertex_at(level))){
              ops_[level].push_back(std::make_pair(true, k));
          }
          else{
            ops_[level].push_back(std::make_pair(false, k));
          }
      }
    }
}

void TrieCompiler::_insert_helper(TrieNode* _cur, size_t _idx,  auto& _ops){
    if(_idx==_ops.size()) {
        depth_ = (_idx+1)>depth_?(_idx+1):depth_;
        return;
    }
    assert(std::abs(_ops[_idx].second)==_idx);
    assert(_cur!=nullptr);
    TrieNode** dummy; 

    if(_ops[_idx].first==false){
        dummy = &(_cur->left());
    }
    else{
        dummy = &(_cur->right());
    }
    if(!(*dummy)){
        (*dummy) = new TrieNode(_ops[_idx].first, _ops[_idx].second);
        num_node_++;
    }
    if(_idx==_ops.size()-1){
        (*dummy)->is_candidate_=true;
    }
    _insert_helper((*dummy), _idx+1, _ops);
}

void TrieCompiler::_insert(std::vector<std::pair<bool, int>> _ops){
    _insert_helper(root_, 0 , _ops);
}

void TrieCompiler::_build_trie(){
    root_ = new TrieNode(true, INT_MAX);
    root_->is_candidate_ = true;
    for(size_t i=0; i<ops_.size(); i++){
        this->_insert(ops_[i]);
    }
}

/*
void TrieCompiler::write_stk_info(CPattern& _stk_info){
    std::queue<TrieNode*> q;
    q.push(root_);
    int i=0;

    char last_slot = 0;
    char op_type = 0
    char special_case = 0;
    char dep_idx = 0;

    while(!q.empty()){
        int width = q.size();
        for(int j=0; j<width; j++){
            TrieNode* a = q.front();
            q.pop();

            if(j==width-1) last_slot = 0b10000000;
            else last_slot = 00000000;

            if(a->positive) op_type = 0b00100000;
            else op_type = 0b00000000;
            
            _fill_set_ops(_stk_info->set_ops[i][j], a);
            //set_op_t& set_ops = _stk_info->set_ops[i][j];

            if(a->left()) q.push(a->left());
            if(a->right()) q.push(a->right());
        }
        i++;
    }   
}
*/


/*
void TrieCompiler::_write_stk_info_helper(TrieNode* _cur,  
                                    int _level,
                                    std::vector<int>& _iter, 
                                    CPattern& _stk_info, 
                                    set_op_t _father_set_op){
    
    set_op_t& cur_set_ops = _stk_info.set_ops[_level][(int)_iter[_level]];
    cur_set_ops =0;

    if(_cur->is_candidate_) cddt_idx_[_level] = _iter[_level];

    if(_level>0){
        cur_set_ops|= ((char)_iter[_level-1] & 0b00001111);  //dep_;
        set_op_t father_op_type = _father_set_op & 0b01100000;
        if(father_op_type == 0b00000000){
            if(_cur->positive) cur_set_ops |=0b00100000;
            else cur_set_ops |=0b00000000;
        }
        else{
            if(_cur->positive) cur_set_ops |=0b01000000;
            else cur_set_ops |=0b01100000;
        }
    }
    if(_cur->left()){
        _write_stk_info_helper(_cur->left(), _level+1, _iter, _stk_info, cur_set_ops);
    }
    if(_cur->right()){
        _write_stk_info_helper(_cur->right(), _level+1, _iter, _stk_info, cur_set_ops);
    }
    _iter[_level]+=1;                     
}
*/

void TrieCompiler::_write_stk_info_helper(TrieNode* _cur,  
                                    int _level,
                                    std::vector<int>& _iter, 
                                    CPattern& _stk_info, 
                                    set_op_t _father_set_op){
    
    set_op_t& cur_set_ops = _stk_info.set_ops[_level][(int)_iter[_level]];
    cur_set_ops =0;

    if(_cur->is_candidate_) cddt_idx_[_level] = _iter[_level];

    if(_level==0){
        cur_set_ops = 0b01000000;
    }
    else if(_level==1){
        cur_set_ops|= ((char)_iter[_level-1] & 0b00001111);  //dep_;
       if(_cur->positive){
            cur_set_ops = 0b00100000;
       }
       else{
           cur_set_ops = 0b01000000;
       }
    }
    else if(_level>1){
        cur_set_ops|= ((char)_iter[_level-1] & 0b00001111);  //dep_;
        if( (_father_set_op & 0b01000000 ) == 0b01000000){
            if(_cur->positive) {
                cur_set_ops &= 0b10111111;  //strip omit
                cur_set_ops |=0b00010000;  //special case
            }
            else cur_set_ops |= 0b01000000;                //omit
        }
        else{
            if(_cur->positive) cur_set_ops |=0b00100000;  //intersection
            else cur_set_ops &=0b11011111;   //difference
        }
    }
    if(_cur->left()){
        _write_stk_info_helper(_cur->left(), _level+1, _iter, _stk_info, cur_set_ops);
    }
    if(_cur->right()){
        _write_stk_info_helper(_cur->right(), _level+1, _iter, _stk_info, cur_set_ops);
    }
    _iter[_level]+=1;                     
}

void TrieCompiler::write_stk_info(CPattern& _stk_info){
    cddt_idx_.resize(depth_, 0);
    level_size_.resize(depth_, 0);
    is_omit_.resize(depth_, 0);
    _write_stk_info_helper(root_, 0, level_size_, _stk_info,  0);   
    std::vector<std::vector<int>> reorder(depth_, std::vector<int>(depth_, 0));
    _remove_omit_op(reorder, _stk_info);
    _move_cddt_leftmost(reorder);
    _reorder_set_ops(reorder, _stk_info);
    //print_pattern(_stk_info);
    return;
}


void TrieCompiler::_remove_omit_op(auto& _reorder, CPattern& _stk_info){
    for(size_t i=1; i<depth_; i++){
        set_op_t& cur_set_ops = _stk_info.set_ops[i][0];
        if( (cur_set_ops & 0b01000000)== 0b01000000){
            for(int j=0; j<level_size_[i]-1; j++){
                _reorder[i][j] = j+1;
            }
            //cddt_idx_[i]-=1;
            level_size_[i]-=1;
            //assert(cddt_idx_[i]>=0);
        }
    }
}

void TrieCompiler::_move_cddt_leftmost(auto& _reorder){
    for(size_t i=1; i<depth_; i++){
        for(int j=1; _reorder[i][j] <=cddt_idx_[i]; j++){
            _reorder[i][j]=_reorder[i][j-1];
        }
        _reorder[i][0] = cddt_idx_[i];
    }
    /*
    for(int i=0; i<_reorder.size(); i++){
        for(int j=0; j<level_size_[i]; j++){
            printf("%d ", _reorder[i][j]);
        }
        printf("\n");
    }
    */
}

void TrieCompiler::_reorder_set_ops(auto& _reorder, CPattern& _stk_info){

    CPattern stk_info_tmp = _stk_info; 
    
    for(size_t i=1; i<depth_; i++){
        for(int j=0; j<level_size_[i]; j++){
            stk_info_tmp.set_ops[i][j] = _stk_info.set_ops[i][_reorder[i][j]];
            char dep = stk_info_tmp.set_ops[i][j] & 0b00001111;
            char revised_dep = 0;
            for(int k=0; k<level_size_[i-1]; k++){
                if(_reorder[i-1][k]==(int)dep){
                    revised_dep = (char)k;
                    break;
                }
            }
            stk_info_tmp.set_ops[i][j] &=0b11110000;
            stk_info_tmp.set_ops[i][j] |=revised_dep;
        }
    }

    for(size_t i=0; i<depth_; i++){
        stk_info_tmp.set_ops[i][level_size_[i]-1]|=0b10000000;
    }
    _stk_info = stk_info_tmp;
}

void TrieCompiler::print_pattern(CPattern& _p){
    #define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
    #define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0') 
    for(size_t i=0; i<depth_; i++){
        for(int j=0; j<level_size_[i]; j++){
            printf(" " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(_p.set_ops[i][j]));
        }
        printf("\n");
    }
}


/*
void TrieCompiler::_init_level_attr(){
    std::queue<TrieNode*> q;
    q.push(root_);
    int level=0;
    SlotAttr* slotPtr = stk_->slots_; //TODO
    while(!q.empty()){
        int width = q.size();
        LevelAttr& stk_level = stk_->at(level);
        stk_level.dataPtr = slotPtr;
        stk_level.width = width;
        for(int i=0; i<width; i++){
            TrieNode* a = q.front();
            q.pop();
            if(a->is_candidate_)
                stk_level.cddt_idx = i;
            if(a->left()) q.push(a->left());
            if(a->right()) q.push(a->right());
        }
        level++;
        slotPtr+=width;
    }   
}

void TrieCompiler::_init_slot_attr_helper(TrieNode* _cur,  
                                    int _level,
                                    std::vector<int>& _iter, 
                                    SlotAttr& father_slot, 
                                    size_t& cursor){
    
    SlotAttr& stk_slot = stk_->at(_level, _iter[_level]);

    
    stk_slot.dataPtr = stk_->dataPtr_+(cursor*stk_->slot_width_);
    //printf("dataPtr recur:%u, cur:%d, width:%d\n", stk_slot.dataPtr, cursor, stk_->slot_width_);
    cursor++;
    if(_level>0) stk_slot.dependency=_iter[_level-1];
    stk_slot.cddt_partial = zp_->partial_[_level];
    //printf("_level:%d, _iter[_level-1]:%d\n", _level, _iter[_level-1]);
    if(_cur->positive) stk_slot.positive = true;

    if(_level==0){
        stk_slot.extType=LEVEL0;
    }
    else if(_level==1 && _cur->positive){
        stk_slot.extType=LEVEL1;
    }
    else if(father_slot.extType==OMIT && _level>=1){
        if(_cur->positive) stk_slot.extType=FIRST_POSITIVE;
        else stk_slot.extType=OMIT;
    }
    else if(_level==this->depth_-1){
        if(_cur->positive) stk_slot.extType=INTERSECT_NUM;
        else stk_slot.extType=DIFFERENCE_NUM;
    }
    else{
        if(_cur->positive) stk_slot.extType=INTERSECT;
        else stk_slot.extType=DIFFERENCE;
    }
    if(_cur->left()){
        _init_slot_attr_helper(_cur->left(), _level+1, _iter, stk_slot, cursor);
    }
    if(_cur->right()){
        _init_slot_attr_helper(_cur->right(), _level+1, _iter, stk_slot, cursor);
    }
    _iter[_level]+=1;
}

void TrieCompiler::_init_slot_attr(){
    std::vector<int> iter(depth_, 0);
    SlotAttr& stk_slot = stk_->at(0, 0);
    size_t cursor = 0;
    _init_slot_attr_helper(root_, 0, iter, stk_slot, cursor);
}
*/



}