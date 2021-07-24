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
// is declared noexcept. The checking is typically rather roundabout. Functions like
// std::vector::push_back call std::move_if_noexcept, a variation of std::move that
// conditionally casts to an rvalue depending on whether the type's move ctor is
// noexcept. In turn, std::move_if_noexcept consults std::is_nothrow_move_constructible,
// and the value of this type trait is set by compilers, based on whether the move
// ctor has a noexcept or throw()) designation.
//
// swap functions comprise another case where noexcept is particularly desirable.
// swap is a key component of many STL algorithm implementations and it is employed
// in copy assignment operators. Whether swaps in the StdLib are noexcept is sometimes
// dependent on whether user-defined swaps are noexcept. For example, the declarations
// for the StdLib's swaps for arrays and std::pair are:
//
//    template <class T, size_t N>
//    void swap(T (&a)[N],
//              T (&b)[N]) noexcept(noexcept(swap(*a, *b)));
//
//    template <class T1, class T2>
//    struct pair {
//      ...
//      void swap(pair& p) noexcept(noexcept(swap(first, p.first)) &&
//                                  noexcept(swap(second, p.second)));
//      ...
//    };
//
// These functions are conditionally noexcept: whether they are noexcept depends on
// whether the expressions inside the noexcept clauses are noexcept. Given two arrays
// of Widget, swapping them is noexcept only if swapping individual elements in the 
// arrays is noexcept i.e. swap for Widget is noexcept. The author of Widget's swap 
// thus determines whether swapping arrays of Widget is noexcept. That, in turn, 
// determines whether swapping arrays of Widget is noexcept. Similarly, whether
// swapping two std::pair objects containing Widgets is noexcept depends on whether
// swap for Widgets is noexcept. The fact that swapping higher-level data structures
// can generally be noexcept only if swapping their lower-level constituents is 
// noexcept motivates the app developer to offer noexcept swap functions whever 
// he/she can.
//
// A function should be declared noexcept only if the developer is willing to commit
// to a nonexcept implementation over the long term. If the developer declares a 
// function noexcept and later attempts to remove it problems will ensue. If he
// removes noexcept from the function's declaration (i.e. change in interface)
// he will be running the risk of breaking client code. He could change the
// implementation such that an exception could escape yet keep the original
// now incorrect exception specification. If he does that the program will be
// terminated if an exception tries to leave the function.
//
// Most functions are exception-neutral. Such functions throw no exceptions themselves
// but the functions they call may emit one. When that happens, the exception-neutral
// function allows the emitted exception to pass through on its way to a handler
// further up the call chain. Exception-neutral functions are never noexcept,
// because they may emit such "just passing through" exceptions. Most functions,
// therefore, quite properly lack the noexcept designation.
//
// Some functions, have natural implementations that emit no exceptions, and a few
// more - notably the move operations and swap - being noexcept can have a significant
// payoff, it's worth implementing them in a noexcept manner if at all possible.
//
// Notice the difference between implementation of functions which have natural 
// noexcept implementations and twisting a function implementation to permit a 
// noexcept declaration. If a straightforward function implementation might yield
// exceptions e.g. by invoking a function which might throw, the work which will be
// needed to hide it from the callers e.g. catching exceptions and replacing them 
// with status codes or special return values will not only complicate the function
// implementation but will complicate the code at the call sites too. For example, 
// callers may have to check for status codes or special return values. The runtime
// cost of those complications e.g. extra branches, larger functions that put more
// pressure on instruction caches could exceed any speedup we'd hoped to achieve
// via noexcept; Additionally the code will be harder to comprehend and maintain.
//
// For some functions, being noexcept is so important, that they are that way by
// default. In C++98, it was considered bad style to permit the memory deallocation 
// functions (i.e. operator delete and operator delete[]) and destructors to emit
// exceptions, and C++11, this style rule has been all but upgraded to a lang rule.
//

int main(const int argc, const char* argv[]) {

   return 0;
}
