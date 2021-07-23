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
//
//  Case 2: ParamType is Universal Reference
//  Universal reference parameters are declared like rvalue references but they 
//  behave differently when lvalue arguments are passed in. 
//  * if expr is an lvalue, both T and ParamType are deduced to be lvalue references.
//  That's doubly unusual. First, it is the only situation in template type deduction
//  where T is deduced to be a reference. Second, although ParamType is declared
//  using the syntax for an rvalue reference, its deduced type is an lvalue reference.
//  * if expr is an rvalue the "normal" (Case 1) rules apply
//
//  For example:
//    template<typename T>
//    void f(T&& param);  // param is now a universal reference
//
//    int x = 27; 
//    const int cx = x;
//    const int& rx = x;
//    f(x);  // x is lvalue so T is int&,
//           // param's type is also int&
//
//    f(cx); // cx is lvalue, so T is const int&
//           // param's type is also const int&
//
//    f(rx); // rx is lvalue, so T is const int&,
//           // param's type is also const int&
//
//    f(27); // 27 is rvalue, so T is int,
//           // param's type is therefore int&&
//
//
//  Case 3: ParamType Is Neither A Pointer Nor A Reference
//
//  When ParamType is neither a pointer nor a reference, we are dealing with pass-by-value:
//
//    template<typename T>
//    void f(T param);   // param is no passed by value
//  
//  This means that param will be a copy of whatever is passed in - completely new object.
//  The fact that param will be a new object motivates the rules that govern how T is
//  deduced from expr:
//
//    1. As before, if expr's type is a reference, ignore the reference part
//    2. If, after agnoring expr's reference-ness, expr is const, ingore that too. If it is
//    volatile, also ingore that. 
//    Hence:
//       int x = 27;
//       const int cx = x;
//       const int& rx = x;
//
//       f(x);   // T's and param's types are both int
//       f(cx);  // T's and param's types are again both int
//       f(rx);  // T's and param's types are still both int
//
//   Note that even though cx and rx represent const values, param isn't const.
//   param is an object that's completely independent of cx and rx - a copy of 
//   cx or rx. The fact that cx and rx can't be modified says nothing about
//   wether param can be. That's why expr's const-ness (and volatile-ness if any)
//   is ignored when deducing a type for param: just because expr can't be modified
//   does not mean that a copy of it can't be.
//
//   Notice that const and volatile are ignored only for by-value parameters.
//   As we've seen, for parameters that are references-to- or pointers-to-const,
//   the const-ness of expr is preserved during type deduction. But consider the
//   case where expr is a const pointer to a const object, and expr is passed to a 
//   by-value param:
//
//      template<typename T>
//      void f(T param);     // param is still passed by value
//
//      const char* const ptr = // ptr is const pointer to const object
//         "Fun with pointers";
//
//      f(ptr);   // pass arg of type const char * const
//
//   Here, we are dealing with const ptr pointing to const char. When ptr is passed
//   to f, the bits making up the pointer are copied into param. As such, the pointer
//   itself (ptr) will be passed by value. In accord with the type deduction rule 
//   for by-value parameters, the const-ness of ptr will be ingored, and the type 
//   deduced for param will be const char* i.e. a modifiable pointer to a const char
//   sring. The const-ness of what ptr points to is preserved during type deducion,
//   but the const-ness of ptr itself is ignored when copying it to create the new
//   pointer, param. 
// 
//
//
//

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


template<typename T>
void f_const_ptr(const T* param) {
  assert(std::is_pointer<T>::value==false);
  assert(std::is_reference<T>::value==false);
  assert(std::is_const<T>::value==false);
};


template<typename T>
void f_univ_ref(T&& param) {
  assert(std::is_pointer<T>::value==false);
  std::cout << "T is reference: " << std::is_reference<T>::value << std::endl;
  std::cout << "T is const: " << std::is_const<T>::value << std::endl;
};


template<typename T>
void f_copy(T param) {
  assert(std::is_const<T>::value==false);
  assert(std::is_volatile<T>::value==false);
  std::cout << "T is reference: " << std::is_reference<T>::value << std::endl;
  std::cout << "T is pointer: " << std::is_pointer<T>::value << std::endl;
};

int main(const int argc, const char* argv[]) 
{
   int x = 27;
   const int cx = x;
   const int& rx = x;
   const int *px = &x;
   const char* const ptr = // ptr is const pointer to const object
     "Fun with pointers";

   f_ref(x);
   f_ref(cx);
   f_ref(rx);

   f_const_ref(x);
   f_const_ref(cx);
   f_const_ref(rx);

   f_ptr(&x);
   f_ptr(px);

   f_const_ptr(&x);
   f_const_ptr(px);

   f_copy(x);
   f_copy(cx);
   f_copy(rx);
   f_copy(px);
   f_copy(ptr);

   return 0;
}
