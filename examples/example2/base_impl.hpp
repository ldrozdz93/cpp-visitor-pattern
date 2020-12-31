#pragma once
#include "vstor/vstor.hpp"
#include "base.hpp"
#include "base_children.hpp"

// clang-format off
template <typename T>
class BaseImpl : public vstor::VisitableImpl<T, Base> {};
// clang-format on