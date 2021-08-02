#include <iostream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <assert.h>
#include <array>
#include <initializer_list>

// Understanding auto type deduction
//
// With one curious exception auto type deduction is template type deduction
// How can that be? Template type deduction involves templates, functions
// and parameters but auto deals with none of those things.
// There is a direct mapping (algorithmic transformation) between template type
// deduction and auto type deduction. 
//
// Previously we have used the following function template to explain template
// type deduction
//
//    template<typename T>
//    void f(ParamType param);
//
// and this general call:
//
//    f(expr);   // call f with some expression
//
// In the call to f, compilers use expr to deduce types for T and ParamType.
// When a variable is declared using auto, auto plays the role of T in the
// template, and the type specifier for the variable acts as ParaType. This 
// is easier to show than to describe, so consider the example below:
//
auto x = 27;

// Here, the type specifier for x is simply auto by itself. On the other hand,
// in this declaration,
const auto cx = x;

// the type specifier is const auto. And here,
const auto& rx = x;

// the type specifier is const auto&. To deduce types for x, cx, and rx in
// these examples, compilers act as if there were a template for each 
// declaration as well as a call to that template with the corresponding
// initializing expression:

template<typename T>                  // conceptual template for
void func_for_x(T param) {            // deducing x's type 
  // some initialization code here
};

template<typename T>                  // conceptual template for
void func_for_cx(const T param) {     // deducing cx's type
  // some initialization code here
};


template<typename T>                  // conceptual template for
void func_for_rx(const T& param) {    // deducing rx's type
  // some initialization code here
};

// Recall, we have divided template type deduction into three cases
// based on the characteristics of ParamType, the type specifier for
// param in the general function template. In a variable declaration
// using auto, the type specifier takes the place of ParamType, so there
// are three cases for that too:
//
// Case 1: the type specifier is a pointer or reference, but not a 
// universal reference
//
// Case 2: the type specifier is a universal reference
//
// Case 3: the type specifier is neither a pointer nor a reference
//
// We have already seen examples of cases 1 and 3:
//
auto x2 = 27;            // case 3 (x is neither ptr nor reference)

const auto cx2 = x;      // case 3 (cx isn't either)

const auto& rx2 = x;     // case 1 (rx is a non-universal ref)

// Case 2 works as you'd expect:
//
auto&& uref1 = x;        // x is int and lvalue,
                         // so uref1's type is int&

auto&& uref2 = cx;       // cx is const int and lvalue,
                         // so uref2's type is const int&

auto&& uref3 = 27;       // 27 is int and rvalue,
//                       // so uref3's type is int&&
//
// We have similar rules for decay of array and function names into 
// pointers for non-reference type specifiers:
//
const char name[] =      // name's type is const char[13]
   "R. N. Briggs";

auto arr1 = name;        // arr1's type is const char*

auto& arr2 = name;       // arr2's type is
                         // const char (&)[13]

void someFunc(int, double) {
  // do something here
};

auto func1 = someFunc;   // func1's type is void (*)(int, double)

auto& func2 = someFunc;  // func2's type is void (&)(int, double)

// as you can see there is a direct mapping between template type 
// deduction and auto type deduction. There is literally an algorithmic 
// transformation from one to the other
//
// There is one case in which auto and template deduction differ. Observe
// that if you want to declare an int with an initial value of 27, C++98
// will give you two syntactic choices:
//
   int x11 = 27;
   int x12(27);
//
// C++11, through its support for uniform initialization, adds these:
//
   int x13 = { 27 };
   int x14{ 27 };
//
// So, four different syntaxes are available to build the result of assigning
// an int with the value 27
//
// With introduction of the keyword auto we would like to replace all of the 
// four syntaxes with the auto equivalents as:
//
   auto x21 = 27;
   auto x22(27);
   auto x23 = { 27 }; // type is std::initializer_list<int>, value: { 27 }
   auto x24{ 27 }; // type is std::initializer_list<int>, value: { 27 }
//
// These declarations all compile, but they don't have the same meaning as the
// ones they replace. The first two statements do, indeed, declare a variable 
// of type int with value 27. The second two, however, declare a variable of
// type std::initializer_list<int> containing a single element with value 27!  
// This is due to a special type deduction rule for auto. When the initializer
// for an auto-declared variable is enclosed in braces, the deduced type is a
// std::initializer_list. If such a type can't be deduced (e.g. because the values
// in the braced initializer are of different types), the code will be rejected:
//
//  auto x5 = { 1, 2, 3.0 };  // error! can't deduce T for std::initializer_list<T>
//
//  As the comment indicates, type deduction will fail in this case, but it's 
//  important to recognize that there are actually two kinds of deduction taking
//  place. One kind stems from the use of auto: x5's type has to be deduced.
//  Because x5's initializer is in braces, x5 must be deduced to be a 
//  std::initializer_list. But std::initializer_list is a template. Instantiations
//  are std::initializer_list<T> for some type T, and that means that T's type
//  must also be deduced. Such deduction fails under the purview of the second kind
//  of deduction occurring here: template type deduction. In this example, that
//  deduction fails, because the values in the braced initializer don't have single 
//  type. 
//
//  The treatment of braced initializers is the only way in which auto type deduction
//  and template type deduction differ. When an auto-declared variable is initialized
//  with a braced initializer, the deduced type is an instantiation of 
//  std::initializer_list. But if the corresponding template is passed the same
//  initializer list, type deduction fails, and the code is rejected:
//
      auto xx = { 11, 23, 9 }; // xx's type is std::initializer_list<int>
      
      template<typename T>     // template with parameter declaration equivalent to 
      void templ_func(T param) {        // x's declaration
        // do something
      };

   // templ_func({ 11 23, 9 });         // error! can't deduce type for T

//  However, if you specify in the template that param is std::initializer_list<T>
//  for some unknown T, template type deduction will deduce what T is:

     template<typename T>
     void templ_func_with_init_list(std::initializer_list<T> initList) {
       // do something
     };

// so the only real difference between auto and template type deduction is that auto assumes
// that a braced initializer represents a std::initializer_list, but template deduction
// does not. 
//
// There does not seem to be convincing explanation why auto type deduction has a special
// rule for braced initializers..
//
// One must remember that if one declares a variable using auto and initializes it with a
// braced initializer, the deduced type will always be std::initializer_list. A classic
// mistake in C++11 programming is accidentally declaring std::initializer_list variable
// when one means to declare something else. This pitfall is one of the reasons some
// developers put braces around their initializers only when they have to.
//
// For C++11 this is the full story, but for C++14 the tale continues. C++14 permits auto
// to indicate that a function's return type should be deduced, and C++14 lambdas may
// use auto in parameter declarations. However, these uses of auto employ 
// template type deduction, not auto type deduction. So a function with an auto return
// type that returns a braced initializer won't compile:
//
//   auto createInitList()
//   {
//      return { 1, 2, 3 };       // error: can't deduce type for { 1, 2, 3 }
//   }
//
// The same is true when auto is used in a parameter type specification in C++14 lambda:
//
//   std::vector<int> v;
//   ...
//
//   auto resetV = 
//     [&v](const auto& newValue) { v = newValue; }     // C++14
//   ...
//   resetV({ 1, 2, 3 });    // error! can't deduce type for { 1, 2, 3 }
//
//
//  Things to remember:
//
//  * auto type deduction is usually the same as template type deduction, but auto
//  type deduction assumes that a braced initializer represents a std::initializer_list,
//  and template type deduction does not.
//
//  * auto in a function return type or a lambda parameter implies template type
//  deduction, not auto type deduction



int main(const int argc, const char* argv[]) 
{

   func_for_x(27);                       // conceptual call: param's
                                         // deduced type is x's type

   func_for_cx(x);                       // conceptual call: param's 
                                         // deduced type is cx's type

   func_for_rx(x);                       // conceptual call: param's
                                         // deduced type is rx's type

   
     templ_func_with_init_list({ 11, 23, 9 });         // T deduced as int, and initList's type is
                                                       // std::initializer_list<int>

   return 0;
}
