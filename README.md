# What is this?
This repo provides single-header C++20 compatible building blocks for implementing the 
Visitor design pattern in c++, put in namespace ```vstor```.

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
- A base class user does not even need to know the possibly-visited classes, if he doesn't use the visitation capabilities. It's hard to achieve it with ```std::variant```.
- A base class user does not need to physically depend on the concrete derived class definitions, if those are not visited (see the full example below).


## Full example:
The idea of this library is to require the base class user to be aware of any concrete visitable classes only when visitation is used on the base class. 
That's achieved with allowing forward-declarations of the classes used for specifying the visitable structure. 

In other words, if the base class user does not use the visitor capabilities, 
he/she does not need to have any physical dependency towards the 
visitation details, apart from forward-declarations. This protects the 
client translation unit from unnecessary recompilation or editing.

Note in the following example:
- ```base.hpp``` forward-declares ```BaseChildren```, so the ```Base``` users opt-in for the visitor dependencies.
- ```base_children.hpp``` forward-declares the children classes, so the ```Base``` users opt-in for concrete children definitions.
- ```user2.cpp``` does not use visitation, so will not be even rebuilt in case ```BaseChildren``` is touched. That's not the case when using ```std::variant``` directly.
- ```user3.cpp``` depends only on ```Derived1```, so does not need to recompile if any other ```Derived``` headers are touched,
and does not need to be touched when any new ```Derived``` classes are added.

##### base.hpp
```
#pragma once
#include "vstor/vstor.hpp"

struct BaseChildren;  // note: forward-declaration

class Base : public vstor::VisitableFor<BaseChildren> {
public:
    virtual ~Base() = default;
    virtual int some_base_method() = 0;
};
```
##### base_impl.hpp
```
#pragma once
#include "vstor/vstor.hpp"
#include "base.hpp"

template <typename T>
class BaseImpl : public vstor::VisitableImpl<T, Base> {};

struct BaseChildren : vstor::VisitableListVariant<
    struct Derived1,
    struct Derived2
> {};
```
##### derived1.hpp
```
#pragma once
#include "base_impl.hpp"

class Derived1 : public BaseImpl<Derived1> {
public:
    int some_base_method() override { return 1; };
    int some_derived1_method(){ return 10; }
};
```
##### derived2.hpp
```
#pragma once
#include "base_impl.hpp"

class Derived2 : public BaseImpl<Derived2> {
public:
    int some_base_method() override { return 2; };
    int some_derived2_method(){ return 20; }
};
```
##### user1.hpp
```
#pragma once
#include "base.hpp"

int user1_value_by_visitation(Base& base);
```
##### user1.cpp
```
#include "user1.hpp"
#include "derived1.hpp"
#include "derived2.hpp"
#include "vstor/vstor.hpp"

/**
 * NOTE: Depends on the whole visitable structure, so will recompile when any visitable header
 * changes.
 * */
int user1_value_by_visitation(Base& base)
{
    return base.visit_by(vstor::Overloaded{
        [](Derived1& d1){ return d1.some_derived1_method(); },
        [](Derived2& d2){ return d2.some_derived2_method(); }
    });
}
```
##### user2.hpp
```
#pragma once
#include "base.hpp"

int user2_value_by_virtual_calls(Base& base);
```
##### user2.cpp
```
#include "user2.hpp"

/**
 * NOTE: Does not need to know anything about the visitables, so touching the visitable structure
 * does not cause recompilation.
 * */
int user2_value_by_virtual_calls(Base& base) 
{ 
    return base.some_base_method(); 
}
```
##### user3.hpp
```
#pragma once
#include "base.hpp"

int user3_value_by_visitation(Base& base);
```
##### user3.cpp
```
#include "user3.hpp"
#include "derived1.hpp"

/**
 * NOTE: Depends only on Derived1, so adding a new Derived3 to hierarchy does not require the user
 * to alter this file.
 * */
int user3_value_by_visitation(Base& base)
{
    return base.visit_by(vstor::Overloaded{
        [](Derived1& d1){ return d1.some_derived1_method(); },
        [](auto&){ return 0; }  // return 0 for any other than Derived1
    });
}
```
##### example.cpp
```
#include "derived1.hpp"
#include "user1.hpp"
#include "user2.hpp"
#include "user3.hpp"

int main()
{
    Derived1 derived{};
    auto user1_result = user1_value_by_visitation(derived);
    auto user2_result = user2_value_by_virtual_calls(derived);
    auto user3_result = user3_value_by_visitation(derived);
    return 0;
}

```
# Alternatives
- Visitor pattern by Fedor Pikus: https://youtu.be/MdtYi0vvct0?t=880
    - uses double virtual dispatch (```vstor``` uses virtual dispatch + ```std::variant``` dispatch)
    - uses a fixed return type (```vstor``` allows arbitrary return types)
    - puts restrictions on the visitor overload set, i.e. correct order of types and no ```auto&``` types alowed (```vstor``` accepts any overload set)
    - TODO: add comparison benchmark 
- Visitor pattern by  Arthur O’Dwyer: https://quuxplusone.github.io/blog/2020/09/29/oop-visit/
    - uses sequential typeid comparison (```vstor``` uses virtual dispatch + ```std::variant``` dispatch)
    - specifies the concrete visitable types in the call site, which requires runtime error handling (```vstor``` knows the types are correct by compilation)
    - the above point makes it less intrusive (```vstor``` knows forces the usage of inheritance and CRTP)
    - TODO: add comparison benchmark
Please write to me if you know any other valuable alternatives to compare with. 