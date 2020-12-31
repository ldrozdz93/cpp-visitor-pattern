#pragma once
#include "base_impl.hpp"

class Derived1 : public BaseImpl<Derived1> {
public:
    int some_base_method() override { return 1; };
    int some_derived1_method(){ return 10; }
};