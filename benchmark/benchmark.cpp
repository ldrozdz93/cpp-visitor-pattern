#define PICOBENCH_DEBUG
#define PICOBENCH_IMPLEMENT_WITH_MAIN
#define PICOBENCH_DEFAULT_ITERATIONS       \
    {                                      \
        1, 10, 100, 1000, 100000, 10000000 \
    }
#include "picobench/picobench.hpp"
#include "visitor_odwyer.hpp"
#include "visitor_pikus.hpp"
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

namespace pikus_alternative_hierarchy {

using BaseVisitor = pikus::Visitor<struct D1, struct D2, struct D3, struct D4, struct D5, struct D6,
                                   struct D7, struct D8, struct D9, struct D10>;
struct B {
    virtual ~B() = default;
    virtual void accept(BaseVisitor& v) = 0;
};

template <typename Derived>
struct Visitable : public B {
    void accept(BaseVisitor& v) override { v.visit(static_cast<Derived*>(this)); }
};

// clang-format off
struct D1 : Visitable<D1> {};
struct D2 : Visitable<D2> {};
struct D3 : Visitable<D3> {};
struct D4 : Visitable<D4> {};
struct D5 : Visitable<D5> {};
struct D6 : Visitable<D6> {};
struct D7 : Visitable<D7> {};
struct D8 : Visitable<D8> {};
struct D9 : Visitable<D9> {};
struct D10 : Visitable<D10> {};
// clang-format on

}  // namespace pikus_alternative_hierarchy

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

void pikus_visit(picobench::state& s)
{
    using namespace pikus_alternative_hierarchy;
    D5 derived{};
    B* current_base = &derived;
    for (auto _ : s) {
        int result{};
        auto visitor = pikus::lambda_visitor<pikus_alternative_hierarchy::BaseVisitor>(
            // clang-format off
            [&](D1*) { result = 1; },
            [&](D2*) { result = 2; },
            [&](D3*) { result = 3; },
            [&](D4*) { result = 4; },
            [&](D5*) { result = 5; },
            [&](D6*) { result = 6; },
            [&](D7*) { result = 7; },
            [&](D8*) { result = 8; },
            [&](D9*) { result = 9; },
            [&](D10*) { result = 10; }
            // clang-format on
        );
        current_base->accept(visitor);
        auto not_optimized = result;
        DoNotOptimize(not_optimized);
        DoNotOptimize(current_base);
    }
}
PICOBENCH(pikus_visit);
