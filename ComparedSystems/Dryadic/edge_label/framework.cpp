#include <vector>
#include <tuple>
#include <type_traits>
#include <cassert>

template<bool store_left, bool store_int, bool store_right, typename T>
std::tuple<uint64_t, uint64_t, uint64_t> set_int_diff(
      const T vert_a, const T vert_b,
      const std::vector<T> &set_a,
      const std::vector<T> &set_b,
      std::vector<T> *res_left=nullptr,
      std::vector<T> *res_int=nullptr,
      std::vector<T> *res_right=nullptr) {
  static_assert(std::is_integral<T>::value);
  if constexpr(store_left) assert(res_left != nullptr);
  if constexpr(store_int) assert(res_int != nullptr);
  if constexpr(store_right) assert(res_right != nullptr);
  uint64_t pos_a = 0, pos_b = 0;
  const uint64_t len_a = set_a.size(), len_b = set_b.size();
  uint64_t idx_left = 0, idx_int = 0, idx_right = 0;
  while(pos_a < len_a && pos_b < len_b) {
    T val_a = set_a[pos_a], val_b = set_b[pos_b];
    if(val_a < val_b) {
      pos_a++;
      if constexpr(store_left) {
        if(val_a != vert_b) {
          (*res_left)[idx_left] = val_a;
          idx_left++;
        }
      }
    } else if(val_b < val_a) {
      pos_b++;
      if constexpr(store_right) {
        if(val_b != vert_a) {
          (*res_right)[idx_right] = val_b;
          idx_right++;
        }
      }
    } else {
      pos_a++;
      pos_b++;
      if constexpr(store_int) (*res_int)[idx_int] = val_a;
      idx_int++;
    }
  }
  if constexpr(store_left) {
    while(pos_a < len_a) {
      T val_a = set_a[pos_a];
      if(val_a != vert_b) {
        (*res_left)[idx_left] = val_a;
        idx_left++;
      }
      pos_a++;
    }
  } else {
    idx_left = len_a - idx_int - 1;
  }
  if constexpr(store_right) {
    while(pos_b < len_b) {
      T val_b = set_b[pos_b];
      if(val_b != vert_a) {
        (*res_right)[idx_right] = set_b[pos_b];
        idx_right++;
      }
      pos_b++;
    }
  } else {
    idx_right = len_b - idx_int - 1;
  }
  return std::make_tuple(idx_left, idx_int, idx_right);
}

#include <iostream>
int main() {
  std::vector<int> a(10), b(10);
  for(int i = 0; i < 10; i++) {
    a[i] = i;
    b[i] = 2 * i;
  }
  std::vector<int> vl(10, -1), vi(10, -1), vr(10, -1);
  int l, i, r;
  std::tie(l, i, r) = set_int_diff<true, true, true>(16, 3, a, b, &vl, &vi, &vr);
  std::cout << "(" << l << "," << i << "," << r << ")\n";
  std::cout << "left:\t";
  for(int x : vl) std::cout << '\t' << x;
  std::cout << "\nintersect:";
  for(int x : vi) std::cout << '\t' << x;
  std::cout << "\nright:\t";
  for(int x : vr) std::cout << '\t' << x;
  std::cout << "\n";
  return 0;
}

