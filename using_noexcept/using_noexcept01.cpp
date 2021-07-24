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
// By default, all memory deallocation functions and all destructors - both user-
// defined and compiler-generated - are implicitly noexcept. Thus there is no need
// to declare them noexcept. The only time a destructor is not implicitly noexcept
// is when a data member of the class (including inheritted members and those
// contained inside other data members) is that of a type that expressly states
// that its destructor may emit exceptions (e.g. declares noexcept(false) ). Such
// destructors are uncommon. There are none in the StdLib and if the destructor for
// an object being used by the StdLib (e.g. because it is in a container or was 
// passed to an algorithm) emits an exception, the behavior of the program is 
// undefined. 
//
// It is worth noting that some library interface designers distinguish functions
// with wide contracts from those with narrow contracts. A function with a wide
// contract has no preconditions. Such a function may be called regardless of the
// state of the program, and it imposes no contraints on the arguments the callers
// pass it. Functions with wide contracts never exhibit undefined behavior.
//
// Functions without wide contracts have narrow contracts. For such functions, if
// a precondition is violated, results are undefined. 
//
// If the developer is writing a function with wide contract and knows that it 
// won't emit exceptions, then it should be declared with noexcept. For functions
// with narrow contracts, the situation is trickier. For example, suppose one is
// writing a function f taking a std::string parameter, and suppose f's natural 
// implementation never yields an exception. That suggest that f should be 
// declared noexcept.
//
// Suppose that f has a precondition: the length of its std::string parameter
// does not exceed 32 chars. If f were to be called with a std::string whose 
// length is greater than 32, behavior would be undefined, because a precondition
// violation by definition results in undefined behavior. f is under no obligation
// to check this precondition, because functions may assume that their preconditions
// are satisified. Callers are responsible for ensuring that such assumptions are 
// valid. Even with a precondition declaring f noexcept seems appropriate:
//
//    void f(const std::string& s) noexcept;   // precond s.length() <= 32
//
// Suppose that f's implementer chooses to check for precondition violations.
// Checking isn't required, but it is also not forbidden, and checking the 
// precondition could be useful e.g. during system testing. Debugging an exception
// that's been thrown is generally easier than trying to track down the cause of
// undefined behavior. But how should a precondition violation be reported such
// that a test harness or client error handler could detect it? A straightforward
// approach would be to throw a "precondition was violated" exception, but if f
// is declared noexcept that would be impossible; throwing an exception would 
// lead to program termination. For this reason, library designers who distinguish
// wide from narrow contracts generally reserve noexcept for functions with wide
// contracts.
//
// Elaboraton of an another point: compilers typically offer no help in identifying
// inconsistencies between function implementations and their exception 
// specifications. Consider this code which is legal:
//
//    void setup();      // functions defined elsewhere
//    void cleanup();
//
//    void doWork() noexcept
//    {
//        setup();       // set up work to be done
//                  
//        ...            // do the actual work
//
//        cleanup();     // perform cleanup actions
//    }
//
// Here, doWork is declared noexcept, even though it calls the non-noexcept 
// functions setup and cleanup. This seems contradictory, but it could be 
// that setup and cleanup document that they never emit exceptions, even
// though they're not declared that way. There could be good reasons for
// their non-noexcept declarations. For example, they might be part of a 
// library written in C. (Even functions from the C StdLib that have been
// moved into the std namespace lack exception specifications e.g. 
// std::strlen isn't declared noexcept.) Or they could be part of a C++98
// library that decided not to use C++98 exception specifications and hasn't
// yet been revised for C++11.
//
// Because there are legitimate reasons for noexcept functions to rely on 
// code lacking the noexcept guarantee, C++ permits such code, and compilers
// generally do not issue warnings about it.
//
// Things To Remember
//
// * noexcept is part of a function's interface, and that means that callers
// may depend on it
//
// * noexcept functions are more optimizable than non-noexcept functions
//
// * noexcept is particularly valuable for the move operations, swap,
// memory deallocation functions, and destructors
//
// * most functions are exception-neutral rather than noexcept

int main(const int argc, const char* argv[]) {

   return 0;
}
