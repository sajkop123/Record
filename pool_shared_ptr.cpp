#include <optional>
#include <memory>
#include <iostream>
#include <queue>

struct A {
  A() {std::cout << "ctor" << "\n";}
  ~A() {std::cout << "dtor" << "\n";}
  A(const A&) { std::cout << "copy" << "\n"; }
  A(A&&) { std::cout << "move" << "\n"; }
};

struct Pool {
  Pool();
  std::shared_ptr<A> pop();
  void push1(const std::shared_ptr<A> val);
  void push2(A* val);
  
  std::queue<std::shared_ptr<A>> mQueue;
};

struct Deleter {
  Deleter(Pool* pool) : mPool(pool) {}
  void operator()(A* a) {
    std::cout << "deleter" << "\n";
    mPool->push2(a);
  }
  Pool* mPool;
};

Pool::Pool() {
  mQueue.push(std::shared_ptr<A>(new A(), Deleter(this)));
}

std::shared_ptr<A> Pool::pop() {
  auto res = mQueue.front();
  mQueue.pop();
  return res; 
}

void Pool::push1(const std::shared_ptr<A> val) {
  std::cout << "push1" << "\n";
  mQueue.push(val);
}

void Pool::push2(A* val) {
  std::cout << "push2" << "\n";
  mQueue.push(std::shared_ptr<A>(val));
}


int main() {
  Pool pool;
  {
    std::cout << "---- 0 ----" << "\n";
    std::shared_ptr<A> temp = pool.pop();
    std::cout << "---- 1 ----" << "\n";
    temp = nullptr;
    std::cout << "---- 2 ----" << "\n";
  }

  {
    std::cout << "---- 0 ----" << "\n";
    std::shared_ptr<A> temp = pool.pop();
    std::cout << "---- 1 ----" << "\n";
    temp = nullptr;
    std::cout << "---- 2 ----" << "\n";
  }

  std::cout << "END" << "\n";
}
