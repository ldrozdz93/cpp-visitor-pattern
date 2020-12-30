#pragma once
#include "base_impl.hpp"

class Derived2 : public BaseImpl<Derived2> {
    int some_base_method() override { return 2; };
};