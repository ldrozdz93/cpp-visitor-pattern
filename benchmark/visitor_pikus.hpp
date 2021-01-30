/**
 * Source code copied under MIT license from:
 * https://github.com/PacktPublishing/Hands-On-Design-Patterns-with-CPP/blob/master/Chapter18/04b_visitor_template
 *
MIT License

Copyright (c) 2018 Packt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 * */

#pragma once

namespace pikus {
// ---------------------------------
// Generic visitor base
template <typename... Types>
class Visitor;

template <typename T>
class Visitor<T> {
public:
    virtual void visit(T* t) = 0;
};

template <typename T, typename... Types>
class Visitor<T, Types...> : public Visitor<Types...> {
public:
    using Visitor<Types...>::visit;
    virtual void visit(T* t) = 0;
};

// ----------------------------------------------
//
template <typename Base, typename...>
class LambdaVisitor;

template <typename Base, typename T1, typename F1>
class LambdaVisitor<Base, Visitor<T1>, F1> : private F1, public Base {
public:
    LambdaVisitor(F1&& f1) : F1(std::move(f1)) {}
    LambdaVisitor(const F1& f1) : F1(f1) {}
    void visit(T1* t) override { return F1::operator()(t); }
};

template <typename Base, typename T1, typename... T, typename F1, typename... F>
class LambdaVisitor<Base, Visitor<T1, T...>, F1, F...>
    : private F1, public LambdaVisitor<Base, Visitor<T...>, F...> {
public:
    LambdaVisitor(F1&& f1, F&&... f)
        : F1(std::move(f1)), LambdaVisitor<Base, Visitor<T...>, F...>(std::forward<F>(f)...)
    {
    }
    LambdaVisitor(const F1& f1, F&&... f)
        : F1(f1), LambdaVisitor<Base, Visitor<T...>, F...>(std::forward<F>(f)...)
    {
    }
    void visit(T1* t) override { return F1::operator()(t); }
};

template <typename Base, typename... F>
auto lambda_visitor(F&&... f)
{
    return LambdaVisitor<Base, Base, F...>(std::forward<F>(f)...);
}
//
// ----------------------------------------------

}  // namespace pikus