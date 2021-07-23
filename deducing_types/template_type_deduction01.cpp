#include <iostream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <assert.h>

//
// Generally the function template looks like:
//
// template<typename T>
// void f(ParamType param);
//
// a call will look like this:
// f(expr);
//
// During compilation, compilers use expr to deduce two types: one for T and one for 
// ParamType. These types are frequently different, because ParamType often contains
// arguments e.g. const or reference qualifiers. For example, if the template is 
// declared like this:
//
// template<typename T>
// void f(const T& param); 
//
// and we call like:
// int x = 0;
// f(x);
// T is deduced to be int, but ParamType is deduced to be const int&.
//
// It is natural to expect that the type deduced for T is the same as the type of the 
// argument passed to the function i.e. that T is the type of expr. In the above 
// example, that's the case: x is an int, and T is deduced to be int. But it does not
// always work this way. The type deduced for T is dependent not just of the type of 
// expt, but also on the form of ParamType. There are three cases:
//
// * ParamType is a pointer or reference type, but not an universal reference. 
// * ParamType is a universal reference
// * ParamType is neither a pointer nor a reference
//
// We therefore have three type deduction scenarios to examine. Each will be based on
// the general form for templates and calls to it:
// template<typename T>
// void f(ParamType param);
//
// f(expr);
//
// Case 1: ParamType is a reference or pointer but not a universal reference
// The simplest situation is when ParamType is a reference type of a pointer type but
// not universal reference. In that case, type deduction works like this:
// 1. if expr's type is a reference, ignore the reference part
// 2. then pattern-match expr's type against ParamType to determine T.
// 
// For example, if this is our template,
//    template<typename T>
//    void f(T& param);
//
// and we have these variable declarations,
//    int x = 27;         // x is an int
//    const int cx = x;   // cs is const int
//    const int& rx = x;  // rx is a reference to x as a const int
//  
// the deduced types for param and T in various calls are as follows:
//    
//    f(x);    // T is int, param's type is int&
//    f(cx);   // T is const int,
//             // param's type is const int&
//    f(rx);   // T is const int,
//             // param's type is const int&
//
// if we change the type of f's parameter from T& to const T&, things change a little,
// but not in any really surprising ways. The const-ness of cx and rx continues to be
// respected, but because we are now assuming that param is a reference-to-const, there
// is no longer a need for const to be deduced as part of T:
//
//    template<typename T>
//    void f(const T& param); 
//
//    int x = 27;
//    const int cx = x;
//    const int& rx = x;
//
//    f(x); // T is int, param's type is const int&
//
//    f(cx); // T is int, param's type is const int&
//
//    f(rx); // T is int, param's type is const int&
//
// As before, rx's reference-ness is ignored during type deduction.
// If param were a pointer (or a pointer to const) instead of a reference,
// things would work essentially the same way:
//
//    template<typename T>
//    void f(T* param);
//
//    int x = 27;
//    const int *px = &x;
//    f(&x); // T is int, param's type is int*
//    f(px); // T is const int,
//           // param's type is const int*

template<typename T>
void f_ref(T& param) {
   assert(std::is_pointer<T>::value==false);
   assert(std::is_reference<T>::value==false);
   std::cout << "T is const: " << std::is_const<T>::value << std::endl;
};


template<typename T>
void f_const_ref(const T& param) {
  assert(std::is_pointer<T>::value==false);
  assert(std::is_reference<T>::value==false);
  assert(std::is_const<T>::value==false); 
};


template<typename T>
void f_ptr(T* param) {
  assert(std::is_pointer<T>::value==false);
  assert(std::is_reference<T>::value==false);
  std::cout << "T is const: " << std::is_const<T>::value << std::endl;
};


int main(const int argc, const char* argv[]) 
{
   int x = 27;
   const int cx = x;
   const int& rx = x;
   const int *px = &x;
   
   f_ref(x);
   f_ref(cx);
   f_ref(rx);

   f_const_ref(x);
   f_const_ref(cx);
   f_const_ref(rx);

   f_ptr(&x);
   f_ptr(px);

   return 0;
}
