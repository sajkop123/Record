#include <memory>
#include <type_traits>
#include <iostream>
#include <cstdint>
#include <functional>
#include <array>
#include <vector>
#include <atomic>
#include <cassert>

/* Something about type erasure*/
// 1. not template
// 2. templated constructor
// 3. has `Concept` class, the type erased object and we hold the pointer
// 4. templated `Model` class inherits `Concept`

struct TypeErasure {
  // template constructor/operator, instead template struct.
  template<typename T>
  TypeErasure(T&& value) {
    *this = value;  // forward to operator= overloading
  }
  template<typename T>
  TypeErasure& operator=(T&& value) {
    m_value.reset(new Model<T>(value));
    return *this;
  }
  
  // I want my template object to have same common interface.
  void printName() const {
    // the impl of printName() on type erased.
    // it's going to forward on a function on the same name of `Concept`
    m_value->printName();
  }

 private:
  // base class for all these specilizations of `Model`
  struct Concept {
    virtual ~Concept() {}
    virtual void printName() const = 0;
  };
  // 'Model` is responsible to know the actual type it holds.
  template<typename T>
  struct Model : Concept {
    Model(const T& value) : m_val(value) {}
    void printName() const override {
      m_val.printName();
    }
    T m_val;
  };

 private:
  std::unique_ptr<Concept> m_value;
};


struct Allen {
  void printName() const { std::cout << "Allen" << "\n"; }
};
struct Hank {
  void printName() const { std::cout << "Hank" << "\n"; }
};

int main() {
  TypeErasure Te{Allen()};
  Te.printName();
  Te = Hank();
  Te.printName();
}
