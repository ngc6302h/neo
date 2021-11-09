/*
* Copyright (C) 2021  Iori Torres (shortanemoia@protonmail.com)
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.

*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.

*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "TypeTraits.h"
namespace neo
{
    template<typename T>
    concept InequalityComparable = requires(T t)
    {
        t != t;
    };

    template<typename T>
    concept EqualityComparable = requires(T t)
    {
        t == t;
    };

    template<typename T>
    concept ThreeWayComparable = requires(T t)
    {
        t <=> t;
    };

    template<typename T>
    concept GreaterThanComparable = requires(T t)
    {
        t > t;
    };

    template<typename T>
    concept GreaterOrEqualThanComparable = requires(T t)
    {
        t >= t;
    };

    template<typename T>
    concept LessThanComparable = requires(T t)
    {
        t < t;
    };

    template<typename T>
    concept LessOrEqualThanComparable = requires(T t)
    {
        t <= t;
    };

    template<typename T>
    concept PrefixIncrementable = requires(T t)
    {
        ++t;
    };

    template<typename T>
    concept PrefixDecrementable = requires(T t)
    {
        --t;
    };

    template<typename T>
    concept PostfixIncrementable = requires(T t)
    {
        t++;
    };

    template<typename T>
    concept PostfixDecrementable = requires(T t)
    {
        t--;
    };

    template<typename T>
    concept Incrementable = PrefixIncrementable<T> && PostfixIncrementable<T>;

    template<typename T>
    concept Decrementable = PrefixDecrementable<T> && PostfixDecrementable<T>;

    template<typename T>
    concept Addable = requires(T t)
    {
        t + t;
    };

    template<typename T>
    concept Subtractable = requires(T t)
    {
        t - t;
    };
    template<typename T>
    concept Multiplicable = requires(T t)
    {
        t* t;
    };

    template<typename T>
    concept Divisible = requires(T t)
    {
        t / t;
    };
    template<typename T>
    concept Dereferenceable = requires(T t)
    {
        *t;
    };

    template<typename T>
    concept Indexable = requires(T t)
    {
        t[0];
    };

    template<typename T>
    concept Iterable = requires(T t)
    {
        t.begin();
        t.end();
    };
    
    template<typename T>
    concept Integral = IsIntegral<T>;
    
    template<typename T>
    concept FloatingPoint = IsFloatingPoint<T>;
    
    template<typename T>
    concept Arithmetic =  Integral<T> || FloatingPoint<T>;
    
    template<typename T>
    concept Enum = __is_enum(T);
    
    template<typename T>
    concept Pointer = IsPointer<T>;
    
    template<typename T>
    concept NullPtr = IsNullptr<T>;
    
    //FIXME: Implement
    //template<typename T>
    //concept MemberPointer =

    template<typename T>
    concept Scalar = Arithmetic<T> || Enum<T> || Pointer<T> || NullPtr<T>; //|| MemberPointer<T>;
    
    template<typename T>
    concept CopyConstructable = requires(const T& t)
    {
        T(t);
    };

    template<typename T>
    concept CopyAssignable = requires(T t, const T& u)
    {
        t = u;
    };

    template<typename T>
    concept MoveConstructable = requires(T t, T& u)
    {
        T(static_cast<T&&>(u));
    };

    template<typename T>
    concept MoveAssignable = requires(T t, T& u)
    {
        t = static_cast<T&&>(u);
    };

    template<typename T, typename U>
    concept SameAs = IsSame<T, U>;
    
    template<typename TBase, typename TDerived>
    concept BaseOf = requires(TDerived* d)
    {
        static_cast<TBase*>(d);
    };

    template<typename TCallable, typename... Args>
    concept Callable = requires(TCallable f, Args... args)
    {
        f(args...);
    };

    template<typename TCallable, typename TReturn, typename... Args>
    concept CallableWithReturnType = requires(TCallable f, Args... args)
    {
        {
            f(forward<Args>(args)...)
            } -> SameAs<TReturn>;
    };

    template<typename T>
    concept Container = requires(T t)
    {
        typename T::type;
    };

    template<typename T>
    concept IterableContainer = Iterable<T> && Container<T>;

    template<typename T>
    concept FixedContainer = requires(T t)
    {
        typename T::type;
        T::size();
        t.template get<0>();
    };

    template<template<typename... Ts> class T, typename TExample = int>
    concept TupleLike = FixedContainer<T<TExample>> && requires(TExample u)
    {
        make_tuple<T>(u);
        typename T<TExample>::template template_type<TExample>;
    };
}
using neo::Addable;
using neo::Callable;
using neo::CallableWithReturnType;
using neo::Container;
using neo::CopyAssignable;
using neo::CopyConstructable;
using neo::Decrementable;
using neo::Dereferenceable;
using neo::Divisible;
using neo::EqualityComparable;
using neo::FixedContainer;
using neo::GreaterOrEqualThanComparable;
using neo::GreaterThanComparable;
using neo::Incrementable;
using neo::Indexable;
using neo::InequalityComparable;
using neo::Iterable;
using neo::Integral;
using neo::FloatingPoint;
using neo::Arithmetic;
using neo::Enum;
using neo::Pointer;
using neo::NullPtr;
using neo::Scalar;
using neo::IterableContainer;
using neo::LessOrEqualThanComparable;
using neo::LessThanComparable;
using neo::MoveAssignable;
using neo::MoveConstructable;
using neo::BaseOf;
using neo::Multiplicable;
using neo::SameAs;
using neo::Subtractable;
using neo::ThreeWayComparable;
