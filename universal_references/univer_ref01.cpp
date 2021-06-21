#include <iostream>
#include <utility>
#include <assert.h>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <memory>


struct SomeDataStructure {
  std::string names[10];
  float numbers[10];
};

class WidgetWithRValueRef {
public:
    WidgetWithRValueRef(WidgetWithRValueRef&& rhs)    // rhs is rvalue reference
        : name(std::move(rhs.name)),
          p(std::move(rhs.p))
    {
      // initialize something else
    }
    WidgetWithRValueRef(std::string&& newName, std::shared_ptr<SomeDataStructure>&& newData) :
        name(std::move(newName)), p(std::move(newData)) 
    {
      // initialize something else
    }
private:
    std::string name;
    std::shared_ptr<SomeDataStructure> p;
};


class WidgetWithUniversalRef {
public:
    template<typename T, typename D>
    WidgetWithUniversalRef(T&& newName, D&& newData) :
        name(std::forward<T>(newName)), data(std::forward<T>(newData)) 
    { 
        // initialize something else
    }
    template<typename T>
    void setName(T&& newName)  // newName is unversal reference 
    { this->name = std::forward<T>(newName); }
    template<typename D>
    void setData(D&& newData)
         { this->p = std::forward<D>(newData); } 
       private:
          std::string name;
          std::shared_ptr<SomeDataStructure> p;
};

int main (int argc, const char* argv[]) {

    {
       // 1. if a function template parameter has type T&& for a deduced
       // type T, or if an object is declared using auto&&, the parameter
       // of object is a universal reference.
       // 2. if the form of the type declaration isn't precisely type&&, 
       // or if type deduction does not occur, type&& denotes rvalue reference.
       // 3. universal references correspond to rvalue references if they are
       // initialized with rvalues. They correspond to lvalue references if
       // they are initialized with lvalues.
   
       auto timeFUncInvocation =
           [](auto&& func, auto&&... params)
           {
               clock_t startTime = clock();
               std::forward<decltype(func)>(func)(
                 std::forward<decltype(params)>(params)...
                );
               testTime = clock();
               timePassed = startTime - testTime;
               secondsPassed = timePassed / (double)CLOCKS_PER_SEC;
   
           };
    }

    {
       // Rvalue references bind only to objects that are candidates for
       // moving. If you have an rvalue reference parameter, you know that
       // the object it's bound to may be moved:
       //
       // class Widget {
       //    Widget(Widget&& rhs);
       //    ...
       // };
       //
       // That being the case, such objects should be passed to other functions
       // in a way that permits those functions to take advantage of the object's
       // rvalueness. The way to do that is to cast parameters bound to such 
       // objects to rvalues. For this purpose we use std::move.
       SomeDataStructure* data = new SomeDataStructure();




    }

    {

    }

    return 0;
}
