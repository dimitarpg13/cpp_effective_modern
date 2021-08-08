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
#include <functional>

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
     const std::unique_ptr<Widget>& p2)   // pointed to by std::unique_ptr's
  { return *p1 < *p2; };

// In C++14 we can introduce parameters to lambda function which types involve auto:

auto derefLess =                          // C++14 comparison  
  [](const auto& p1, const auto& p2)      // function for values pointed
  { return *p1 < *p2; };                  // to by anything pointer-like

// we do not really need auto to declate a variable that holds closure because we
// can use a std::function object. But what is a std::function object?
//
// std::function is a template in the C++11 stdlib that generalizes the idea of a
// function pointer. Whereas function pointers can point only to functions, 
// std::function objets can refer to any callable object i.e. anything that can be
// invoked like a function. Just as you must specify the type of function to point
// to when you create a function pointer (i.e. the signature of the functions), you
// must specify the type of the function to refer to when you create a std::function
// object. You do that through std::function's template parameter. For example, to
// declare a std::function object named func that could refer to any callable object 
// acting as if it had this signature:
// 
// bool( const std::unique_ptr<Widget>&,  // C++11 signature for std::unique_ptr<Widget>
//       const std::unique_ptr<Widget>&)  // std::unique_ptr<Widget> comparison function
// 
// you'd write this:
//
std::function<bool(const std::unique_ptr<Widget>&,
                   const std::unique_ptr<Widget>&)> func;


int main(const int argc, const char* argv[]) 
{


   return 0;
}
