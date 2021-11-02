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
#include "String.h"
#include "StringCommon.h"
#include "Text.h"

namespace neo
{
    class StringBuilder
    {
    public:
        static constexpr size_t DEFAULT_CAPACITY = 32;

        StringBuilder() :
            m_string(new char[DEFAULT_CAPACITY]), m_current_offset(0), m_capacity(DEFAULT_CAPACITY)
        {
        }

        explicit StringBuilder(const StringView& string) :
            m_string(new char[max(DEFAULT_CAPACITY, string.byte_size())]), m_current_offset(string.byte_size()), m_capacity(max(DEFAULT_CAPACITY, string.byte_size()))
        {
            __builtin_memcpy(m_string, string.non_null_terminated_buffer(), string.byte_size());
        }

        ~StringBuilder()
        {
            delete[] m_string;
        }

        [[nodiscard]] String to_string() const
        {
            return String(m_string, m_current_offset);
        }

        StringBuilder& append(const String& string)
        {
            if (m_current_offset + string.byte_size() < m_capacity)
                resize(max(m_capacity, string.byte_size()) * 2);
            __builtin_memcpy(m_string + m_current_offset, string.null_terminated_characters(), string.byte_size());
            m_current_offset += string.byte_size();
            return *this;
        }

        StringBuilder& append(const char* cstring)
        {
            size_t length = __builtin_strlen(cstring);
            if (m_current_offset + length < m_capacity)
                resize(max(m_capacity, length) * 2);
            __builtin_memcpy(m_string + m_current_offset, cstring, length);
            m_current_offset += length;
            return *this;
        }

        StringBuilder& append(char ch)
        {
            if (m_current_offset + 1 < m_capacity)
                resize(m_capacity * 2);
            m_string[m_current_offset++] = ch;
            return *this;
        }

        StringBuilder& remove(const StringView& what)
        {
            VERIFY(!what.is_empty());
            VERIFY(m_current_offset >= what.byte_size());
            for (ssize_t i = m_current_offset - what.byte_size() + 1; i >= 0; i--)
            {
                if (__builtin_memcmp(m_string + i, what.non_null_terminated_buffer(), what.byte_size()) == 0)
                {
                    __builtin_memcpy(m_string + i, m_string + i + what.byte_size(), m_current_offset - i);
                    m_current_offset -= what.byte_size();
                }
            }
            return *this;
        }

        StringBuilder& replace(const StringView& what, const StringView& with)
        {
            VERIFY(!what.is_empty());
            if (with.is_empty())
            {
                remove(what);
                return *this;
            }

            VERIFY(m_current_offset >= what.byte_size());

            //Same length optimization
            if (what.byte_size() == with.byte_size())
            {
                for (size_t i = 0; i < m_current_offset;)
                {
                    if (__builtin_memcmp(m_string + i, what.non_null_terminated_buffer(), what.byte_size()) == 0)
                    {
                        __builtin_memcpy(m_string + i, with.non_null_terminated_buffer(), what.byte_size());
                        i += what.byte_size();
                    }
                    else
                        i++;
                }
                return *this;
            }

            Vector<size_t> hits;
            for (size_t i = 0; i <= m_current_offset - what.byte_size();)
            {
                if (__builtin_memcmp(m_string + i, what.non_null_terminated_buffer(), what.byte_size()) == 0)
                {
                    hits.append(i);
                    i += what.byte_size();
                }
                else
                    i++;
            }

            if (hits.size() == 0)
                return *this;

            size_t new_size = max(m_current_offset - hits.size() * what.byte_size() + hits.size() * with.byte_size(), m_capacity);
            char* new_buffer = new char[new_size];

            size_t new_current_offset = 0;
            size_t old_current_offset = 0;

            for (auto i : hits)
            {
                __builtin_memcpy(new_buffer + new_current_offset, m_string + old_current_offset, i - old_current_offset);
                new_current_offset += i - old_current_offset;
                old_current_offset += i - old_current_offset;
                __builtin_memcpy(new_buffer + new_current_offset, with.non_null_terminated_buffer(), with.byte_size());
                new_current_offset += with.byte_size();
                old_current_offset += what.byte_size();
            }

            __builtin_memcpy(new_buffer + new_current_offset, m_string + old_current_offset, m_current_offset - hits[hits.size() - 1] - 1);

            delete[] m_string;
            m_string = new_buffer;
            m_current_offset = new_size;
            m_capacity = new_size;
            return *this;
        }

        StringBuilder& trim_whitespace(TrimMode from_where)
        {
            if ((from_where & TrimMode::End) == TrimMode::End)
            {
                auto end = --StringIterator(m_string, m_string + m_current_offset, m_string + m_current_offset);
                while (isspace(*end))
                    --end;
                m_current_offset = end.ptr() - m_string;
            }

            if ((from_where & TrimMode::Start) == TrimMode::Start)
            {
                auto start = StringIterator(m_string, m_string + m_current_offset, m_string);
                while (isspace(*start))
                    ++start;
                m_current_offset = m_string + m_current_offset - start.ptr() + 1;
                __builtin_memcpy(m_string, start.ptr(), m_current_offset);
            }
            return *this;
        }

        void clear()
        {
            m_current_offset = 0;
        }

    private:
        void resize(size_t new_size)
        {
            m_capacity = new_size;
            char* new_buffer = new char[new_size];
            __builtin_memcpy(new_buffer, m_string, m_current_offset);
            delete[] m_string;
            m_string = new_buffer;
        }

        char* m_string { nullptr };
        size_t m_current_offset { 0 };
        size_t m_capacity { 0 };
    };
}
using neo::StringBuilder;
