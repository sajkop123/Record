#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <list>

enum ENUM {
  A,
  B,
  C,
  D,
  E,
  F,
  ENUM_NUMS,
};

template<typename KEY_T, typename VALUE_T>
class EnumMap {
 public:
  // EnumMap() { 
  //   puts(__PRETTY_FUNCTION__);
  // }
  void insert(const std::pair<KEY_T, VALUE_T>& pair) {
    size_t id = static_cast<size_t>(pair.first);
    if (mPtrMap[id] == nullptr) {
      mData.push_back(pair.second);
      mPtrMap[id] = std::addressof(mData.back());
      // printf("new elem pair(%zu, %d) %p\n",
      //        id, pair.second, mPtrMap[id]);
    } else {
      ELEM_PTR p = mPtrMap[id];
      *p = pair.second;
      // printf("replace elem pair(%zu, %d) %p\n",
      //        id, pair.second, mPtrMap[id]);
    }
  }

  // bool erase(const KEY_T& key) = delete;

  VALUE_T& operator[](const KEY_T& key) {
    size_t id = static_cast<size_t>(key);
    return *(mPtrMap[id]);
  }
  VALUE_T& at(const KEY_T& key) {
    size_t id = static_cast<size_t>(key);
    return *(mPtrMap[id]);
  }

 private:
  const static size_t ENUM_SIZE = static_cast<size_t>(KEY_T::ENUM_NUMS);
  typedef std::list<VALUE_T> CONTAINER_T;

  typedef VALUE_T* ELEM_PTR;
  typedef std::array<ELEM_PTR, ENUM_SIZE> PTR_MAP_T;

  typedef typename CONTAINER_T::iterator ITERATOR;
  typedef std::array<ITERATOR, ENUM_SIZE> ITER_MAP_T;

  CONTAINER_T mData;
  PTR_MAP_T mPtrMap = {};
};

static const size_t ROUND = 100;
ENUM test_enums[4] = {ENUM::E, ENUM::C, ENUM::D, ENUM::A};

void testInsert_stlMap() {
  std::map<ENUM, int> map;
  for (int i = 0; i < ROUND; ++i) {
    map.insert(std::make_pair(test_enums[i%4], i));
  }
}
void testInsert_enumMap() {
  EnumMap<ENUM, int> map;
  for (int i = 0; i < ROUND; ++i) {
    map.insert(std::make_pair(test_enums[i%4], i));
  }
}

std::map<ENUM, int> g_map;
EnumMap<ENUM, int> g_emap;

void testSearch_stlMap() {
  for (int i = 0; i < ROUND; ++i) {
    auto data = g_map.at(test_enums[i%4]);
  }
}
void testSearch_enumMap() {
  for (int i = 0; i < ROUND; ++i) {
    auto data = g_emap.at(test_enums[i%4]);
  }
}

int main() { 
  for (int i = 0; i < 4; i++) {
    g_map.insert(std::make_pair(test_enums[i%4], i));
    g_emap.insert(std::make_pair(test_enums[i%4], i));
  }
  return 1;
}

// benchmark
// https://quick-bench.com/q/kjmJPbf6azINW_-_ZsT5SgJwIME
