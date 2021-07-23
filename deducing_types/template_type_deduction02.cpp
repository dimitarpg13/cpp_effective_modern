#include <iostream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <assert.h>
#include <array>

// Array Arguments
//
// This is special niche with its own rules for template types deduction which is 
// is worth special mention. Array types are different from pointer types, even though
// that sometimes they seem interchangeable. A primary contributor to this illusion is 
// that, in many contexts, an array decays into a pointer to its first element. This
// decay is what permits code like this to compile:
//
//    const char name[] = "J. P. Briggs";  // name's type is const char[13]
//
//    const char * ptrToName = name;  // array decays to pointer
//
// Here, the const char* pointer ptrToName is being initialized with name, which is a 
// const char[13]. These types (const char* and const char[13]) are not the same, but
// because of the array-tp-pointer decay rule, the code compiles.
//
// But what if an array is passed to a template taking a by-value parameter?
// What happens then?
//
//    template<typename T>
//    void f(T param);       // template with by-value parameter
//
//    f(name);  // what types are deduced for T and param?
//
// We begin with the observation that there is no such thing as a function parameter
// that is an array. Note that the syntax below is legal:
//   
//    void myFunc(int param[]);
//
// but the array declaration is treated as a pointer declaration, meaning that myFunc
// could equivalently be declared as:
//
//    void myFunc(int* param);
//
// The equivalence of array and pointer parameters is related to the C roots of C++ and
// it fosters the illusion that array and pointer types are the same.
//
// Because array parameter declarations are treated as if they were pointer parameters
// the type of an array that's passed to a template function by value is deduced to be
// a pointer type. That means that in the call to the template f, its type parameter T
// is deduced to be const char *:
//
//    f(name);  // name is array but T is deduced as const char *
//
// NOTE: Although functions can't declare parameters that are truly arrays, they can 
// declare parameters that are references to arrays! So if we modify the template f to
// take its argument by reference
//
//    template<typename T>
//    void f(T& param);   // template with by-reference parameter
//
// and we pass an array to it,
//
//    f(name);   // pass array to f
//
// the type deduced for T is the actual type of the array! That type includes the size
// of the array so in the example above T is deduced to be const char [13] and the 
// type of f's parameter (a reference to this array) is const char (&) [13].
//
// The ability to declare references to arrays enables the creation of a template
// that deduces the number of elements that an array contains:

// return size of an array as a compile-time constant. (The array parameter has no name,
// because we care only about the number of elements it contains.)
template<typename T, std::size_t N>                   // see info
constexpr std::size_t arraySize(T (&)[N]) noexcept    // below on constexpr
{                                                     // and noexcept
   return N;
}

// using constexpr makes the result available during compilation. That makes it possible
// to declare an array with the same number of elements as a second array whose size is 
// computed from a braced initializer:
//
int keyVals[] = { 1, 3, 7, 9, 11, 22, 35 };   // keyVals has 7 elements

int mappedVals[arraySize(keyVals)];  // and so does mappedVals

// alternatively we can use std::array
//
std::array<int, arraySize(keyVals)> mappedVals2; // mappedVals2' size is 7



int main(const int argc, const char* argv[]) 
{


   return 0;
}
