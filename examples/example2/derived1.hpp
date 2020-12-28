#pragma once

#include "base.hpp"
#include "base_visitables.hpp"

class Derived1 : public BaseImpl<Derived1> {
    int some_base_method() override { return 1; };
};