#ifndef VSTOR_HPP
#define VSTOR_HPP

#include <type_traits>
#include <variant>

namespace vstor {

namespace detail {

/**
 * A commonly used template for creating lambda-based callable overload sets.
 *
 * @todo add possibility to set a fixed required return type
 * */
template <class... Ts>
class Overloaded : public Ts... {
public:
    using Ts::operator()...;
};
#if __cpp_deduction_guides < 201907L  // if CTAD for aggregates and aliases not supported
template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;
#endif

// TODO: implement concept: invocable_with_each_variant_option<F, VisitablesVariant>

/**
 * A tag class for marking every VisitableFor<...> instance. Used instead of clunky sfinae for
 * checking if a given type is a subclass of an instantiation of VisitableFor<...> template.
 * */
struct VisitableFor_BaseTag {
};
template <typename T>
concept AnyVisitableFor = std::is_base_of_v<VisitableFor_BaseTag, T>;

/**
 * A base class for all Visitable classes, that perform the double dispatch of the visitor pattern.
 * The concrete implementations of this base participate in the first dispatch by virtual functions.
 * The second dispatch is performed as a std::visit visitation of possible visitables.
 *
 * @tparam Variant - a maybe-incomplete class, that at the point of instantiation must be a class
 * derived from VisitableListVariant<...>. Note it's not checked by any concept, because that would
 * require an eager instantiation of the list of possible visitable variants, which is not intended.
 * */
template <typename Variant>
class VisitableFor : public VisitableFor_BaseTag {
public:
    using VisitableVariant = Variant;

    virtual VisitableVariant as_variant() const noexcept = 0;

    /**
     * Performs visitation on 'this' by the same rules as std::visit does.
     */
    template <typename Visitor>
    // requires invocable_with_each_variant_option<Visitor, VisitableVariant>
    decltype(auto) visit_by(Visitor&& visitor);

    template <typename Visitor>
    // requires invocable_with_each_variant_option<Visitor, VisitableVariant>
    decltype(auto) visit_by(Visitor&& visitor) const;
};

template <typename Variant>
template <typename Visitor>
decltype(auto) VisitableFor<Variant>::visit_by(Visitor&& visitor)
{
    auto invoke_visitor_after_dereference_and_deconst = [&](auto&& v) -> decltype(auto) {
        // NOTE: it's safe to const_cast, because the pointee of the pointer inside the visitable
        // variant is in fact 'this', so it has the same cv-qualification
        using NonConstVisitable = std::remove_cvref_t<decltype(*std::forward<decltype(v)>(v))>;
        return std::invoke(std::forward<Visitor>(visitor),
                           const_cast<NonConstVisitable&>(*std::forward<decltype(v)>(v)));
    };
    // NOTE: the whole pattern cannot be noexcept friendly due to std::visit possibly throwing
    return std::visit(invoke_visitor_after_dereference_and_deconst, as_variant().as_std_variant());
}

template <typename Variant>
template <typename Visitor>
decltype(auto) VisitableFor<Variant>::visit_by(Visitor&& visitor) const
{
    auto invoke_visitor_after_dereference = [&](auto&& v) -> decltype(auto) {
        return std::invoke(std::forward<Visitor>(visitor), *std::forward<decltype(v)>(v));
    };

    return std::visit(invoke_visitor_after_dereference, as_variant().as_std_variant());
}

/**
 * Base class for the concrete visitable implementation.
 *
 * @tparam CrtpImpl - CRTP class that must be a part of the previously-defined VisitableVariant.
 * It's incomplete due to CRTP usage.
 *
 * @tparam Base - The concrete VisitableFor<...> base class this implementation is based on.
 * */
template <typename CrtpImpl, AnyVisitableFor Base>
class VisitableImpl : public Base {
public:
    using VisitableVariant = typename Base::VisitableVariant;

    /**
     * This method is const, without any non-const overload for the following reason: the
     * return value can be incomplete until used. The return value for a non-const overload would
     * need to differ from the const overload. Doing that with type traits would require a complete
     * VisitableVariant class, which we want to avoid, so the end user would need to provide 2
     * incomplete VisitableVariants that would differ from each other only with the possible variant
     * being const. Instead, we const_cast the const away in case we know the instance being worked
     * with is in fact non-const.
     *
     * @returns 'this' as one of the possible variant options.
     * */
    VisitableVariant as_variant() const noexcept final
    {
        return VisitableVariant{static_cast<const CrtpImpl*>(this)};
    }
};
template <typename... Args>
using StdVariantBase = std::variant<const Args* const...>;

/**
 * Base class for aggregating maybe-incomplete classes possible for visitation.
 *
 * @tparam Visitables - a variadic list of maybe-incomplete visitables.
 * */
template <typename... Visitables>
class VisitableListVariant : private StdVariantBase<Visitables...> {
public:
    using VariantBase = StdVariantBase<Visitables...>;
    using VariantBase::VariantBase;
    VariantBase& as_std_variant() & noexcept { return *this; }
    const VariantBase& as_std_variant() const& noexcept { return *this; }
};

/**
 * Specialization of VisitableListVariant that unfolds any list-like template into a proper
 * VisitableListVariant. It's intended to be used in case the user is keeping the visitable classes
 * as some templated list for any reason.
 *
 * @tparam AnyList - any template in a form of TemplateName<T1, T2, ...>,
 * ex: 'std::variant<T1, T2>' or 'boost::mpl::list<T1, T2>'
 * */
template <template <typename...> typename AnyList, typename... Visitables>
class VisitableListVariant<AnyList<Visitables...>> : public VisitableListVariant<Visitables...> {
public:
    using VisitableListVariant<Visitables...>::VisitableListVariant;
};

}  // namespace detail

using detail::Overloaded;
using detail::VisitableFor;
using detail::VisitableImpl;
using detail::VisitableListVariant;
}  // namespace vstor

#endif  // VSTOR_HPP