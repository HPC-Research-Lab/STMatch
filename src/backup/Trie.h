#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <climits>
#include <algorithm>
#include <cassert>


namespace libra {

  /**
   * Prefix tree Implementation
  */
  struct TrieNode {
    int val_ = -1;
    bool is_candidate_ = false;
    TrieNode* father_ = nullptr;
    std::pair<TrieNode*, TrieNode*> children_;
    std::set<int> candidates_in_thepath_;
    std::set<int> candidates_in_subtree_;


    TrieNode(int _val) {
      children_.first = nullptr;
      children_.second = nullptr;
      this->val_ = _val;
    }
    ~TrieNode() {}
    TrieNode*& left() {
      return children_.first;
    }
    TrieNode*& right() {
      return children_.second;
    }
    bool is_end() {
      return  children_.first == nullptr && children_.second == nullptr;
    }

    bool _valid_for_partial(std::vector<std::vector<int> >* L_adj_matrix_) {
      if (val_ == INT_MIN) return false;
      std::vector<int> line_valid(candidates_in_subtree_.size(), 0);
      for (auto& i : candidates_in_thepath_) {
        std::vector<int> tmp_line_valid;
        for (auto& j : candidates_in_subtree_) {
          tmp_line_valid.push_back((*L_adj_matrix_)[i][j]);
        }
        for (int k = 0; k < candidates_in_subtree_.size(); k++) {
          line_valid[k] |= tmp_line_valid[k];
        }
      }
      for (int i = 0; i < candidates_in_subtree_.size(); i++) {
        if (!line_valid[i]) return false;
      }
      return true;
    }

    std::vector<int> partial_nodes(std::vector<std::vector<int> >* L_adj_matrix_) {
      std::vector<int> res;
      if (!_valid_for_partial(L_adj_matrix_)) return res;
      std::map<std::vector<int>, int> reversed_res;
      for (auto& i : candidates_in_thepath_) {
        std::vector<int> tmp_line_valid;
        for (auto& j : candidates_in_subtree_) {
          if ((*L_adj_matrix_)[i][j] == 1) {
            tmp_line_valid.push_back(j);
          }
        }
        std::sort(tmp_line_valid.begin(), tmp_line_valid.end());
        if (tmp_line_valid.size() != 0) {
          if (reversed_res.find(tmp_line_valid) != reversed_res.end()) {
            reversed_res[tmp_line_valid] = ((reversed_res[tmp_line_valid] > i) ? reversed_res[tmp_line_valid] : i);
          }
          else {
            reversed_res[tmp_line_valid] = i;
          }
        }
      }
      for (auto it = reversed_res.begin(); it != reversed_res.end(); it++) {
        res.push_back(it->second);
        //std::cout<<it->second<<" ";
      }
      //std::cout<<std::endl;
      return res;
    }
  };

  class Trie {
  public:
    Trie(size_t _size) {
      this->size_ = _size;
      root_ = new TrieNode(INT_MIN);
    }
    ~Trie() {
      clear(root_);
    }
    void clear(TrieNode* _root) {
      if (_root->left())  clear(_root->left());
      if (_root->right()) clear(_root->right());
      delete _root;
    }

    /** Inserts a word into the trie. */
    void _insert(TrieNode* _cur, size_t _idx,
      std::vector<int>& _candidates_in_thepath,
      std::vector<int>& _word, bool _positive_zero = true) {
      if (_idx == _word.size()) {
        return;
      }
      assert(std::abs(_word[_idx]) == _idx);
      assert(_cur != nullptr);
      TrieNode** dummy;
      if (_word[_idx] == 0) {
        if (!_positive_zero) {
          dummy = &(_cur->left());
        }
        else {
          dummy = &(_cur->right());
        }
      }
      else if (_word[_idx] < 0) {
        dummy = &(_cur->left());
      }
      else {
        dummy = &(_cur->right());
      }

      if (!(*dummy)) {
        (*dummy) = new TrieNode(_word[_idx]);
      }
      if (_idx == _word.size() - 1) {
        (*dummy)->is_candidate_ = true;
      }

      (*dummy)->candidates_in_thepath_.insert(
        _candidates_in_thepath.begin(), _candidates_in_thepath.end());
      if ((*dummy)->is_candidate_) {
        _candidates_in_thepath.push_back(_idx + 1);

      }
      (*dummy)->candidates_in_subtree_.insert(_word.size());
      _insert((*dummy), _idx + 1, _candidates_in_thepath, _word);
    }


    void insert(std::vector<int>& _word, bool _positive_zero = true) {
      std::vector<int> _candidates_in_thepath(1, 0);
      _insert(root_, 0, _candidates_in_thepath, _word, _positive_zero);
    }

    void _trie2vec(TrieNode* _cur,
      int _level,
      std::vector<int>& _iter,
      std::vector<std::vector<std::vector<int>>>& _partial_level_dependency,
      std::vector<std::vector<std::pair<int, int>>>& _level_dependency,
      std::vector<int>& _candidate_index) {

      if (_cur->left()) {
        _level_dependency[_level + 1].push_back(std::make_pair(_iter[_level], (_cur->left()->val_)));
        _trie2vec(_cur->left(), _level + 1, _iter, _partial_level_dependency, _level_dependency, _candidate_index);
      }
      if (_cur->right()) {
        _level_dependency[_level + 1].push_back(std::make_pair(_iter[_level], (_cur->right()->val_)));
        _trie2vec(_cur->right(), _level + 1, _iter, _partial_level_dependency, _level_dependency, _candidate_index);
      }
      if (_cur->is_candidate_) {
        _candidate_index[_level] = _iter[_level];
      }
      _partial_level_dependency[_level].push_back(std::vector<int>(size_, -1));
      std::vector<int> res = _cur->partial_nodes(L_adj_matrix_);
      for (int i = 0; i < res.size(); i++) {
        _partial_level_dependency[_level][_iter[_level]][i] = res[i];
      }
      _iter[_level]++;
    }

    void trie2vec(
      std::vector<std::vector<std::vector<int>>>& _partial_level_dependency,
      std::vector<std::vector<std::pair<int, int>>>& _level_dependency,
      std::vector<int>& _candidate_index) {
      std::vector<int> iter(size_ + 1, 0);
      _trie2vec(root_, 0, iter, _partial_level_dependency, _level_dependency, _candidate_index);
    }

  public:
    int size_ = 0;
    TrieNode* root_;
    std::vector<std::vector<int> >* L_adj_matrix_;
  };

}