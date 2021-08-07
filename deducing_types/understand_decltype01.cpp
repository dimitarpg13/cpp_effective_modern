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
// In C++11, the primary use of decltype is for declaring function templates
// where the function's return type depends on its parameter types. For example
// suppose we'd like to write a function that takes a container that supports
// indexing via square brackets plus an index, then authenticates the user
// before returning the result of the indexing operation. The return type
// of the function should be the same as the type returned by the indexing 
// operation.
//
// operator[] on a container of objects of type T typically returns a T&. 
// This is the case for std::deque, and it is almost always true for 
// std::vector. For std::vector<bool>, however, operator[] does not return
// a bool&. Instead, it returns a brand new object. So the type returned by the 
// container's operator[] depends on the container.
//
// decltype makes it easy to express that. Here's a first cut at the template
// we'd like to write, showing the use of decltype to compute the return type.
// The template needs a bit of refinement but we will defer that for now:
//
   void authenticateUser() {
       // do some authentication
   };
//
   template<typename Container, typename Index>      // works but
   auto authAndAccess1(Container& c, Index i)         // it requires 
     -> decltype(c[i])                               // refinement
   {
      authenticateUser();
      return c[i];
   };   
//
// The use of auto before the function name has nothing to do with type 
// deduction. Rather, it indicates that C++11's trailing return type 
// syntax is being used, i.e. that the function's return type will be
// declared following the parameter list (after the "->"). A trailing
// return type has the advantage that the function's parameters can be
// used in the specification of the return type. In authAndAccess, for
// example, we specify the return type using c and i. If we were to 
// have the return type precede the function name in the conventional 
// fashion, c and i would be unavailable, because they would not have
// been declared yet.
//
// With this declaration, authAndAccess returns whatever type operator[]
// returns when applied to the passed-in container, exactly as we desire.
//
// C++11 permits return types for single-statement lambdas to be deduced,
// and C++14 extends this to both all lambdas and all functions, including
// those with multiple statements. In the case of authAndAccess that means 
// that in C++14 we can omit the trailing return type, leaving just the 
// leadning auto. With that form of declaration, auto does mean that 
// type deduction will take place. In particular, it means that compilers
// will deduce the function's return type from function's implementation:
//
   template<typename Container, typename Index>       // C++14
   auto authAndAccess2(Container& c, Index i)         // not quite correct
   {
      authenticateUser();
      return c[i];            // return type deduced from c[i]
   }
//
// For functions with an auto return type specification, compilers employ
// template type deduction. In this case that's problematic. As we've 
// discussed operator[] for most containers-of-T returns T& but as we 
// already have seen the reference-ness of an initializing expression is
// ignored. Consider what that means for this client code:
//
//   std::deque<int> d;
//   ...
//   authAndAccess(d, 5) = 10;  // authenticate user, return d[5],
//                              // then assign 10 to it;
//                              // this won't compile!
//
// Here, d[5] returns an int&, but auto return type deduction for authAndAccess
// will strip off the reference, thus yielding a return type of int. That int,
// being the return value of a function, is an rvalue so the code won't compile.
//
// To get authAndAccess to ork as we'd like, we need to use decltype type deduction
// for its return type i.e. to specify that authAndAccess should return exactly the
// same type that the expression c[i] returns. The need to use decltype type deduction
// rules in some cases where types are inferred were anticipated and made possible
// in C++14 through the decltype(auto) specifier. What may initially seems 
// contradictory (decltype and auto) actually makes perfect sense: auto specifies 
// that the type is to be deduced, and decltype says that decltype rules should be
// used during deduction. We can thus write authAndAccess like this:
//
   template<typename Container, typename Index>       // C++14; works,
   decltype(auto)                                     // but still
   authAndAccess3(Container& c, Index i)              // requires
   {                                                  // refinement
      authenticateUser();
      return c[i];
   }; 
//
// Now authAndAccess will truly return whatever c[i] returns. In particular, for the
// common case where c[i] returns a T&, authAndAccess will also return a T&, and in
// the uncommon case where c[i] returns an object, authAndAccess will return an
// object too. 
//
// The use of decltype(auto) is not limited to function return types. It can also be
// convenient for declaring variables when you want to apply the decltype type 
// deduction rules to the initializing expression:
//
     struct Widget {
       int i;
     };
     
     Widget w;

     const Widget& cw = w;

     auto myWidget1 = cw;    // auto type deduction: myWidget1's type is Widgest

     decltype(auto) myWidget2 = cw;   // decltype type deduction:
                                      // myWidget2's type is const Widget&

// Let us look again at authAndAccess3 - the container is passed by 
// lvalue-reference-to-non-const, because returning a reference to an element
// of the container permits clients to modify the container. But this means
// it is not possible to pass rvalue containers to this function. Rvalues
// can't bind to lvalue references (unless they're lvalue-references-to-const
// which is not the case here).
//
// Admittedly, 

int main(const int argc, const char* argv[]) 
{


   return 0;
}