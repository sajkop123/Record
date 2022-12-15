#include <memory>
#include <type_traits>
#include <iostream>
#include <cstdint>
#include <functional>
#include <array>
#include <vector>
#include <atomic>
#include <cassert>

struct Source {
  int a;
};

struct Dest {
  int a;
};

// type erasure struct
// just for assignment, not touch lifecycle
struct TE {
  void* repr_ = nullptr;
  void (*equal_)(void*, void*);
  
  template<class T>
  TE(T &n)
    : repr_(&n)
    , equal_([](void* self, void* from) {
      *(T*)(self) = *(T*)(from);
      T temp =  *(T*)(self);
      // std::cout << temp << "\n";
    }) {
      puts(__PRETTY_FUNCTION__);
    }

  TE& operator=(TE&& rhs) noexcept {
    equal_(this->repr_, rhs.repr_);
    return *this;
  }
  TE& operator=(const TE& rhs) noexcept {
    equal_(this->repr_, rhs.repr_);
    return *this;
  }
};

int main() {
  int src = 10;
  int dst = 2;

  TE teSrc(src);
  TE teDst(dst);
  teDst = teSrc;

  std::cout << dst << "\n";
}

// https://godbolt.org/z/sr4oE8ojo
