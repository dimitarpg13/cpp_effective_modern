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
