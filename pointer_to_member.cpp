#include <iostream>

using namespace std;

struct A {
  int a = 0;
  A(int _a) : a(_a) {}
};

struct B {
  A mA;
  int b = 0;
  B(int _a, int _b) : mA(_a), b(_b) {}
};

template<typename T_Data, typename T_Member, typename T_Value>
void set(T_Data* data, T_Member member, const T_Value& value) {
  (*data).*member = value;
}

void print(const B& b) {
  cout << "mA.a = " << b.mA.a << ", "
       << "b = " << b.b << "\n";
}

int main() {
  B b1(1,2);
  B b2(0,0);

  auto _l = [](auto pSrc, auto pDst, auto&& pMember) {
    if ((*pDst).*pMember != (*pSrc).*pMember) {
      (*pDst).*pMember = (*pSrc).*pMember;
    }
  };

  // I want b2.b = b1.b if not equivalent.
  _l(&b1, &b2, &B::b);
  // I want b2.mA.a = b1.mA,a if not equivalent.
  _l(&b1.mA, &b2.mA, &A::a);
  
  // set b2.b = 10;
  set(&b2, &B::b, 10);

  print(b1);
  print(b2);
}
