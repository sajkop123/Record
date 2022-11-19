#include <iostream>
#include <vector>
#include <ranges>
#include <array>

// variadic template to store data in std::array
template<typename DATA_T, typename...PARAMS>
std::array<DATA_T, sizeof...(PARAMS)+1>
get_data1(const DATA_T& d1, const PARAMS& ...params) {
  return {d1, params...};
}

// change to `auto`, basically not help anything 
template<typename DATA_T, typename...PARAMS>
auto
get_data2(const DATA_T& d1, const PARAMS& ...params) {
  return std::array<DATA_T, sizeof...(PARAMS)+1>{d1, params...};
}

// If only there was a way to automatically deduce the type of the
// array being returned...

// it's class template argument deduction.
// in C++17 "std::vector vec{1,2,3};" -> it's possible
template<typename DATA_T, typename...PARAMS>
auto
get_data3(const DATA_T& d1, const PARAMS& ...params) {
  return std::array{d1, params...};   // auto deduce type/size
}

// and now we can simplify the template argument
// no need the first DATA_T anymore
// notice : this might make using this function harder as it would result in
//          compile error if the "types differ"
template<typename... PARAMS>
auto
get_data4(const PARAMS&... params) {
  return std::array{params...};
}

// In C++20's `auto` concept we might make this.
// notice : the only problem now is that this code requires the types
//          to be "copyable".
auto
get_data5(const auto& ... params) {
  return std::array{params...};
}

// If only there was a way to forward argements and avoid copies...

// add forwarding references and no necessary copies of params
// It guarantee copy/move elision of return value
auto
get_data5(auto&& ... params) {
  return std::array{std::forward<decltype(params)>(params)...};
}

template<typename DATA_T, size_t SIZE>
void show(const std::array<DATA_T, SIZE>& arr) {
  for (const auto& item : arr) {
    std::cout << item << "\n";
  }
}

int main()
{
  auto arr1 = get_data1(1,2,3,4);
  auto arr2 = get_data2(1,2,3,4);
  auto arr3 = get_data3(1,2,3,4);
  auto arr4 = get_data4(1,2,3,4);
  auto arr5 = get_data5(1,2,3,4);
}
