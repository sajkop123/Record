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

typedef size_t PEU_Ptr;

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

struct Relavence {
  std::variant<A*, std::shared_ptr<A>*> mp;
  std::vector<int> mImgs;
};


template<typename T, class ALLOC = Allocator<T>>
struct ClusteredType {
  T& acquire(PEU_Ptr peu) {
    std::unique_lock<std::mutex> _l(mMutex);
    T*& res = mpPeuRel[peu];
    if (res == nullptr) {
      mList.emplace_back(ALLOC::alloc());
      res = &mList.back();
    }
    printf("peu:%zd %p mList.size=%zu, mPeuRel.size=%zu\n",
           peu, res, mList.size(), mpPeuRel.size());
    return *res;
  }
  size_t size() const { return mList.size(); }

  std::list<T> mList;
  std::unordered_map<PEU_Ptr, T*> mpPeuRel; 
  std::mutex mMutex;
};

struct Cluster {
  ClusteredType<A> mA;
  ClusteredType<std::shared_ptr<A>> mpA;
  
  A& acq_mA(PEU_Ptr peu) {
    return mA.acquire(peu);
  }
  std::shared_ptr<A>& acq_mpA(PEU_Ptr peu) {
    return mpA.acquire(peu);
  }
};

int main() {
  Cluster a;
  auto &t1 = a.acq_mA(1);
  auto &t2 = a.acq_mA(3);
  auto &t3 = a.acq_mA(2);

  t1.g = 22;
  std::cout << a.acq_mA(1).g << "\n";

  a.acq_mA(3).g = 123;
  std::cout << t2.g << "\n";

  std::cout << "=================" << "\n";

  auto &t4 = a.acq_mpA(70);
  t4->g = 212321l;
  std::cout << a.mpA.mList.front()->g << "\n";

  return 0;
}
