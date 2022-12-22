#include <memory>
#include <type_traits>
#include <iostream>
#include <cstdint>
#include <functional>
#include <array>
#include <vector>
#include <atomic>
#include <cassert>

class Test {
 public:
  Test& operator=(const Test& other) {
    Test tmp(other);
    // CAS is inefficient but we need a copy anyway.
    std::swap(a, tmp.a);
    return *this;
  }
 public:
  int a[10] = {};
};


int main() {
  Test a;
  a.a[0] = 1111;
  
  // it seems somehow std::swap do "swap" behavior for two array.
  Test b;
  std::swap(a.a, b.a);
  std::cout << a.a[0] << "\n";
  std::cout << b.a[0] << "\n";

  // but we can use it for perfect cpoy
  // it's Copy-And-Swap idiom.
  Test c = b;
  std::cout << b.a[0] << "\n";
  std::cout << c.a[0] << "\n";
}
