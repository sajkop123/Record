#include <iostream>
#include <string>
#include <typeinfo>
#include <memory>
#include <stdio.h>
#include <vector>
#include <queue>

template<typename T> class Pool;

struct Img : std::enable_shared_from_this<Img> {
  Img(int id) : mId(id) { std::cout << "ctor" << mId << "\n"; }
  ~Img() { std::cout << "dtor" << mId << "\n"; }
  std::shared_ptr<Img> getPtr() { return shared_from_this(); }
  int mId;
};

template<typename T>
struct Allocator_Normal {
  typedef T value_type;

  Allocator_Normal() = default;

  template<typename U>
  Allocator_Normal(const Allocator_Normal<U>&) noexcept {}

  [[nodiscard]] T* allocate(size_t n) {
    std::cout << "allocate\n";
    return static_cast<T*>(::operator new(n * sizeof(T)));
  }

  void deallocate(T* ptr, std::size_t n) {
    std::cout << "deallocate " << ptr << "\n";
    delete(ptr);
  }
};

template<typename T>
struct Deleter_Normal {
  Deleter_Normal(Pool<T>* pPool) : mPool(pPool) {}
  void operator()(T* ptr) const noexcept {
    std::cout << "delete " << ptr << "\n";
    mPool->recycle(std::shared_ptr<T>(ptr, Deleter_Normal<T>(mPool)));
  }

  Pool<T> *mPool;
};

template<typename T>
class Pool {

 public:
  Pool(size_t n) {
    for (size_t i = 0; i < n; ++i) {
      mData.push_back(std::make_shared<T>(i));
      mDataSP.push(
        std::shared_ptr<T>(mData[i].get(),
                           Deleter_Normal<T>(this))
      );
      // std::cout << "ptr[" << i << "] use_count=" << mDataSP.back().use_count() << "\n";
    }
  }
  
  std::shared_ptr<T> acquire() {
    auto p = mDataSP.front();
    mDataSP.pop();
    return p;
  }

  void recycle(std::shared_ptr<T> p) {
    mDataSP.emplace(p);
  }

  void print() {
    std::cout << "mData : [";
    for (auto& item : mData) {
      std::cout << item << "/" << item.use_count() << " " ;
    }
    std::cout << "] ";
    std::cout << "mDataSP[" << mDataSP.size() <<"] : [";
    std::cout << mDataSP.front() << "/" << mDataSP.front().use_count() << " " ;
    std::cout << "]\n";
  }

  ~Pool() {
  }

 private:
  std::vector<std::shared_ptr<T>> mData;
  std::queue<std::shared_ptr<T>> mDataSP;
};

int main () {
  Pool<Img> pool(1);
  std::cout << "----------\n";
  {
    pool.print();
    auto p = pool.acquire();
    pool.print();
    p = nullptr;
    pool.print();
  }
  std::cout << "----------\n";
  {
    pool.print();
    auto p = pool.acquire();
    pool.print();
    p = nullptr;
    pool.print();
  }
  std::cout << "----------\n";
  {
    pool.print();
    auto p = pool.acquire();
    auto p2 = p;
    pool.print();
    p = nullptr;
    p2 = nullptr;
    pool.print();
  }

  std::cout << "----------\n";
  return 1;
}
