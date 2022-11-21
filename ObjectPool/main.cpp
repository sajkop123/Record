#include <vector>
#include <iostream>
#include <memory>

typedef std::shared_ptr<int> LazyImg;
typedef std::shared_ptr<double> LazySync;


struct SWIN {
  LazyImg gyroIn;
  LazyImg gyroOut;
};

struct HWOUT {
  LazyImg imgi;
  LazyImg imgo;
};

struct PEU {
  std::vector<LazySync> mWaitList;
  std::vector<LazySync> mNotifyList;

  SWIN mSWIn;
  HWOUT mHWOut;
};

class Test {
 public:
  Test() { printf("ctor\n"); }
  ~Test() { printf("dtor\n"); }
};

int main() {
  std::shared_ptr<Test> a1 = std::make_shared<Test>();
  printf("%d\n", *a1);
  std::cout << "asduasdasoidaspod" << "\n";
  return 0;
}