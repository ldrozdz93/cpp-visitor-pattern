#define PICOBENCH_DEBUG
#define PICOBENCH_IMPLEMENT_WITH_MAIN
#define PICOBENCH_DEFAULT_ITERATIONS          \
    {                                         \
        10, 100, 1000, 10000, 100000, 1000000 \
    }
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

using BaseChildren =
    vstor::VisitableListVariant<struct Derived1, struct Derived2, struct Derived3, struct Derived4,
                                struct Derived5, struct Derived6, struct Derived7, struct Derived8,
                                struct Derived9, struct Derived10>;

struct Base : vstor::VisitableFor<BaseChildren> {
    virtual ~Base() = default;
};

// clang-format off
struct Derived1 : vstor::VisitableImpl<Derived1, Base> {};
struct Derived2 : vstor::VisitableImpl<Derived2, Base> {};
struct Derived3 : vstor::VisitableImpl<Derived3, Base> {};
struct Derived4 : vstor::VisitableImpl<Derived4, Base> {};
struct Derived5 : vstor::VisitableImpl<Derived5, Base> {};
struct Derived6 : vstor::VisitableImpl<Derived6, Base> {};
struct Derived7 : vstor::VisitableImpl<Derived7, Base> {};
struct Derived8 : vstor::VisitableImpl<Derived8, Base> {};
struct Derived9 : vstor::VisitableImpl<Derived9, Base> {};
struct Derived10 : vstor::VisitableImpl<Derived10, Base> {};
// clang-format on

void vstor_visit(picobench::state& s)
{
    Derived10 derived{};
    Base* current_base = &derived;
    for (auto _ : s) {
        auto not_optimized = current_base->visit_by(vstor::Overloaded{
            [](Derived1&) { return 1; },
            [](auto&) { return 2; },
        });
        DoNotOptimize(not_optimized);
        DoNotOptimize(current_base);
    }
}
PICOBENCH(vstor_visit);

void odwyer_visit(picobench::state& s)
{
    Derived10 derived{};
    Base* current_base = &derived;
    for (auto _ : s) {
        auto not_optimized = odwyer::visit<Derived1, Derived2, Derived3, Derived4, Derived5,
                                           Derived6, Derived7, Derived8, Derived9, Derived10>(
            *current_base, vstor::Overloaded{
                               [](Derived5&) { return 1; },
                               [](auto&) { return 2; },
                           });
        DoNotOptimize(not_optimized);
        DoNotOptimize(current_base);
    }
}
PICOBENCH(odwyer_visit);
