/*
Copyright (C) 2022  Iori Torres (shortanemoia@protonmail.com)
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "Concepts.h"

namespace neo
{
    template<Indexable T, typename... DimensionsSizes> requires ((Integral<DimensionsSizes>&&...))
    class MultidimensionalView
    {
    public:
        MultidimensionalView(T& container, DimensionsSizes... sizes) : m_underlying_container(container)
        {
            size_t i {};
            ((m_dimension_sizes[i++] = sizes) , ...);
        }

    private:
        template<size_t>
        constexpr size_t underlying_index(size_t index)
        {
            return index;
        }
        
        template<size_t CurrentDimensionIndex, typename ... Rest>
        constexpr size_t underlying_index(size_t index, Rest... rest)
        {
            VERIFY(index < m_dimension_sizes[CurrentDimensionIndex]);
            size_t sum {1};
            for(size_t i = 0; i < sizeof...(DimensionsSizes)-CurrentDimensionIndex-1; ++i)
                sum*=m_dimension_sizes[i];
            
            return index * sum + underlying_index<CurrentDimensionIndex+1>(rest...);
        }

        public:
        constexpr auto& operator[](DimensionsSizes... indexes)
        {
            size_t index = underlying_index<0>(indexes...);
            return m_underlying_container[index];
        }
        
        constexpr auto const& operator[](DimensionsSizes... indexes) const
        {
            size_t index = underlying_index<0>(indexes...);
            return m_underlying_container[index];
        }
        
    private:
        size_t m_dimension_sizes[sizeof...(DimensionsSizes)];
        T& m_underlying_container;
    };
}
using neo::MultidimensionalView;
