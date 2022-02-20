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
#include "Concepts.h"
#include "Assert.h"
#include "NumericLimits.h"

namespace neo
{
    template<typename T, typename TState, typename TPump, typename TRewindFunc, typename TValueExtractor>
    class GeneratorIterator
    {
    public:
        using type = T;
        using iterator_type = GeneratorIterator;
        using underlying_container_type = GeneratorIterator;
        constexpr GeneratorIterator(TPump pump, TRewindFunc rewind, TValueExtractor extractor, TState initial_state, bool exhausted = false) :
            m_pump(pump), m_rewind(rewind), m_value_extractor(extractor), m_state(initial_state), m_exhausted(exhausted) { }

        constexpr T operator*()
        {
            return m_value_extractor(m_state);
        }

        constexpr GeneratorIterator& operator++()
        {
            m_pump(m_state, m_exhausted);
            return *this;
        }

        constexpr GeneratorIterator operator++(int)
        {
            auto prev = *this;
            m_pump(m_state, m_exhausted);
            return prev;
        }

        constexpr GeneratorIterator& operator--()
        {
            m_rewind(m_state, m_exhausted);
            return *this;
        }

        constexpr GeneratorIterator operator--(int)
        {
            auto current = *this;
            m_rewind(m_state, m_exhausted);
            return current;
        }

        constexpr bool is_end() const
        {
            return m_exhausted;
        }

        constexpr bool operator==(GeneratorIterator const& right) const
        {
            if (m_state == right.m_state)
                return true;
            return false;
        }

    private:
        TPump m_pump;
        TRewindFunc m_rewind;
        TValueExtractor m_value_extractor;
        TState m_state;
        bool m_exhausted;
    };

    template<typename T, typename TState = T>
    struct GeneratorUtil;

    template<typename T, typename TState, typename TPump, typename TRewindFunc, typename TValueExtractor>
    class Generator
    {
        friend GeneratorUtil<T, TState>;

    public:
        constexpr auto begin() const
        {
            return GeneratorIterator<T, TState, TPump, TRewindFunc, TValueExtractor>(m_pump, m_rewind, m_value_extractor, m_initial_state, false);
        }

        constexpr auto end() const
        {
            return GeneratorIterator<T, TState, TPump, TRewindFunc, TValueExtractor>(m_pump, m_rewind, m_value_extractor, m_end_state, true);
        }

        constexpr size_t generate_into(auto range_start, auto range_end)
        {
            size_t count {};
            auto start = begin();
            auto _end = end();
            while (range_start != range_end && start != _end)
            {
                *range_start++ = *start++;
                count++;
            }
            return count;
        }

    private:
        explicit constexpr Generator(
            TPump pump, TRewindFunc rewind, TValueExtractor extractor,
            TState initial_state,
            TState end_state) :
            m_pump(pump), m_rewind(rewind), m_value_extractor(extractor), m_initial_state(initial_state), m_end_state(end_state)
        {
        }

        TPump m_pump;
        TRewindFunc m_rewind;
        TValueExtractor m_value_extractor;
        TState m_initial_state;
        TState m_end_state;
    };

    template<typename T, typename TState>
    struct GeneratorUtil
    {
        static constexpr auto create_generator(auto pump, auto rewind, auto extractor, TState initial_state, TState end_state)
        {
            return Generator<T, TState, decltype(pump), decltype(rewind), decltype(extractor)>(pump, rewind, extractor, initial_state, end_state);
        }
    };

    // Default generators

    namespace Generators
    {
        template<Integral T>
        constexpr auto integer_sequence(T initial_value)
        {
            return GeneratorUtil<T, T>::create_generator([](T& state, bool&)
                { return state++; },
                [](T& state, bool&)
                {
                    return state--;
                },
                [](T& state)
                { return state; },
                initial_value,
                NumericLimits<T>::max());
        }

    }
}
using neo::Generator;
using neo::GeneratorIterator;
using neo::GeneratorUtil;
