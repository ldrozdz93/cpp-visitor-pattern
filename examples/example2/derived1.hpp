#pragma once
#include "base_impl.hpp"

class Derived1 : public BaseImpl<Derived1> {
    int some_base_method() override { return 1; };
};