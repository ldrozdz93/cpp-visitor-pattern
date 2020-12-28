#pragma once

#include "base.hpp"
#include "base_visitables.hpp"

class Derived2 : public BaseImpl<Derived2> {
    int some_base_method() override { return 2; };
};