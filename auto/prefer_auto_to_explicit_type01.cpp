#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <typeinfo>
#include <type_traits>
#include <assert.h>
#include <array>
#include <initializer_list>
#include <deque>
#include <memory>

// Intro:
// auto is simple but at the same time it is more subtle than it looks.
// Using it saves typing but also can obstruct correctness and lead
// to subtle performance issues related to manual type declarations.
// Furthermore, some of auto's type deduction results, while dutigully
// conforming to the prescribed algorithm, are, from the perspective
// of the programmer, just wrong. When this is the case it is important
// to know how to guide auto to the right answer, because falling back
// on manual type declarations is an alternative that's often best 
// avoided.
//
// Prefer auto to explicit type declarations
//
// Let's declare a local variable which will be initialized by dereferncing
// an iterator:
//
template<typename It>
void dwim(It b, It e)
{
  while (b != e) {
    typename std::iterator_traits<It>::value_type 
      currValue = *b;
  }
};

// and with using auto:
//
template<typename It>
void dwim2(It b, It e)
{
  while (b != e) {
    auto currValue = *b;
  }
};

// and because auto uses type deduction it can represent types known only
// to compilers

struct Widget {
  int i;
  bool operator<(const Widget& other) {
    return i < other.i;
  }
};

auto derefUPLess =                        // comparison func
  [](const std::unique_ptr<Widget>& p1,   // for Widgets
     const std::unique_ptr<Widget>& p2)  // pointed to by std::unique_ptr's
  { return *p1 < *p2; };

int main(const int argc, const char* argv[]) 
{


   return 0;
}
