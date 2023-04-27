#include <optional>
#include <memory>
#include <iostream>
#include <queue>
#include <list>
#include <array>
#include <mutex>
#include <cassert>
#include <variant>
#include <unordered_map>

struct A {
  int g = 0;
};
template<typename T> struct is_shared_ptr : std::false_type {};
template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template<typename T>
struct Allocator {
  static T alloc() { return T(); }
};
template<typename T>
struct Allocator<std::shared_ptr<T>> {
  static std::shared_ptr<T> alloc() { return std::make_shared<T>(); }
};

template<typename T, class ALLOC = Allocator<T>>
struct ClusteredType {
  ClusteredType()  { 
    // mTypeName = std::string()
  }
  T& acquire() {
    std::unique_lock<std::mutex> _l(mMutex);
    mList.emplace_back(ALLOC::alloc());
    return mList.back();
  }
  size_t size() const { return mList.size(); }

  std::list<T> mList;
  std::mutex mMutex;
};

struct Relavence {
  std::variant<A*, std::shared_ptr<A>*> mp;
};

// auto f() {
//   return 1;
// }

struct Cluster {
  ClusteredType<A> mA;
  ClusteredType<std::shared_ptr<A>> mpA;
  std::unordered_map<size_t, Relavence> mMap;

  // A* rel(size_t id) {
  //   return std::get<A*>(mMap.at(id).mp);
  // }
  // std::shared_ptr<A>* rel(size_t id) {
  //   return std::get<std::shared_ptr<A>*>(mMap.at(id).mp);
  // }
  
  A& acq_mA(size_t id) {
    mMap[id].mp = &mA.acquire();
    return *(std::get<A*>(mMap.at(id).mp));
  }
  std::shared_ptr<A>& acq_mpA(size_t id) {
    mMap[id].mp = &mpA.acquire();
    return *(std::get<std::shared_ptr<A>*>(mMap.at(id).mp));
  }
};

int main() {
  Cluster a;
  auto t1 = a.acq_mA(1);
  auto t2 = a.acq_mA(3);
  auto t3 = a.acq_mA(2);
  auto t4 = a.acq_mpA(4);
  std::cout <<  a.mA.size() << "\n";
  std::cout <<  a.mpA.size() << "\n";

  // t1.g = 22;
  // std::cout << a.rel(1)->g << "\n";

  t4->g = 212321l;
  std::cout << a.mpA.mList.front()->g << "\n";
}
