#include <iostream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <assert.h>

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
//

int main(const int argc, const char* argv[]) 
{

   return 0;
}
