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
// function won't emit exceptions.  

int main(const int argc, const char* argv[]) {

   return 0;
}
