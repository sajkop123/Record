#include <memory>

struct A {
  A() { printf("ctor\n"); }
  ~A() { printf("dtor\n"); }
  int a;
  int b;
  int c;
};

int main() {
  A a;
  a.~A();
}