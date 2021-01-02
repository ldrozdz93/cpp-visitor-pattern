#pragma once
#include "vstor/vstor.hpp"
#include "base.hpp"

// clang-format off
template <typename T>
class BaseImpl : public vstor::VisitableImpl<T, Base> {};

struct BaseChildren : vstor::VisitableListVariant<
    struct Derived1,
    struct Derived2
> {};
// clang-format on