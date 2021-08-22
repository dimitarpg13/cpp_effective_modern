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
#include <vector>
#include <unordered_map>

// Distinguish between () and {} when creating objects
//
// Depending on your perspective, syntax choices for object initialization in C++11
// embody either an embarassment of riches or a confusing mess. As a general rule, 
// initialization values may be specified with parentheses, an equal sign or braces:
//
int x(0);       // initializer is parentheses

int y = 0;      // initializer follows "="

int z{ 0 };     // initializer is in braces

// in many cases, it is also possible to use an equals sign and braces together
//
int z2 = { 0 };  // initializer uses '=' and braces

// for the remainder of this Item, I will generally ignore the equals-sign-plus-braces
// syntax because C++ usually treats it the same as the braces-only version.
//
// For built-in types like int the difference between the equals-sign-plus-braces 
// syntax and braces-only syntax is academic, but for user-defined types, it is 
// important to distinguish initialization from assignment, because different function
// calls are involved:
//
struct Widget {
  int a;
  std::string b;
};


int main(const int argc, const char* argv[]) 
{

   Widget w1;        // call default constructor

   Widget w2 = w1;   // not an assignment; calls copy ctor

   w1 = w2;          // an assignment; calls copy operator=

   return 0;
}
