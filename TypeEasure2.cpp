#include <memory>
#include <type_traits>
#include <iostream>
#include <cstdint>
#include <functional>
#include <array>
#include <vector>
#include <atomic>
#include <cassert>

struct TypeErasure {
  template<typename T>
  TypeErasure(T&& value) { *this = value; }

  template<typename T>
  TypeErasure& operator=(T&& value) {
    m_value.reset(new Model<T>(value));
    return *this;
  }
  
  void printName() const {
    m_value->printName();
  }

 private:
  struct Concept {
    virtual ~Concept() {}
    virtual void printName() const = 0;
  };
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
