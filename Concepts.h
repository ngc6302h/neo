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

namespace neo
{
    template<typename T>
    concept InequalityComparable =
    requires(T t) {
        t != t;
    };
    
    template<typename T>
    concept EqualityComparable =
    requires(T t) {
        t == t;
    };
    
    template<typename T>
    concept ThreeWayComparable =
    requires(T t) {
        t <=> t;
    };
    
    template<typename T>
    concept GreaterThanComparable =
    requires(T t) {
        t > t;
    };
    
    template<typename T>
    concept GreaterOrEqualThanComparable =
    requires(T t) {
        t >= t;
    };
    
    template<typename T>
    concept LessThanComparable =
    requires(T t) {
        t < t;
    };
    
    template<typename T>
    concept LessOrEqualThanComparable =
    requires(T t) {
        t <= t;
    };
    
    template<typename T>
    concept PrefixIncrementable =
    requires(T t) {
        ++t;
    };
    
    template<typename T>
    concept PrefixDecrementable =
    requires(T t) {
        --t;
    };
    
    template<typename T>
    concept PostfixIncrementable =
    requires(T t) {
        t++;
    };
    
    template<typename T>
    concept PostfixDecrementable =
    requires(T t) {
        t--;
    };
    
    template<typename T>
    concept Incrementable = PrefixIncrementable<T> && PostfixIncrementable<T>;
    
    template<typename T>
    concept Decrementable = PrefixDecrementable<T> && PostfixDecrementable<T>;
    
    template<typename T>
    concept Addable =
    requires(T t) {
        t + t;
    };
    
    template<typename T>
    concept Subtractable =
    requires(T t) {
        t - t;
    };
    template<typename T>
    concept Multiplicable =
    requires(T t) {
        t * t;
    };
    
    template<typename T>
    concept Divisible =
    requires(T t) {
        t / t;
    };
    template<typename T>
    concept Dereferenceable =
    requires(T t) {
        *t;
    };
    
    template<typename T>
    concept Indexable =
    requires(T t) {
        t[0];
    };
    
    template<typename T>
    concept Iterable =
    requires(T t) {
       t.begin();
       t.end();
    };
    
    template<typename T>
    concept CopyConstructable =
    requires(const T& t) {
        T(t);
    };
    
    template<typename T>
    concept CopyAssignable =
    requires(T t, const T& u) {
        t = u;
    };
    
    template<typename T>
    concept MoveConstructable =
    requires(T t, T& u) {
         T(static_cast<T&&>(u));
    };
    
    template<typename T>
    concept MoveAssignable =
    requires(T t, T& u) {
        t = static_cast<T&&>(u);
    };
}