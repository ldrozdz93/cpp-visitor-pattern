#include "user1.hpp"
#include "base_visitables.hpp"
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
        [](Derived1&){ return 1; },
        [](Derived2&){ return 2; }
    });
    // clang-format on
}
