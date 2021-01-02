#include <deque>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#define PICOBENCH_DEBUG
#define PICOBENCH_IMPLEMENT_WITH_MAIN
#define PICOBENCH_DEFAULT_ITERATIONS {10, 100, 1000, 10000, 100000, 1000000}
#include "picobench/picobench.hpp"
#include "visitor_odwyer.hpp"
#include "vstor/vstor.hpp"

// DoNotOptimize implementation copypasted from google-benchmark
template <class Tp>
__attribute__((always_inline)) void DoNotOptimize(Tp const& value)
{
#if defined(__clang__)
    asm volatile("" : : "g"(value) : "memory");
#else
    asm volatile("" : : "i,r,m"(value) : "memory");
#endif
}

using BaseChildren = vstor::VisitableListVariant<struct Derived1, struct Derived2>;

struct Base : vstor::VisitableFor<BaseChildren> {
    virtual ~Base() = default;
};

// clang-format off
struct Derived1 : vstor::VisitableImpl<Derived1, Base> {};
struct Derived2 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived3 : vstor::VisitableImpl<Derived1, Base> {};
struct Derived4 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived5 : vstor::VisitableImpl<Derived1, Base> {};
struct Derived6 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived7 : vstor::VisitableImpl<Derived1, Base> {};
struct Derived8 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived9 : vstor::VisitableImpl<Derived1, Base> {};
struct Derived10 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived11 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived12 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived13 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived14 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived15 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived16 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived17 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived18 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived19 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived20 : vstor::VisitableImpl<Derived2, Base> {};
// clang-format on

Base* curr_base;

void vstor_visit(picobench::state& s)
{
    Derived10 derived{};
    curr_base = &derived;
    for (auto _ : s) {
        auto not_optimized = curr_base->visit_by(vstor::Overloaded{
            [](Derived1&) { return 1; },
            [](Derived2&) { return 2; },
        });
        DoNotOptimize(not_optimized);
    }
}
PICOBENCH(vstor_visit);

void odwyer_visit(picobench::state& s)
{
    Derived10 derived{};
    curr_base = &derived;
    for (auto _ : s) {
        auto not_optimized =
            odwyer::visit<Derived1, Derived2, Derived3, Derived4, Derived5, Derived6, Derived7,
                          Derived8, Derived9, Derived10, Derived11, Derived12, Derived13, Derived14,
                          Derived15, Derived16, Derived17, Derived18, Derived19, Derived20>(
                *curr_base, vstor::Overloaded{
                                [](Derived20&) { return 1; },
                                [](auto&) { return 2; },
                            });
        DoNotOptimize(not_optimized);
    }
}
PICOBENCH(odwyer_visit);
