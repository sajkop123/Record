/**
 * https://gracicot.github.io/reflection/2018/04/03/reflection-present.html
 */ 

#include <memory>

template<typename T> // foo version
auto foo_or_bar(T const& t) -> decltype(t.foo()) {
  return t.foo();
}

template<typename T> // bar version
auto foo_or_bar(T const& t) -> decltype(t.bar()) {
  return t.bar();
}

int main() {
  struct has_foo { void foo() const {} } f;
  struct has_bar { void bar() const {} } b;
  struct has_both : has_foo, has_bar {} fb;
  
  foo_or_bar(f);  // calls foo version
  foo_or_bar(b);  // calls bar version
  // foo_or_bar(fb); // error, ambiguous call
}