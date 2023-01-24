// reference : https://www.youtube.com/watch?v=xJSKk_q25oQ

#include <functional>
#include <memory>

template <typename T>
struct function {};

template <typename Ret, typename... Param>
class function<Ret(Param...)> {
 public:
  // more generic syntax than ctor above
  template <typename FunctionObject>
  function(FunctionObject fo)
      : callable{
            std::make_unique<callable_impl<FunctionObject>>(std::move(fo))} {}

  Ret operator()(Param... param) { return callable->call(param...); }

 private:  // store the function.
  struct callable_interface {
    virtual Ret call(Param...) = 0;
    virtual ~callable_interface() = default;
    // finish rule of 5
  };

  template <typename Callable>
  struct callable_impl : callable_interface {
    callable_impl(Callable callable_) : callable{std::move(callable_)} {};
    Ret call(Param... param) { return std::invoke(callable, param...); }
    Callable callable;
  };

  std::unique_ptr<callable_interface> callable;
};

int f(int a, int b) { return a + b; }

int main() {
  function<int(int, int)> func{f};
  function<int(int, int)> func2{[](int a, int b) { return a + b; }};
  return func(1, 2) + func2(3, 4);
}
