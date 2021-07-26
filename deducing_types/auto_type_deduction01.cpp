#include <iostream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <assert.h>
#include <array>

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
// auto x = 27;           // case 3 (x is neither ptr nor reference)
//
// const auto cx = x;     // case 3 (cx isn't either)
//
// const auto& rx = x;    // case 1 (rx is a non-universal ref)
//
// Case 2 works as you'd expect:
//
// auto&& uref1 = x;      // x is int and lvalue,
//                        // so uref1's type is int&
//
// auto&& uref2 = cx;     // cx is const int and lvalue,
//                        // so uref2's type is const int&
//
// auto&& uref3 = 27;     // 27 is int and rvalue,
//                        // so uref3's type is int&&
//
// We have similar rules for decay of array and function names into 
// pointers for non-reference type specifiers:
//
// 


int main(const int argc, const char* argv[]) 
{

   func_for_x(27);                       // conceptual call: param's
                                         // deduced type is x's type

   func_for_cx(x);                       // conceptual call: param's 
                                         // deduced type is cx's type

   func_for_rx(x);                       // conceptual call: param's
                                         // deduced type is rx's type
   return 0;
}
