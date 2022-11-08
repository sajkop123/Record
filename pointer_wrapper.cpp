#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <array>
#include <functional>
#include <memory>
#include <algorithm>
#include <list>
#include <vector>
#include <numeric>
#include <random>

class Img;
typedef std::shared_ptr<Img> ImgPtr;

namespace detail {
template <class T> constexpr T& FUN(T& t) noexcept { return t; }
template <class T> void FUN(T&&) = delete;
}

template<typename T>
class pointer_wrapper {
 public:
  pointer_wrapper() : _ptr(nullptr) {}
  pointer_wrapper(T& _t) : _ptr(&_t) {}
  pointer_wrapper(T&& _t) : _ptr(&_t) {}

  // assignment
  pointer_wrapper& operator=(const pointer_wrapper& x) noexcept = default;

  // access
  constexpr operator T& () const noexcept { return *_ptr; }
  constexpr T& get() const noexcept { return *_ptr; }

 private:
  T* _ptr;
};

struct HW {
  ImgPtr hwIn;
  ImgPtr hwOut;
};

struct SW {
  ImgPtr swIn;
  ImgPtr swOut;
};

struct WPE_T {
  ImgPtr wpei;
  ImgPtr wpeo;
  ImgPtr tuning;
};

void fillWPE_SW(SW& sw, const WPE_T& wpe) {
  sw.swIn = wpe.tuning;
}

void fillWPE_HW(HW& hw, const WPE_T& wpe) {
  // hw.hwIn = wpe.wpei;
  hw.hwOut = wpe.wpeo;
}

int main(){
  int c = 1;
  pointer_wrapper<const int> b = c;
  pointer_wrapper<int> b2 = 3;
  return b;
}
