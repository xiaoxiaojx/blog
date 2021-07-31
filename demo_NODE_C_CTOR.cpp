#include <iostream>

# define NODE_CTOR_PREFIX static

#define NODE_C_CTOR(fn)                                               \
  NODE_CTOR_PREFIX void fn(void) __attribute__((constructor));        \
  NODE_CTOR_PREFIX void fn(void)

using namespace std;

NODE_C_CTOR(_register_) {                              \
    std::cout << "before main function" << std::endl;                               \
}

int main()
{
  char site[] = "Hello, world!";
  cout << site << endl;
  return 0;
}