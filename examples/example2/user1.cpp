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
    // clang-format off
    return base.visit_by(vstor::Overloaded{
        [](Derived1& d1){ return d1.some_derived1_method(); },
        [](Derived2& d2){ return d2.some_derived2_method(); }
    });
    // clang-format on
}
