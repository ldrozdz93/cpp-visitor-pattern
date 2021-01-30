#pragma once

#include <exception>
#include <type_traits>

namespace odwyer {

template <class T, class U>
struct match_cvref {
    using type = U;
};
template <class T, class U>
struct match_cvref<T&, U> {
    using type = U&;
};
template <class T, class U>
struct match_cvref<T&&, U> {
    using type = U&&;
};
template <class T, class U>
struct match_cvref<const T, U> {
    using type = const U;
};
template <class T, class U>
struct match_cvref<const T&, U> {
    using type = const U&;
};
template <class T, class U>
struct match_cvref<const T&&, U> {
    using type = const U&&;
};
template <class T, class U>
using match_cvref_t = typename match_cvref<T, U>::type;

template <class Base, class F, class E>
struct visit_impl {
    template <int...>  // this is call<>(b, f, e)
    static auto call(Base&& b, const F&, const E& e)
    {
        return e(static_cast<Base&&>(b));
    }

    template <class DerivedClass, class... Rest>
    static auto call(Base&& b, const F& f, const E& e)
    {
        using Derived = odwyer::match_cvref_t<Base, DerivedClass>;
        using T = decltype(f(static_cast<Derived&&>(b)));
        using ErrorT = decltype(e(static_cast<Base&&>(b)));
        if (typeid(b) == typeid(DerivedClass)) {
            return f(static_cast<Derived&&>(b));
        } else if constexpr (sizeof...(Rest) != 0) {
            return call<Rest...>(static_cast<Base&&>(b), f, e);
        } else if constexpr (std::is_void_v<ErrorT> && !std::is_void_v<T>) {
            // If e(b) has type void, assume it exits by throwing.
            e(static_cast<Base&&>(b));
            throw std::bad_cast();  // we expect this to be unreachable
        } else {
            return e(static_cast<Base&&>(b));
        }
    }
};

template <class... DerivedClasses, class Base, class F, class E>
auto visit(Base&& base, const F& f, const E& e)
{
    static_assert(!std::is_pointer_v<std::remove_reference_t<Base>>,
                  "Argument must be (reference-to) class type, not pointer type");
    static_assert(std::is_polymorphic_v<std::remove_reference_t<Base>>,
                  "Argument must be of polymorphic class type");
    static_assert((std::is_polymorphic_v<DerivedClasses> && ...),
                  "Template arguments must all be polymorphic class types");

    return visit_impl<Base, F, E>::template call<DerivedClasses...>(static_cast<Base&&>(base), f,
                                                                    e);
}

template <class... DerivedClasses, class Base, class F>
auto visit(Base&& b, const F& f)
{
    return odwyer::visit<DerivedClasses...>(static_cast<Base&&>(b), f,
                                            [](Base&&) { throw std::bad_cast(); });
}

}  // namespace odwyer
