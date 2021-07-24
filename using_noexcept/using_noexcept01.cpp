#include <iostream>
#include <string>
#include <utility>
#include <memory>
#include <array>
#include <assert.h>
#include <type_traits>

// During the work on C++11, a consensus emerged that the truly meaningful information
// about a function exception-emitting behavior was whether it had any. Black or white
// either a function might emit an exception or it guaranteed it would not. This 
// maybe-or-never dichotomy forms the basis of the C++11's exception specification
// which essentially replaces C++98. C++98 style exceptions remain valid but they are
// deprecated. In C++11, unconditional noexcept is for functions that guarantee that
// they won't emit exceptions.
//
// Whether a function should be declared is a matter of interface design. The exception-
// emitting behavior of a function is of key interest to clients. Callers can query
// a function's noexcept status, and the results of such query can affect the exception
// safety or efficiency of the calling code. Whether a function is noexcept is as 
// important piece of information as whether a member function is const. 
//
// There is an additional incentive to apply noexcept to functions that won't produce
// exceptions: it permits compilers to generate better object code. To understand why,
// let us examine the differences between the C++98 and C++11 ways of saying that a 
// function won't emit exceptions. Consider a function f that promises callers
// they will never receive an exception. The two ways of expressing that are:
//
//    int f(int x) throw();    // no exceptions from f: C++98 style
//    int f(int x) noexcept;   // no exceptions from f: C++11 style
//
// If, at runtime, an exception leaves f, f's exception specification is violated.
// With the C++98 exception specification, the call stack is unwound to f's caller,
// and after some actions not relevant here, program execution is terminated. With
// the C++11 exception specification, runtime behavior is slightly different: the 
// stack is only possibly unwound before program execution is terminated.
//
// The difference between unwinding the call stack and possibly unwinding it has
// a surprisingly large impact on code generation. In a noexcept function, optimizers
// need not keep the runtime stack in an unwindable state if an exception would
// propagate out of the function, nor must they ensure that objects in an noexcept
// are destroyed in the inverse order of construction should an exception leave the
// function. Functions with throw() exception specifications lack such optimization 
// flexibility, as do functions with no exception specification at all. This can be
// summarized as:
//
//    RetType function(params) noexcept;   // most optimizable
//    RetType function(params) throw();    // less optimizable
//    RetType function(params);            // less optimizable
//
// This alone is a sufficient reason to declare functions noexcept whenver you know
// they won't produce exceptions.
//
// For some functions, the case is even stronger. The move operations are the pre-
// eminent example. Suppose you have a C++98 code base making use of a 
// std::vector<Widget>. Widgets are added to the std::vector via push_back:
//
//    std::vector<Widget> vw;
//    ...
//    Widget w;
//    ...                // work with w
//    vw.push_back(w);   // add w to vw
//    ...
//
// Assume this code works fine, and you have no interest in modifying it for C++11.
// However, you do want to take advantage of the fact that C++11's move semantics
// can improve the performance of legacy code when move-enabled types are involved.
// You therefore ensure that Widget has move operations, either by writing them
// yourself or by seeing to it that the conditions for their automatic generations 
// are fulfilled. 
//
// std::vector::push_back takes advantage of the "move if you can, copy if you must"
// strategy and it is not the only function in the Stdlib that does. Other functions
// supporting the strong exception guarantee in C++98 such as std::vector::reserve,
// std::deque::insert behave in the same way. All these functions replace calls to 
// copy operations in C++98 with calls to move operations in C++11 only if the move
// are known not to emit exceptions. But how can a function know if a move operation
// won't produce exception? The answer is obvious: it checks to see if the operation
// is declared noexcept.
//
//

int main(const int argc, const char* argv[]) {

   return 0;
}
