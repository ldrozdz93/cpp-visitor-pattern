# What is this?
This repo provides the building blocks for implementing the Visitor design pattern in c++.

# How does it work?
The Visitor pattern implementation provided by this repository allows the user to 
use the same semantics for a visitation as used for ```std::visit```, i.e. use overload resolution instead
 of overriding virtual functions.
This allows the user to return any desired type from a visitation.


# How to use it?
## Minimal example:
#### Define a class hierarchy:
```
#include "vstor/vstor.hpp"

using PossibleVisitables = vstor::VisitableListVariant<struct Derived1, struct Derived2>;
using VisitableBase = vstor::VisitableFor<PossibleVisitables>;

struct Base : VisitableBase {
    virtual ~Base() = default;
};

struct Derived1 : vstor::VisitableImpl<Derived1, Base> {
};

struct Derived2 : vstor::VisitableImpl<Derived2, Base> {
};
```
#### And use it:
```
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
