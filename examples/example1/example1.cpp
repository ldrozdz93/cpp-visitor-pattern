#include "vstor/vstor.hpp"

namespace {
// clang-format off
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

// clang-format on
}  // namespace

int main()
{
    Derived1 derived{};
    return fun(derived); // returns 1
}
