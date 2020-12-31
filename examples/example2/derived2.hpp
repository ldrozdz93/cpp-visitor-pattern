#pragma once
#include "base_impl.hpp"

class Derived2 : public BaseImpl<Derived2> {
public:
    int some_base_method() override { return 2; };
    int some_derived2_method(){ return 20; }
};