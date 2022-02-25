#ifndef TRIE_GENERATOR_H_
#define TRIE_GENERATOR_H_

#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <climits>

#include "pattern/StlPattern.h"
#include "pattern/CPattern.h"
#include "ZCompiler.h"

namespace libra{

/**
 * Prefix tree Implementation
*/
struct TrieNode{
    unsigned int val_=0;
    bool is_candidate_=false;
    bool positive = true;
    std::pair<TrieNode*, TrieNode*> children_;
    //std::set<int> candidates_in_thepath_;
    //std::set<int> candidates_in_subtree_;
   
    TrieNode(bool _positive, int _val){
        children_.first=nullptr;
        children_.second=nullptr;
        this->val_ = _val;
        this->positive = _positive;
    }
    ~TrieNode(){}
    TrieNode*& left(){
        return children_.first;
    }
    TrieNode*& right(){
        return children_.second;
    }
    bool is_end(){
        return  children_.first==nullptr && children_.second==nullptr;
    }
};

struct TrieCompiler {
public:
    TrieCompiler();
    ~TrieCompiler();
    void print();
    
    void compile(StlPattern& _stl_pattern);

    void write_stk_info(CPattern&);

private:
    void _clear(){
        ops_.clear();
        cddt_idx_.clear();
        level_size_.clear();
        is_omit_.clear();
        _clear_trie(root_);
        depth_ = 0;
        num_node_ = 0;
    }
    void _clear_trie(TrieNode* _root);
    void _print_ops();
    void _print_trie();

    void _fill_ops();
    void _build_trie();
    void _insert(std::vector<std::pair<bool, int>> _ops);
    void _insert_helper(TrieNode* _cur, size_t _idx,  auto & _ops);

    void _write_stk_info_helper(TrieNode*, int, std::vector<int>&, CPattern&, set_op_t);

    void _remove_omit_op(auto& _reorder, CPattern& _stk_info);
    void _move_cddt_leftmost(auto& _reorder);
    void _reorder_set_ops(auto& _reorder, CPattern& _stk_info);
    void print_pattern(CPattern& _p);
    //void _init_level_attr();
    //void _init_slot_attr();
    //void _init_slot_attr_helper(TrieNode* _cur, int _level, std::vector<int>& _iter, SlotAttr& father_slot, size_t& cursor);

    StlPattern* stl_pattern_;
    ZCompiler z_compiler_;
    
    std::vector<std::vector<std::pair<bool, int>>> ops_;
    std::vector<int> cddt_idx_;
    std::vector<int> level_size_; 
    std::vector<int> is_omit_;
    TrieNode* root_ = nullptr;
    size_t depth_ = 1;
    int num_node_ = 1;

};

}
#endif