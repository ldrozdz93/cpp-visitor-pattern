# What is this?
This repo provides the building blocks for implementing the Visitor design pattern in c++, put in namespace ```vstor```.

# Simplest example:
```
#include "vstor/vstor.hpp"

using BaseChildren = vstor::VisitableListVariant<
    struct Derived1,
    struct Derived2
>;

struct Base : vstor::VisitableFor<BaseChildren> {
    virtual ~Base() = default;
};

struct Derived1 : vstor::VisitableImpl<Derived1, Base> {};

int fun(Base& base)
{
    return base.visit_by(vstor::Overloaded{
        [](Derived1&){ return 1; },
        [](Derived2&){ return 2; }
    });
}

int main()
{
    Derived1 derived{};
    return fun(derived); // returns 1
}

```

# What is "special" about ```vstor```?
- It's easy to add convenient visitation capabilities to already existing stable class hierarchies. Just update the headers.  
- User can return arbitrary types from a visitation, like with ```std::visit```, compared to the "classic" visitor pattern approach in C++.
- It's generally applicable in practice, unlike the often given advice "just refactor to ```std::vartiant```".
- A base class user does not even need to know the possibly-visited classes, if he doesn't use the visitation capabilities. It's had to achieve it with ```std::variant```.
- A base class user does not need to physically depend on the concrete derived class definitions, if those are not visited (see the full example below).


## Full example:
The idea of this library is to require the base class user to be aware of any concrete visitable classes only when visitation is used on the base class. 
That's achieved with allowing forward-declarations of the classes used for specifying the visitable structure. 

In other words, if the base class user does not use the visitor capabilities, he/she does not need to have any physical dependency towards the visitation details, apart from forward-declarations. This protects the client translation unit from unnecessary recompilation.

Given the following class hierarchy: 
   