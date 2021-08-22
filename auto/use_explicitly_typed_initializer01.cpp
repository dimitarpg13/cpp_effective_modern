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

// Sometimes auto's type deduction will deliver a type which is not what we need.
// For example, suppose we have a function that takes a Widget and returns
// std::vector<bool> where each bool indicates whether the Widget offers a 
// particular feature:
//
struct Widget {
  // some initialization
};

std::vector<bool> features(const Widget& w) {
   return { true, true, false, true, false, true };
};

bool processWidget(const Widget& w, bool highPriority) {
   return true;
};

// further suppose that bit 5 indicates whether the Widget has high priority. 
// We can write code like this:
//
Widget w;

bool highPriority = features(w)[5]; // is w high priority?

bool res = processWidget(w, highPriority); // process w in accord with its priority

// There is nothing wrong with this code. It will work fine. But changing bool to 
// auto will produce undefined behavior despite the fact that it will still compile:

auto highPriority2 = features(w)[5];

// auto res2 = processWidget(w, highPriority2);

// The above call (commented) to processWidget now has undefined behavior.
// In the code using auto, the type of highPriority2 is no longer bool. 
// Though std::vector<bool> conceptually holds bools, operator[] for std::vector<bool>
// does not return a reference to an element of the container (which is what
// std::vector::operator[] returns for every type except bool. Instead, it 
// returns an object of type std::vector<bool>::reference which is a class
// nested inside std::vector<bool>.
//
// std::vector<bool>::reference exists because std::vector<bool> is specified
// to represent its bools in a packed form, one bit per bool. That creaes a 
// problem for std::vector<bool>'s operator[], because operator[] for std::vector<T>
// is supposed to return a T&, but C++ forbids references to bits. Not being able
// to return a bool&, operator[] for std::vectgor<bool> returns an object that acts
// like a bool&. For this act to succeed, std::vector<bool>::reference objects must
// be useable in essentially all contexts where bool& can be. Among the features in
// std::vector<bool>::reference that make this work is an implicit conversion to 
// bool. Note: not to bool&, to bool! To explain the full set of techniques used by
// std::vector<bool>::reference to emulate the behavior of a bool& would take us too
// far astray so we will note only that such implicit conversion is only one element
// of larger strategy.
//
// With this info in mind let us look again at this part of the original code:
//
// bool highPriority = features(w)[5];    // declare highPriority's type explicitly
//
// Here features returns a std::vector<bool> object, on which operator[] is invoked.
// operator[] returns a std::vector<bool>::reference object, which is then implicitly
// converted to the bool that is needed to initialize highPriority. highPriority
// thus ends up with the value of bit 5 in the std::vector<bool> returned by 
// features just like it is supposed to.
//
// Contrast that with what happens in the auto-ized declaration for highPriority:
//
//   auto highPriority = features(w)[5]; // deduce highPriority's type
//
// Again, features return a std::vector<bool> object and again operator[] is invoked 
// on it. operator[] continues to return a std::vector<bool>::reference object, but
// now there is a change, because auto deduces that as the type of highPriority.
// highPriority doesn't have the value of bit 5 of the std::vector<bool> returned
// by features at all.
//
// The value it does have depends on how std::vector<bool>::reference is implemented.
// One implementation is for such objects to contain a pointer to the machine word
// holding the reference bit, plus the offset into that word for that bit. Consider
// what that means for the initialization of highPriority, assuming that such 
// std::vector<bool>::reference implementation is in place.
//
// The call to `features` returns a temporary std::vector<bool> object. This object 
// has no name, but for purposes of this discussion we will cal it `temp`. operator[]
// is invoked on `temp`, and the std::vector<bool>::reference it returns contains a 
// pointer to a word in the data structure holding the bits that are managed by temp,
// plus the offset into that word corresponding to bit 5. highPriority is a copy of 
// this std::vector<bool>::reference object, so highPriority too contains a pointer
// to a word in `temp`, plus the offset corresponding to bit 5. At the end of this
// statement `temp` is destroyed, because it's a temporary object. Therefore,
// highPriority contains a dangling pointer and that's the cause of the undefined 
// behavior in the call to processWidget:
//
//    processWidget(w, highPriority);    // undefined behavior!
//                                       // highPriority contains dangling pointer!
//
// std::vector<bool>::reference is an example of a proxy class: a class that exists
// for the purpose of emulating and augmenting the behavior of some other type. 
// Proxy classes are employed for a variety of purposes. std::vector<bool>::reference
// exists to offer the illusion that operator[] for std::vector<bool> returns a 
// reference to a bit, for example, and the standard library's smart pointer types
// are proxy classes that graft resource management onto raw pointers. The utility
// of proxy classes is well-established. In fact, the design pattern "Proxy" is one
// of the most longstanding software design patterns.
//
// Some proxy classes are designed to be apparent to clients. That's the case for
// std::shared_ptr and std::unique_ptr for example. Other proxy classes are 
// designed to act more or less invisibly, std::vector<bool>::reference is an 
// example of such "invisible" proxies, as is its std::bitset compatriot, 
// std::bitset::reference.
//
// Also in that camp are some classes in C++ libraries employing a technique known
// as _expression templates_. Such libraries were originally developed to improve
// the efficiency of numeric code. Given a class Matrix and Matrix objects m1, 
// m2, m3, and m4 the expression
//
//   Matrix sum = m1 + m2 + m3 + m4;
//
// can be computed much more efficiently if operator+ for Matrix objects returns a
// proxy for the result instead of the result itself. That is, operator+ for two
// Matrix objects would return an object of a proxy class such as Sum<Matrix, Matrix>
// instead of a Matrix object. As was the case with std::vector<bool>::reference
// and bool, there'd be an implicit conversion from the proxy class to Matrix which
// would permit initialization of sum from the proxy object produced by the 
// expression on the right side of the "=". 
//
// As a general rule, "invisible" proxy classes don't play well with `auto`. Objects 
// of such classes are often not designed to live longer than a single statement, so
// creating variables of those types tends to violate fundamental library design
// assumptions.
//
// You therefore want to avoid a code in this form:
//
//   auto someVar = expression of "invisible" proxy class type;
//
// But how can you recognize when proxy objects are in use? The sw employing them 
// is unlikely to advertise their existence. They are supposed to be invisible,
// at least conceptually. And once you've found them, do you really have to 
// abandon auto and the many advantages of it?
//
// Let's answer the how-do-you-find-them question first. Although "invisible"
// proxy classes are designed to fly beneath programmer radar in day-to-day use
// libraries using them often do so. 
//
// Where documentation comes up short header files fill the gap. It is rarely 
// possible for source code to fully cloak proxy objects. They are typically
// returned from functions that clients are expected to call, so function
// signatures usually reflect their existence. Here's the spec for 
// std::vector<bool>::operator[] for example:
//
//   namespace std {  // from C++ standards
//      template <class Allocator>
//      class vector<bool, Allocator> {
//      public:
//        ...
//        class reference { ... };
//
//        reference operator[](size_type n);
//
//        ...
//
//      };
//
//   }  
//
// Assuming you know that operator[] for std::vector<T> normally returns a T&, the
// unconventional return type for operator[] in this case is a tip-off that a 
// proxy class is in use. Once auto has been determined to be deducing the type
// of a proxy class instead of the type being proxied, the solution need not 
// involve abandoning auto. The problem is that auto isn't deducing the type you
// want it to deduce. The solution is to force a different type deduction. For 
// the purpose we use _the explicitly typed initializer idiom_.
//
// The explicitly typed initializer idiom involves declaring a variable with auto,
// but casting the initialization expression to the type you want auto to deduce.
// Here's how it can be used to force highPriority to be a bool, for example:
//
//   auto highPriority = static_cast<bool>(features(w)[5]);
//
// Here, features(w)[5] continues to return a std::vector<bool>::reference object, 
// just as it always has, but the cast changes the type of the expression to bool,
// which auto then deduces as the type for highPriority. At runtime, the
//  std::vector<bool>::reference object return from std::vector<bool>::operator[]
//  executes the conversion to bool that it supports, and as part of that conversion
//  the still-valid pointer to the std::vector<bool> returned from features is 
//  dereferenced. That avoids the undefined behavior we ran into earlier. The index 5
//  is then applied to the bits pointed to by the pointer, and the bool value that
//  emerges is used to initialize highPriority. 
//
//  For the Matrix example. the explicitly typed initializer idion would look like 
//  this:
//
//    auto sum = static_cast<Matrix>(m1 + m2 + m3 + m4);
//
//  Applications of the idiom aren't limited to initializers yielding proxy class
//  types. It can also be useful to emphasize that you are deliberately creating a 
//  variable of a type that is different from the initializing expression. For
//  example suppose you have a function to calculate some tolerance value:
//
//    double calcEpsilon();    // return tolerance value
//
//  calcEpsilon clearly returns a double , but suppose you know that for your 
//  application, the precision of a float is adequate, and you care about the 
//  difference in size between floats and doubles. You could declare a float 
//  variable to store the result of calcEpsilon,
//
//   float ep = calcEpsilon();  // implicitly convert double -> float
//
//  A declaration using the explicitly typed initializer idiom, however,
//  is clear:
//
//   auto ep = static_cast<float>(calcEpsilon());
//
//  Similar reasoning applies if you have a floating-point expression that you
//  are delibierately storing as an integer value. Suppose you need to calculate 
//  the index of an element in a container with random access iterators (i.e.
//   std::vector, std::deque, or std::array) and you are given a double between
//   0.0 and 1.0 indicating how far from the beginning of the container the 
//   desired element is located. (0.5 would indicate the middle of the container).
//   Further suppose that you are confident that the resulting index will fit in
//   an int. If the container is c and the double is d, you could calculate the 
//   index this way,
//
//    int index = d * c.size();
//
//  but this obscures the fact that you're intentionally converting the double on
//  the right to int. The explicitly typed initializer idiom makes things 
//  transparent:
//
//    auto index = static_cast<int>(d * c.size())
//
//  Things to remember:
//
//  * "Invisible" proxy types can cause auto to deduce the "wrong" type for an
//  initializing expression
//
//  * The explicitly typed initializer idiom forces auto to deduce the type 
//  you want it to have
//


int main(const int argc, const char* argv[]) 
{

    return 0;
}
