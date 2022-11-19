#include <iostream>
#include <vector>
#include <ranges>
#include <array>
#include <initializer_list>

// It's an recursive, variadic expansion solution
// But oh no... It can be horribly slow at compile time, difficult to optimize
// and difficult to debug.
template<typename T>
auto sum(const T& t) {
  return t;
}
template<typename FIRST, typename ... T>
auto sum(const FIRST& first, const T& ... t) {
  return first + sum(t...);
}

// In C++11, we can do it with an interesting `initializer_list` hack
// Avoids the recursion but fixes the result type as the type of
// the first parameter
template<typename FIRST, typename ... T>
auto sum2(const FIRST& first, const T& ... t) {
  auto result = first;
  // {result += t, 0}, it returns the last most one. The operation regardless
  // of what type is and what type ti returns `0` is going to be used to 
  // initialize the `intializer_list` is performed.
  // (void) is used to avoid unused variable error 
  (void)std::initializer_list<int>{ (result += t, 0) ... };
  return result;
}

// If only there was some way to ask the compiler to sum up this parameter
// set for us...

// In C++ 17, fold expression!!
template<typename ... T>
auto sum3(const T& ... t) {
  return (t + ...);
}


int main()
{
  auto a = sum2(1,2,3,4,5);
  std::cout<< a << "\n";
}
