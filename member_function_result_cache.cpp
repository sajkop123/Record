#include <functional>
#include <iostream>
#include <map>
using namespace std;

template <typename T>
void func(T) {
  puts(__PRETTY_FUNCTION__);
}

// general function traits
template<typename T>
struct function_traits;

// partial specilization for member function pointer
template<class C, typename R, typename ...Args>
struct function_traits<R (C::*)(Args...)> {
  using return_type = R;
  using class_type = C;
  using function_type = R (C::*)(Args...);
  using arg_tuple_type = std::tuple<Args...>;
  static constexpr size_t arg_count = sizeof...(Args); 
};

// helper
template<typename T>
using getReturnType = typename function_traits<T>::return_type;
template<typename T>
using getClassType = typename function_traits<T>::class_type;
template<typename T>
using getArgTupleType = typename function_traits<T>::arg_tuple_type;
template<typename T>
using getFnType = typename function_traits<T>::function_type;

template<typename MemFuncPtr_T>
struct MemFnCache {
  using Return_T = getReturnType<MemFuncPtr_T>;
  using Class_T = getClassType<MemFuncPtr_T>;
  using ArgTuple_T = getArgTupleType<MemFuncPtr_T>;
  using Fn_T = getFnType<MemFuncPtr_T>;

  MemFnCache() {}
  MemFnCache(Class_T* pObj, MemFuncPtr_T pMemFn) :
      mpObj(pObj), mpMemFn(pMemFn) {}

  template<typename ...Args>
  Return_T execute(Args&&... args) {
    std::cout << "user called\n";
    if (mResult.count(std::forward_as_tuple(args...)) <= 0) {
      mResult[std::forward_as_tuple(args...)] =
          (mpObj->*mpMemFn)(std::forward<Args>(args)...);
    }
    return mResult.at(std::forward_as_tuple(args...));
  }
  
 private:
  Class_T* mpObj;
  MemFuncPtr_T mpMemFn;
  mutable std::map<ArgTuple_T, Return_T> mResult;
};


template <typename T>
struct ResultMemento {

  #define MAKE_FUNC(RET, FN, ...)  \
    virtual RET FN##_impl(__VA_ARGS__) = 0;  \
    template<typename ...Args> RET FN(Args&&... args) {  \
      return mFnCache_##FN.execute(std::forward<Args>(args)...); }  \
      MemFnCache<decltype(&ResultMemento::FN##_impl)> mFnCache_##FN;

  MAKE_FUNC(bool, test, int, int);

  #undef MAKE_FUNC

  ResultMemento() :
      mFnCache_test(static_cast<T*>(this), &ResultMemento::test_impl) {}

};

struct Usage : ResultMemento<Usage> {
  bool test_impl(int a, int b) override {
    std::cout << "impl" << "\n";
    return a == 0 && b == 0;
  }

  // TODO : consider how to support const function
  // wihch also means the function is thread-safe since read-only property.
  // We should take care on write result into MemFnCache is not thread-safe.
  bool test2(int a, int b) const {
    return a == 0 && b == 0;
  }
};

int main() {
  Usage usage;
  bool b;
  b = usage.test(0, 10);
  b = usage.test(0, 10);
  b = usage.test(1, 10);
  b = usage.test(0, 10);

  return 1;
}
