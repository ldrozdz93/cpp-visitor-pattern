#include "vstor/vstor.hpp"

namespace {

using PossibleVisitables = vstor::VisitableListVariant<struct Derived1, struct Derived2>;
using VisitableBase = vstor::VisitableFor<PossibleVisitables>;

struct Base : VisitableBase {
    virtual ~Base() = default;
};

struct Derived1 : vstor::VisitableImpl<Derived1, Base> {
};

struct Derived2 : vstor::VisitableImpl<Derived2, Base> {
};

int fun(Base& base)
{
    // clang-format off
    return base.visit_by(vstor::Overloaded{
        [](Derived1&){ return 1; },
        [](Derived2&){ return 2; }
    });
    // clang-format on
}

}  // namespace

int main()
{
    Derived1 derived{};
    return fun(derived); // returns 1
}
