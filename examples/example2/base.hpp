#pragma once
#include "vstor/vstor.hpp"

struct BaseChildren;  // note: forward-declaration

class Base : public vstor::VisitableFor<BaseChildren> {
public:
    virtual ~Base() = default;
    virtual int some_base_method() = 0;
};
