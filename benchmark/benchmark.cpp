#include <memory>
#include <random>
#include <tuple>
#include <variant>

#include <benchmark/benchmark.h>

#include "visitor_odwyer.hpp"
#include "visitor_pikus.hpp"
#include "vstor/vstor.hpp"

namespace util {

template <size_t I = 0, typename... Tp, typename F>
void for_each_tup_elem(std::tuple<Tp...>& t, F&& f)
{
    f(std::get<I>(t));
    if constexpr (I + 1 != sizeof...(Tp)) {
        for_each_tup_elem<I + 1>(t, std::forward<F>(f));
    }
}

auto shuffle(auto& range)
{
    std::shuffle(std::begin(range), std::end(range),
                 std::default_random_engine{std::random_device{}()});
}

}  // namespace util

using BaseChildren =
    vstor::VisitableListVariant<struct Derived1, struct Derived2, struct Derived3, struct Derived4,
                                struct Derived5, struct Derived6, struct Derived7, struct Derived8,
                                struct Derived9, struct Derived10>;

struct Base : vstor::VisitableFor<BaseChildren> {
    virtual ~Base() = default;
    virtual int value_by_virtual() = 0;
};

// clang-format off
struct Derived1 : vstor::VisitableImpl<Derived1, Base> { int value_by_virtual() override {return 13654;} };
struct Derived2 : vstor::VisitableImpl<Derived2, Base> { int value_by_virtual() override {return 23654;} };
struct Derived3 : vstor::VisitableImpl<Derived3, Base> { int value_by_virtual() override {return 33654;} };
struct Derived4 : vstor::VisitableImpl<Derived4, Base> { int value_by_virtual() override {return 43654;} };
struct Derived5 : vstor::VisitableImpl<Derived5, Base> { int value_by_virtual() override {return 53654;} };
struct Derived6 : vstor::VisitableImpl<Derived6, Base> { int value_by_virtual() override {return 63654;} };
struct Derived7 : vstor::VisitableImpl<Derived7, Base> { int value_by_virtual() override {return 73654;} };
struct Derived8 : vstor::VisitableImpl<Derived8, Base> { int value_by_virtual() override {return 83654;} };
struct Derived9 : vstor::VisitableImpl<Derived9, Base> { int value_by_virtual() override {return 93654;} };
struct Derived10 : vstor::VisitableImpl<Derived10, Base> { int value_by_virtual() override {return 103654;} };
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

class One_single_call_from_the_hierarchy : public benchmark::Fixture {
};
BENCHMARK_F(One_single_call_from_the_hierarchy, virtual_call_without_visitor)
(benchmark::State& s)
{
    auto current_base = std::make_unique<Derived10>();
    for (auto _ : s) {
        auto not_optimized = current_base->value_by_virtual();
        benchmark::DoNotOptimize(not_optimized);
        benchmark::DoNotOptimize(current_base);
    }
}
BENCHMARK_F(One_single_call_from_the_hierarchy, std_variant_without_virtual_calls)
(benchmark::State& s)
{
    Derived10 derived{};
    using Base = std::variant<Derived1, Derived2, Derived3, Derived4, Derived5, Derived6, Derived7,
                              Derived8, Derived9, Derived10>;
    Base current_base = derived;
    for (auto _ : s) {
        auto not_optimized = std::visit(vstor::Overloaded{
                                            [](Derived1&) { return 1; },
                                            [](auto&) { return 2; },
                                        },
                                        current_base);
        benchmark::DoNotOptimize(not_optimized);
        benchmark::DoNotOptimize(current_base);
    }
}
BENCHMARK_F(One_single_call_from_the_hierarchy, vstor_visit)(benchmark::State& s)
{
    Derived10 derived{};
    Base* current_base = &derived;
    for (auto _ : s) {
        auto not_optimized = current_base->visit_by(vstor::Overloaded{
            [](Derived1&) { return 1; },
            [](auto&) { return 2; },
        });
        benchmark::DoNotOptimize(not_optimized);
        benchmark::DoNotOptimize(current_base);
    }
}
BENCHMARK_F(One_single_call_from_the_hierarchy, odwyer_visit)(benchmark::State& s)
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
        benchmark::DoNotOptimize(not_optimized);
        benchmark::DoNotOptimize(current_base);
    }
}
BENCHMARK_F(One_single_call_from_the_hierarchy, pikus_visit)(benchmark::State& s)
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
        benchmark::DoNotOptimize(result);
        benchmark::DoNotOptimize(current_base);
    }
}

class Calling_every_class_from_the_hierarchy : public benchmark::Fixture {
};
BENCHMARK_F(Calling_every_class_from_the_hierarchy, virtual_method_call)
(benchmark::State& s)
{
    using AllDerived = std::tuple<Derived1, Derived2, Derived3, Derived4, Derived5, Derived6,
                                  Derived7, Derived8, Derived9, Derived10>;
    AllDerived all_derived{};
    std::vector<std::unique_ptr<Base>> all_bases{};
    util::for_each_tup_elem(all_derived, [&](auto& derived) {
        all_bases.push_back(std::make_unique<std::remove_reference_t<decltype(derived)>>(derived));
    });
    util::shuffle(all_bases);
    for (auto _ : s) {
        for (auto& current_base : all_bases) {
            auto not_optimized = current_base->value_by_virtual();
            benchmark::DoNotOptimize(not_optimized);
        }
    }
}
BENCHMARK_F(Calling_every_class_from_the_hierarchy, std_variant_visit)
(benchmark::State& s)
{
    using AllDerived = std::tuple<Derived1, Derived2, Derived3, Derived4, Derived5, Derived6,
                                  Derived7, Derived8, Derived9, Derived10>;
    using Base = std::variant<Derived1, Derived2, Derived3, Derived4, Derived5, Derived6, Derived7,
                              Derived8, Derived9, Derived10>;
    AllDerived all_derived{};
    std::vector<Base> all_bases{};
    util::for_each_tup_elem(all_derived, [&](auto& derived) { all_bases.push_back(derived); });
    util::shuffle(all_bases);
    for (auto _ : s) {
        for (auto& current_base : all_bases) {
            auto not_optimized =
                std::visit(vstor::Overloaded{
                               // clang-format off
                                                [](Derived1&) { return 13654; },
                                                [](Derived2&) { return 23654; },
                                                [](Derived3&) { return 33654; },
                                                [](Derived4&) { return 43654; },
                                                [](Derived5&) { return 53654; },
                                                [](Derived6&) { return 63654; },
                                                [](Derived7&) { return 73654; },
                                                [](Derived8&) { return 83654; },
                                                [](Derived9&) { return 93654; },
                                                [](Derived10&) { return 103654; }
                               // clang-format on
                           },
                           current_base);
            benchmark::DoNotOptimize(not_optimized);
        }
    }
}
BENCHMARK_F(Calling_every_class_from_the_hierarchy, vstor_visitor)(benchmark::State& s)
{
    using AllDerived = std::tuple<Derived1, Derived2, Derived3, Derived4, Derived5, Derived6,
                                  Derived7, Derived8, Derived9, Derived10>;
    AllDerived all_derived{};
    std::vector<Base*> all_bases{};
    util::for_each_tup_elem(all_derived, [&](auto& derived) { all_bases.push_back(&derived); });
    util::shuffle(all_bases);
    for (auto _ : s) {
        auto visitor = vstor::Overloaded{
            [](Derived1&) { return 1; },
            [](auto&) { return 2; },
        };
        for (auto* current_base : all_bases) {
            auto not_optimized = current_base->visit_by(vstor::Overloaded{
                // clang-format off
                [](Derived1&) { return 13654; },
                [](Derived2&) { return 23654; },
                [](Derived3&) { return 33654; },
                [](Derived4&) { return 43654; },
                [](Derived5&) { return 53654; },
                [](Derived6&) { return 63654; },
                [](Derived7&) { return 73654; },
                [](Derived8&) { return 83654; },
                [](Derived9&) { return 93654; },
                [](Derived10&) { return 103654; }
                // clang-format on
            });
            benchmark::DoNotOptimize(not_optimized);
        }
    }
}
BENCHMARK_F(Calling_every_class_from_the_hierarchy, odwyer_visitor)(benchmark::State& s)
{
    using AllDerived = std::tuple<Derived1, Derived2, Derived3, Derived4, Derived5, Derived6,
                                  Derived7, Derived8, Derived9, Derived10>;
    AllDerived all_derived{};
    std::vector<Base*> all_bases{};
    util::for_each_tup_elem(all_derived, [&](auto& derived) { all_bases.push_back(&derived); });
    util::shuffle(all_bases);
    for (auto _ : s) {
        auto visitor = vstor::Overloaded{
            [](Derived1&) { return 1; },
            [](auto&) { return 2; },
        };
        for (auto* current_base : all_bases) {
            auto not_optimized = odwyer::visit<Derived1, Derived2, Derived3, Derived4, Derived5,
                                               Derived6, Derived7, Derived8, Derived9, Derived10>(
                *current_base,
                vstor::Overloaded{
                    // clang-format off
                    [](Derived1&) { return 13654; },
                    [](Derived2&) { return 23654; },
                    [](Derived3&) { return 33654; },
                    [](Derived4&) { return 43654; },
                    [](Derived5&) { return 53654; },
                    [](Derived6&) { return 63654; },
                    [](Derived7&) { return 73654; },
                    [](Derived8&) { return 83654; },
                    [](Derived9&) { return 93654; },
                    [](Derived10&) { return 103654; }
                    // clang-format on
                });
            benchmark::DoNotOptimize(not_optimized);
        }
    }
}
BENCHMARK_F(Calling_every_class_from_the_hierarchy, pikus_visitor)(benchmark::State& s)
{
    using namespace pikus_alternative_hierarchy;

    using AllDerived = std::tuple<D1, D2, D3, D4, D5, D6, D7, D8, D9, D10>;
    AllDerived all_derived{};
    std::vector<B*> all_bases{};
    util::for_each_tup_elem(all_derived, [&](auto& derived) { all_bases.push_back(&derived); });
    util::shuffle(all_bases);
    for (auto _ : s) {
        int result{};
        for (auto* current_base : all_bases) {
            auto visitor = pikus::lambda_visitor<pikus_alternative_hierarchy::BaseVisitor>(
                // clang-format off
                [&](D1*) { result = 13654; },
                [&](D2*) { result = 23654; },
                [&](D3*) { result = 33654; },
                [&](D4*) { result = 43654; },
                [&](D5*) { result = 53654; },
                [&](D6*) { result = 63654; },
                [&](D7*) { result = 73654; },
                [&](D8*) { result = 83654; },
                [&](D9*) { result = 93654; },
                [&](D10*) { result = 103654; }
                // clang-format on
            );
            current_base->accept(visitor);
            benchmark::DoNotOptimize(result);
        }
    }
}
