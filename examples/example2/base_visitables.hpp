#pragma once
#include "vstor/vstor.hpp"

struct Visitables : vstor::VisitableListVariant<struct Derived1, struct Derived2> {
};
