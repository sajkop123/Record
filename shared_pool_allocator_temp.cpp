#include <algorithm>
#include <list>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <atomic>

struct Img {
  Img(int id) : mId(id) { std::cout << "ctor" << mId << " " << this << "\n"; }
  Img() { std::cout << "ctor" << mId << " " << this << "\n"; }
  ~Img() { std::cout << "dtor" << mId << " " << this << "\n"; }
  int mId;
};

template <class T, bool grow_on_demand = true>
class Pool {
 private:
  struct Deleter {
    Deleter(Pool* p) : _m(p) {}
    void operator()(T* p) const noexcept {
      if (_m) {
        std::cout << "deleter::recycle " << p << "\n";
        _m->free(p);
      } else {
        std::cout << "deleter::delete " << p << "\n";
        delete p;
      }
    }
    Pool* _m;
  };

 public:
  Pool(const char* name_p, size_t n)
      : mutex_m(), free_m(0), used_m(0), name_m(name_p) {
    for (size_t i = 0; i < n; i++) {
      free_m.push_front(std::make_unique<T>());
    }
  }
  
  ~Pool() {
    std::cout << "Pool::dtor\n";
  }

  const char* getName() const { return name_m.c_str(); }

  std::shared_ptr<T> alloc() {
    std::unique_lock<std::mutex> lock(mutex_m);
    if (free_m.empty()) {
      if constexpr (grow_on_demand) {
        free_m.push_front(std::make_unique<T>());
      } else {
        throw std::bad_alloc();
      }
    }
    auto it = free_m.begin();
    std::shared_ptr<T> sptr(it->get(), Deleter(this));
    used_m.push_front(std::move(*it));
    free_m.erase(it);
    std::cout << "Pool::acquire obj " << sptr << "\n";
    return sptr;
  }

  size_t getFreeCount() {
    std::unique_lock<std::mutex> lock(mutex_m);
    return free_m.size();
  }

 private:
  void free(T* obj) {
    std::unique_lock<std::mutex> lock(mutex_m);
    auto it =
        std::find_if(used_m.begin(), used_m.end(),
                     [&](std::unique_ptr<T>& p) { return p.get() == obj; });
    if (it != used_m.end()) {
      std::cout << "Pool::recycle obj " << it->get() << "\n";
      free_m.push_back(std::move(*it));
      used_m.erase(it);
    } else {
      throw std::runtime_error("unexpected: unknown object freed.");
    }
  }

  std::mutex mutex_m;
  std::list<std::unique_ptr<T>> free_m;
  std::list<std::unique_ptr<T>> used_m;
  std::string name_m;
};

const int TOTAL_SIZE = 3;

struct B {
  int a;
  int b;
  int c;
  int d;
};

struct singleton_CB {
  static singleton_CB* getInstace() {
    static singleton_CB ins = singleton_CB();
    return &ins;
  }
  
  singleton_CB() {
    b = std::vector<B>(TOTAL_SIZE, B());
  }

  B* get() {
    return &b[(cnt++) % TOTAL_SIZE];
  }

  std::vector<B> b;
  std::atomic<int> cnt;
};

template<typename T>
struct Allocator_S {
  typedef T value_type;

  Allocator_S() {
    m = singleton_CB::getInstace();
    mCnt = 0;
  }

  Allocator_S(const Allocator_S& other) {
    m = other.m;
    mCnt = other.mCnt;
    std::cout << "COPY " << mCnt <<"\n";
  }
  Allocator_S(Allocator_S&& other) {
    m = other.m;
    mCnt = other.mCnt;
    // std::cout << "MOVE " << m->size() <<"\n";
  }

  template<typename U>
  Allocator_S(const Allocator_S<U>& other) noexcept {
    m = other.m;
    mCnt = other.mCnt;
    // std::cout << "MOVE<U> " << m->size() <<"\n";
  }

  [[nodiscard]] T* allocate(size_t n) {
    T* p = nullptr;
    if (m) {
      p = reinterpret_cast<T*>((m->get()));
      std::cout << "Alloc::use " << "p=" << p << "\n";
      // p = static_cast<T*>(::operator new(n * sizeof(T)));
    } else {
      p = static_cast<T*>(::operator new(n * sizeof(T)));
      std::cout << "Alloc::new : " << p << "\n";
    }
    return p;
  }

  void deallocate(T* p, std::size_t n) {
    std::cout << "deallocate : " << p << "\n";
    if (!m) {
      delete p;
    }
  }
  
  singleton_CB* m;
  int mCnt;
};

template<typename T>
class SharedPtr_Pool {
  struct Deleter_S { 
    Deleter_S() = default;
    void operator()(T* p) const noexcept {
      std::cout << "deleter\n";
    }
  };

 public:
  SharedPtr_Pool(const char* name_p, size_t n)
      : mData(std::vector<T>(n, T(0)))
      , mAlloc()
      , mb("cb_pool", n)
      , mCnt(0) {
    mCB = std::make_shared<std::vector<B>>();
    for (int i = 0; i < n; ++i) {
      mCB->push_back(B());
    }
  }

  std::shared_ptr<T> acquire() {
    return std::shared_ptr<T>(&mData[(mCnt++) % mData.size()], mDel, mAlloc);
  }

 private:
  std::vector<Img> mData;
  std::shared_ptr<std::vector<B>> mCB;
  Pool<B> mb;

  Deleter_S mDel;
  Allocator_S<T> mAlloc;
  int mCnt;
};

int main() {
  // {
  //   Pool<Img>* pool = new Pool<Img>("imgpool", 1);
  //   auto p = pool->alloc();
  //   p = nullptr;
  //   delete pool;
  // }
  
  std::cout << "============\n";

  {
    SharedPtr_Pool<Img> pool("shared_imgpool", TOTAL_SIZE);
    std::cout << "============\n";
    auto p1 = pool.acquire();
    std::cout << "============\n";
    p1 = nullptr;
    std::cout << "============\n";
    auto p2 = pool.acquire();
    std::cout << "============\n";
    p2 = nullptr;
  }

  // {
  //   Pool<Img> pool("imgpool", TOTAL_SIZE);
  //   auto p = pool.alloc();
  //   p = nullptr;
  // }
  return 1;
}
