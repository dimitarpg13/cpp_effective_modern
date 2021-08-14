#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <typeinfo>
#include <type_traits>
#include <assert.h>
#include <array>
#include <initializer_list>
#include <deque>
#include <memory>
#include <functional>
#include <vector>
#include <unordered_map>

// Intro:
// auto is simple but at the same time it is more subtle than it looks.
// Using it saves typing but also can obstruct correctness and lead
// to subtle performance issues related to manual type declarations.
// Furthermore, some of auto's type deduction results, while dutigully
// conforming to the prescribed algorithm, are, from the perspective
// of the programmer, just wrong. When this is the case it is important
// to know how to guide auto to the right answer, because falling back
// on manual type declarations is an alternative that's often best 
// avoided.
//
// Prefer auto to explicit type declarations
//
// Let's declare a local variable which will be initialized by dereferncing
// an iterator:
//
template<typename It>
void dwim(It b, It e)
{
  while (b != e) {
    typename std::iterator_traits<It>::value_type 
      currValue = *b;
  }
};

// and with using auto:
//
template<typename It>
void dwim2(It b, It e)
{
  while (b != e) {
    auto currValue = *b;
  }
};

// and because auto uses type deduction it can represent types known only
// to compilers

struct Widget {
  int i;
  bool operator<(const Widget& other) {
    return i < other.i;
  }
};

auto derefUPLess =                        // comparison func
  [](const std::unique_ptr<Widget>& p1,   // for Widgets
     const std::unique_ptr<Widget>& p2)   // pointed to by std::unique_ptr's
  { return *p1 < *p2; };

// In C++14 we can introduce parameters to lambda function which types involve auto:

auto derefLess =                          // C++14 comparison  
  [](const auto& p1, const auto& p2)      // function for values pointed
  { return *p1 < *p2; };                  // to by anything pointer-like

// we do not really need auto to declate a variable that holds closure because we
// can use a std::function object. But what is a std::function object?
//
// std::function is a template in the C++11 stdlib that generalizes the idea of a
// function pointer. Whereas function pointers can point only to functions, 
// std::function objets can refer to any callable object i.e. anything that can be
// invoked like a function. Just as you must specify the type of function to point
// to when you create a function pointer (i.e. the signature of the functions), you
// must specify the type of the function to refer to when you create a std::function
// object. You do that through std::function's template parameter. For example, to
// declare a std::function object named func that could refer to any callable object 
// acting as if it had this signature:
// 
// bool( const std::unique_ptr<Widget>&,  // C++11 signature for std::unique_ptr<Widget>
//       const std::unique_ptr<Widget>&)  // std::unique_ptr<Widget> comparison function
// 
// you'd write this:
//
std::function<bool(const std::unique_ptr<Widget>&,
                   const std::unique_ptr<Widget>&)> func;

// Because lambda expressions yield callable objects, closures can be stored in 
// std::function objects. This means we could declare C++11 version of derefUPLess
// without using auto as follows:

std::function<bool(const std::unique_ptr<Widget>&,
                   const std::unique_ptr<Widget>&)>
  derefUPLess2 = [](const std::unique_ptr<Widget>& p1,
                    const std::unique_ptr<Widget>& p2)
                   { return *p1 < *p2; };

// it is important to recognize that even setting aside the syntactic verbosity and 
// need to repeat the parameter types, using std::function is not the same as 
// using auto. An auto-declared variable holding a closure has the same type as the
// closure and as such it uses only as much memory as the closure requires. The type
// of std::function-declared variable holding a closure is an instantiation of the
// std::function template and that has a fixed size for any given signature. 
// This size may not be adequate for the closure it's asked to store and when that is
// the case the std::function constructor will allocate heap memory to store the 
// closure. The result is that the std::function object typically uses more memory 
// than the auto-declared object. Due to the implementation details that restrict 
// inlining and yield indirect function calls, invoking a closure via a std::function
// object is almost certain to be slower than calling it via an auto-declared object.
// In other words, the std:function approach is generally bigger and slower than the
// auto approach, and it may yield out-of-memory exceptions too. So if one have to 
// choose between std::function and auto for holding a closure it is quite obvious
// that auto is the better choice. A similar argument can be made for auto over
// std::function for holding the result of calls to std::bind.
//
// The advantages of auto extend beyond the avoidance of uninitialized variables, 
// verbose variable declarations, and the ability to directly hold closures. One is
// the ability to avoid problems related to "type shortcuts". Here is an example:
//
std::vector<int> v;
// ...
unsigned sz = v.size();
//
// The official return type of v.size() is std::vector<int>::size_type which is 
// specified to be an unsigned integral type. On 32-bit Windows for example, both
// unsigned and std::vector<int>::size_type are the same size but on 64-bit 
// unsigned is 32 bits while std::vector<int>::size_type is 64 bits. This means
// portability issues when porting to 64-bits a code which works correctly 
// under 32-bits.  

// Using auto solves this issue:
//
auto sz_new = v.size();

// Another example where using auto is expedient
//
//std::unordered_map<std::string, int> m;

//for (const std::pair<std::string, int>& p : m)
//{
//   // do something with p
//}

// The problem here is that the key part of std::unordered_map is const, so the
// type of pair in the hash table isn't std::pair<std::string, int>, it is 
// std::pair<const std::string, int>. The compilers will strive to find a way
// to convert std::pair<const std::string, int> objects to std::pair<std::string, int>.
// They may succeed by creating a temporary object of the type that p wants to bind to
// by copying each object in m, then binding the reference p to that temp object.
// At the end of each loop iteration, the temp object would be destroyed. 
//
// Such unintentional mismatches can be auto-ed away:
//
//for (const auto& p : m)
//{
   // do something with p
//}


int main(const int argc, const char* argv[]) 
{
   // Another example where using auto is expedient
   //
   std::unordered_map<std::string, int> m = { {"Dimitar", 1}, {"Mieko", 2} };

   // creates a temporary object of the type that p wants to bind to
   // by copying each object in m, then binding the reference p to that temp object.
   // At the end of each loop iteration, the temp object would be destroyed.
   for (const std::pair<std::string, int>& p : m)
   {
       // do something with p
       std::cout << "p.first=" << p.first << ", p.second=" << p.second << std::endl;
   };

   // Such unintentional mismatches can be auto-ed away:
   //
   for (const auto& p : m)
   {
       // do something with p
       std::cout << "p.first=" << p.first << ", p.second=" << p.second << std::endl;
   };

   return 0;
}
