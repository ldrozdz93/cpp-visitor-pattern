#pragma once
#include "vstor/vstor.hpp"

// clang-format off
struct Visitables : vstor::VisitableListVariant<struct Derived1, struct Derived2> {};
// clang-format on
