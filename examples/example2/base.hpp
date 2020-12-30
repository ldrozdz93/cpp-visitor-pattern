#pragma once
#include "vstor/vstor.hpp"

struct Visitables;  // note: forward-declaration

class Base : public vstor::VisitableFor<Visitables> {
public:
    virtual ~Base() = default;
    virtual int some_base_method() = 0;
};

// clang-format off
template <typename T>
class BaseImpl : public vstor::VisitableImpl<T, Base> {};
// clang-format on