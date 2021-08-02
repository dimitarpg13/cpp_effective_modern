#include <iostream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <assert.h>
#include <array>
#include <initializer_list>

// Understanding  decltype
//
// decltype is an odd creature. Given a name or an expression, decltype
// tells you the name of the expression's type. Typically, what it tells
// you is exactly what you'd predict. Occasionally, decltype provides
// very non-intuitive answers.
//
// Let us begin with the most obvious cases which are intuitively clear.
// decltype typically parrots back the exact type of the name or expression
// you give it:
//
// const int i = 0;                  // decltype(i) is const int
//
// bool f(const Widget& w);          // decltype(w) is const Widget&
//                                   // decltype(f) is bool(const Widget&)
//
// struct Point {
//    int x, y;                      // decltype(Point::x) is int
// };                                // decltype(Point::y) is int
//
// Widget w;                         // decltype(w) is Widget
//
// if (f(w)) ...                     // decltype(f(w)) is bool
//
// template<typename T>              // simplified version of std::vector
// class vector {
// public: 
//   ...
//   T& operator[](std::size_t index);
//   ...
// };
//
// vector<int> v;                    // decltype(v) is vector<int>
// ...
// if (v[0] == 0) ...                // decltype(v[0]) is int&
//
//
// 

int main(const int argc, const char* argv[]) 
{


   return 0;
}
