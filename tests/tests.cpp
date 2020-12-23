#include <string>
#include <type_traits>

#include "catch.hpp"
#include "vstor/vstor.hpp"

namespace {

using vstor::Overloaded;
using vstor::VisitableFor;
using vstor::VisitableImpl;
using vstor::VisitableListVariant;

// NOTE: below code order is important from the testing POV

// GIVEN a forward-declared list of visitables,
struct VisitablesVariant;

// WHEN creating a visitable base,
// THEN it can be declared with an incomplete type
struct Base : VisitableFor<VisitablesVariant> {
};

// AND list of visitables can be any template list
template <typename...>
struct AnyList;
struct D1;
struct D2;

using ListOfVisitables = AnyList<D1, D2>;

// AND require complete type only when visitation is used
struct VisitablesVariant : VisitableListVariant<ListOfVisitables> {
    using VisitableListVariant<ListOfVisitables>::VisitableListVariant;
};

template <typename Impl>
struct BaseImpl : VisitableImpl<Impl, Base> {
};

struct D1 : BaseImpl<D1> {
};
struct D2 : BaseImpl<D2> {
};

TEST_CASE("Visitor")
{
    using std::string;
    GIVEN("Polymorphic class hierarchy")
    {
        D2 d2{};
        WHEN("object is mutable")
        {
            Base& b = d2;
            THEN("can be visited")
            {
                string res =
                    b.visit_by(Overloaded{[](D1&) { return "D1"; }, [](D2&) { return "D2"; }});
                REQUIRE(res == "D2");
            }
        }
        WHEN("object is const")
        {
            const Base& b = d2;
            THEN("can be visited")
            {
                string res = b.visit_by(
                    Overloaded{[](const D1&) { return "D1"; }, [](const D2&) { return "D2"; }});
                REQUIRE(res == "D2");
            }
        }
        WHEN("object is rvalue")
        {
            THEN("can be visited")
            {
                // TODO: should non-const references be allowed to bind to rvalues?
                string res =
                    D1{}.visit_by(Overloaded{[](D1&) { return "D1"; }, [](D2&) { return "D2"; }});
                REQUIRE(res == "D1");
            }
        }

        // TODO: will constexpr ever work here?
        //        WHEN("object is constexpr")
        //        {
        //            constexpr const D1 d1{};
        //            THEN("can be visited")
        //            {
        //                constexpr int res = d1.visit_by(
        //                    overloaded{[](const D1&) { return 1; }, [](const D2&) { return 2; }});
        //                STATIC_REQUIRE(res == "D2");
        //            }
        //        }
    }
}

TEST_CASE("Utilities")
{
    SECTION("Overloaded")
    {
        // TODO: add better tests.

        using namespace std::string_literals;
        auto sut = vstor::detail::Overloaded{
            [](char) { return "char"s; },
            [](int) { return "int"s; },
            [](float) { return "float"s; },
        };
        REQUIRE("char" == sut(char{}));
        REQUIRE("int" == sut(int{}));
        REQUIRE("float" == sut(float{}));
    }
}

}  // namespace