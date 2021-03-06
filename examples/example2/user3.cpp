#include "user3.hpp"
#include "derived1.hpp"

/**
 * NOTE: Depends only on Derived1, so adding a new Derived3 to hierarchy does not require the user
 * to alter this file.
 * */
int user3_value_by_visitation(Base& base)
{
    // clang-format off
    return base.visit_by(vstor::Overloaded{
        [](Derived1& d1){ return d1.some_derived1_method(); },
        [](auto&){ return 0; }  // return 0 for any other than Derived1
    });
    // clang-format on
}
