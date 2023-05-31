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
#include "Vector.h"
#include "Optional.h"

namespace neo
{
    template<typename T>
    class CircularBuffer
    {
    public:
        explicit CircularBuffer(size_t capacity) :
            m_buffer(Vector<T>::create_with_capacity(capacity))
        {
        }

        void enqueue(T const& value)
        {
            if (m_write_index == m_buffer.capacity() && m_read_index == 0)
                m_buffer.resize(m_buffer.size() * 2);
            m_buffer[m_write_index++] = value;
        }

        void enqueue(T&& value)
        {
            if (m_write_index == m_buffer.capacity() && m_read_index == 0)
                m_buffer.change_size(m_buffer.size() * 2);
            m_buffer[m_write_index++] = std::move(value);
        }

        Optional<T> dequeue()
        {
            if (m_read_index == m_write_index)
                return {};
            size_t previous = m_read_index++;
            if (m_read_index == m_buffer.capacity())
                m_read_index = 0;
            return { std::move(m_buffer[previous]) };
        }
        
        size_t size() const
        {
            if (m_write_index > m_read_index)
                return m_write_index - m_read_index;
            else
                return m_buffer.size() - m_read_index + m_write_index;
        }

    private:
        Vector<T> m_buffer;
        size_t m_read_index {};
        size_t m_write_index {};
    };
}
using neo::CircularBuffer;
