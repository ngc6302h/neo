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

namespace neo
{
    namespace detail
    {
        struct generator_end_t
        {
        };
    }

    template<typename T, typename TState, typename TPump, typename TValueExtractor>
    class GeneratorIterator
    {
    public:
        constexpr GeneratorIterator(TPump pump, TValueExtractor extractor, TState initial_state = {}, bool exhausted = false) :
            m_pump(pump), m_value_extractor(extractor), m_state(initial_state), m_exhausted(exhausted) { }

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

        constexpr bool is_end() const
        {
            return m_exhausted;
        }

        constexpr bool operator==(GeneratorIterator const& right)
        {
            if (m_state == right.m_state)
                return true;
            return false;
        }

        constexpr bool operator==(detail::generator_end_t const&)
        {
            return is_end();
        }

    private:
        TPump m_pump;
        TValueExtractor m_value_extractor;
        TState m_state;
        bool m_exhausted;
    };

    template<typename T, typename TState = T>
    struct GeneratorUtil;

    template<typename T, typename TState, typename TPump, typename TValueExtractor>
    class Generator
    {
        friend GeneratorUtil<T, TState>;

    public:
        constexpr GeneratorIterator<T, TState, TPump, TValueExtractor> begin() const
        {
            return GeneratorIterator<T, TState, TPump, TValueExtractor>(m_pump, m_value_extractor, m_initial_state, false);
        }

        constexpr detail::generator_end_t end() const
        {
            return {};
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
            TPump pump, TValueExtractor extractor = [](TState& s) -> T
            { return s; },
            TState initial_state = {}) :
            m_pump(pump), m_value_extractor(extractor), m_initial_state(initial_state)
        {
        }

        TPump m_pump;
        TValueExtractor m_value_extractor;
        TState m_initial_state;
    };

    template<typename T, typename TState>
    struct GeneratorUtil
    {
        static constexpr auto create_generator(auto pump, auto extractor, TState initial_state = {})
        {
            return Generator<T, TState, decltype(pump), decltype(extractor)>(pump, extractor, initial_state);
        }
    };

    // Default generators

    namespace Generators
    {
        template<Integral T, T InitialValue = 0>
        static constexpr auto integer_sequence = GeneratorUtil<T, T>::create_generator([](T& state, bool&)
            { return state++; },
            [](T& state)
            { return state; },
            InitialValue);

    }
}
using neo::Generator;
using neo::GeneratorIterator;
using neo::GeneratorUtil;
