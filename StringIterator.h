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
#include "Iterator.h"
#include "Types.h"

namespace neo
{
    using Utf8Char = u32;
    
    class StringIteratorContainer
    {
    public:
        explicit constexpr StringIteratorContainer(const char* base, const char* end, const char* current) :
                m_base(base),
                m_end(end),
                m_current(current)
        {
        }

        constexpr Utf8Char operator*() const
        {
            VERIFY(!is_end());
            //TODO: Research how to make this branchless
            int codepoint_size;
            const char* ptr = m_current;
            if (((*ptr >> 7) & 1) == 0)
                codepoint_size = 1;
            else if (((*ptr >> 5) & 7) == 6)
                codepoint_size = 2;
            else if (((*ptr >> 4) & 0xF) == 0xE)
                codepoint_size = 3;
            else if (((*ptr >> 3) & 0x1F) == 0x1E)
                codepoint_size = 4;
            else
                codepoint_size = 1;
            Utf8Char codepoint;
            switch (codepoint_size)
            {
            case 1:
                codepoint = ptr[0];
                break;
            case 2:
                codepoint = (((u32)ptr[1]) & 0x3F) | (((u32)ptr[0]) & 0x1F) << 6;
                break;
            case 3:
                codepoint = (((u32)ptr[2]) & 0x3F) | (((u32)ptr[1]) & 0x3F) << 6 | (((u32)ptr[0]) & 0xF) << 12;
                break;
            case 4:
                codepoint = (((u32)ptr[3]) & 0x3F) | (((u32)ptr[2]) & 0x3F) << 6 | (((u32)ptr[1]) & 0x3F) << 12 | (((u32)ptr[0]) & 7) << 18;
                break;
            default:
                VERIFY_NOT_REACHED();
            }
            VERIFY(codepoint != 0);
            return codepoint;
        }

        constexpr StringIteratorContainer& operator++()
        {
            VERIFY(!is_end());
            const char* ptr = m_current;
            if (((*ptr >> 7) & 1) == 0)
                ptr++;
            else if (((*ptr >> 5) & 7) == 6)
                ptr += 2;
            else if (((*ptr >> 4) & 0xF) == 0xE)
                ptr += 3;
            else if (((*ptr >> 3) & 0x1F) == 0x1E)
                ptr += 4;
            else
                ptr++; //TODO: Verify this is correct behavior
            m_current = ptr;
            return *this;
        }

        constexpr StringIteratorContainer operator++(int)
        {
            VERIFY(!is_end());
            auto prev = *this;
            const char* ptr = m_current;
            if (((*ptr >> 7) & 1) == 0)
                ptr++;
            else if (((*ptr >> 5) & 7) == 6)
                ptr += 2;
            else if (((*ptr >> 4) & 0xF) == 0xE)
                ptr += 3;
            else if (((*ptr >> 3) & 0x1F) == 0x1E)
                ptr += 4;
            else
                ptr++; //TODO: verify this is correct behavior
            m_current = ptr;
            return prev;
        }

        constexpr StringIteratorContainer& operator--()
        {
            VERIFY(m_current > m_base);
            do
                m_current--;
            while ((((*m_current) >> 6) & 3) == 2);
            return *this;
        }

        constexpr StringIteratorContainer operator--(int)
        {
            VERIFY(m_current > m_base);
            auto prev = *this;
            do
                m_current--;
            while ((((*m_current) >> 6) & 3) == 2);
            return prev;
        }
    
        constexpr bool is_end() const
        {
            return *m_current == 0 || m_current == m_end;
        }
        
        constexpr const char* ptr() const
        {
            return m_current;
        }

        constexpr bool operator!=(const StringIteratorContainer& other) const
        {
            return m_current != other.m_current;
        }

        constexpr bool operator==(const StringIteratorContainer& other) const
        {
            return m_current == other.m_current;
        }

    private:
        const char* m_base;
        const char* m_end;
        const char* m_current;
    };
    using StringIterator = StringIteratorContainer;
    using StringViewIterator = StringIteratorContainer;

}
using neo::StringIterator;
using neo::StringViewIterator;
using neo::Utf8Char;
