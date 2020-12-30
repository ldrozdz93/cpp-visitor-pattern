#pragma once
#include "vstor/vstor.hpp"

// clang-format off
struct BaseChildren : vstor::VisitableListVariant<struct Derived1, struct Derived2> {};
// clang-format on
