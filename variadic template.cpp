// variadic template to store data in std::array
template<typename DATA_T, typename...PARAMS>
std::array<DATA_T, sizeof...(PARAMS)+1>
get_data(const DATA_T& d1, const PARAMS& ...params) {
  return {d1, params...};
}
