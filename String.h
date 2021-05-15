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

#include "Types.h"
#include "Iterator.h"
#include "AsciiString.h"
#include "Optional.h"
#include "Assert.h"
#include <string.h>

namespace neo
{
    using Utf8Char = u32;
    
    struct StringIteratorContainer
    {
        char* data;
        //using TReturn = Utf8Char;
    
        constexpr StringIteratorContainer(char* ptr) : data(ptr) {}
        
        constexpr Utf8Char operator*() const
        {
            int codepoint_size;
            char *ptr = data;
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
            switch (codepoint_size)
            {
                case 1:
                    return ptr[0];
                case 2:
                    return (((u32) ptr[1]) & 0x3F) | (((u32) ptr[0]) & 0x1F) << 6;
                case 3:
                    return (((u32) ptr[2]) & 0x3F) | (((u32) ptr[1]) & 0x3F) << 6 | (((u32) ptr[0]) & 0xF) << 12;
                case 4:
                    return (((u32) ptr[3]) & 0x3F) | (((u32) ptr[2]) & 0x3F) << 6 | (((u32) ptr[1]) & 0x3F) << 12 |
                           (((u32) ptr[0]) & 7) << 18;
                default:
                    __builtin_unreachable();
            }
        }
    
        constexpr StringIteratorContainer& operator++()
        {
            char *ptr = data;
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
            data = ptr;
            return *this;
        }
    
        constexpr StringIteratorContainer operator++(int)
        {
            auto prev = *this;
            char *ptr = data;
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
            data = ptr;
            return prev;
        }
    
        constexpr StringIteratorContainer& operator--()
        {
            while ((((*data) >> 6) & 3) == 2)
                data--;
            return *this;
        }
    
        constexpr StringIteratorContainer operator--(int)
        {
            auto prev = *this;
            while ((((*data) >> 6) & 3) == 2)
                data--;
            return prev;
        }
    
        constexpr bool operator!=(const StringIteratorContainer &other) const
        {
            return data != other.data;
        }
    
        constexpr bool operator==(const StringIteratorContainer &other) const
        {
            return data == other.data;
        }
    };
    
    using StringBidIt = BidirectionalIterator<char*, StringIteratorContainer>;
    
    class String
    {
    public:
        explicit constexpr String() = default;
    
        constexpr String(const String& other) : m_byte_length(other.m_byte_length)
        {
            m_buffer = new char[other.m_byte_length+1];
            m_buffer[other.m_byte_length] = 0;
            if (this_is_constexpr())
            {
                for(size_t i = 0; i < other.m_byte_length; i++)
                    m_buffer[i] = other.m_buffer[i];
            }
            else
                memcpy(m_buffer, other.m_buffer, other.m_byte_length);
        }
    
        constexpr String(String&& other)
        {
            m_buffer = other.m_buffer;
            other.m_buffer = nullptr;
            other.m_byte_length = 0;
        }
    
        constexpr String(const AsciiString& other) : m_byte_length(other.size())
        {
            m_buffer = new char[other.size()+1];
            m_buffer[other.size()] = 0;
            m_byte_length = other.size();
            if (this_is_constexpr())
            {
                for(size_t i = 0; i < other.size(); i++)
                    m_buffer[i] = other.m_buffer[i];
            }
            else
                memcpy(m_buffer, other.m_buffer, other.size());
        }
    
        constexpr String(AsciiString&& other) : m_byte_length(other.size())
        {
            m_buffer = other.m_buffer;
            m_byte_length = other.m_length;
            other.m_length = 0;
        }
    
         constexpr String(const char* cstring)
        {
            size_t length;
            if (this_is_constexpr())
            {
                length = 0;
                while (cstring[length] != 0) length++;
            }
            else length = strlen(cstring);
            
            VERIFY(length!=0);
            
            m_buffer = new char[length+1];
            m_buffer[length] = 0;
            m_byte_length = length;
            if (this_is_constexpr())
            {
                for(size_t i = 0; i < length; i++)
                    m_buffer[i] = cstring[i];
            }
            else
                memcpy(m_buffer, cstring, length);
        }
        
        constexpr String(const char* cstring, size_t length)
        {
            size_t size = length;
            VERIFY(size != 0);
        
            m_buffer = new char[size+1];
            m_buffer[size] = 0;
            m_byte_length = size;
            for(size_t i = 0; i < size; i++)
                m_buffer[i] = cstring[i];
        }
        
        constexpr String& operator=(const String& other)
        {
            if (&other == this)
                return *this;
            
            delete[] m_buffer;
            m_buffer = new char[other.m_byte_length];
            m_byte_length = other.m_byte_length;
            if (this_is_constexpr())
            {
                for(size_t i = 0; i < other.m_byte_length; i++)
                    m_buffer[i] = other.m_buffer[i];
            }
            else
            {
                memcpy(m_buffer, other.m_buffer, other.m_byte_length);
            }
            return *this;
        }
    
        constexpr String& operator=(String&& other)
        {
            if (&other == this)
                return *this;
            
            delete[] m_buffer;
            m_buffer = other.m_buffer;
            other.m_buffer = nullptr;
            m_byte_length = other.m_byte_length;
            other.m_byte_length = 0;
            return *this;
        }
        
        [[nodiscard]] constexpr operator char*() const
        {
            return m_buffer;
        }
        
        [[nodiscard]] constexpr bool operator==(const String& other) const
        {
            if (m_byte_length != other.m_byte_length)
                return false;
            for (size_t i = 0; i < m_byte_length; i++)
            {
                if (m_buffer[i] != other.m_buffer[i])
                    return false;
            }
            return true;
        }
    
        [[nodiscard]] constexpr bool operator!=(const String& other) const
        {
            return !(*this == other);
        }
    
        [[nodiscard]] constexpr int operator<=>(const String& other) const
        {
            auto begin = cbegin();
            auto other_begin = other.cbegin();
            for (; *begin == *other_begin; begin++, other_begin++)
            {
                if (*begin == 0)
                    return 0;
            }
            
            return *begin < *other_begin ? -1 : 1;
        }
        
        [[nodiscard]] constexpr const StringBidIt cbegin() const
        {
            return StringBidIt(m_buffer);
        }
    
        [[nodiscard]] constexpr StringBidIt begin() const
        {
            return StringBidIt(m_buffer);
        }
        
        [[nodiscard]] constexpr const StringBidIt cend() const
        {
            return StringBidIt(m_buffer + m_byte_length);
        }
    
        [[nodiscard]] constexpr StringBidIt end() const
        {
            return StringBidIt(m_buffer + m_byte_length);
        }
        
        //Size in bytes
        [[nodiscard]] constexpr size_t size() const
        {
            return m_byte_length;
        }
        
        [[nodiscard]] constexpr bool starts_with(const String& other)
        {
            if (!size() || !other.size() || other.size() > size())
                return false;
            
            for(auto other_char : other)
            {
                for (auto my_char : *this)
                {
                    if (my_char != other_char)
                        return false;
                }
            }
            return true;
        }
        
        [[nodiscard]] constexpr bool ends_with(const String& other) const
        {
            if (!size() || !other.size() || other.size() > size())
                return false;
    
            for(auto other_char = other.cend(); other_char != other.cbegin(); other_char--)
            {
                for (auto my_char = cend(); my_char != begin(); my_char--)
                {
                    if (my_char != other_char)
                        return false;
                    other_char--;
                }
            }
            return true;
        }
        
        [[nodiscard]] constexpr Optional<StringBidIt> contains(const String& other) const
        {
            if (!size() || !other.size() || size() < other.size())
                return {};
    
            for(auto my_char = begin(); my_char != end(); my_char++)
            {
                bool found = true;
                for (auto other_char : other)
                {
                    if (*my_char != other_char)
                    {
                        found = false;
                        break;
                    }
                    my_char++;
                }
                if (found) return { my_char };
            }
            return {};
        }
    
        [[nodiscard]] constexpr Span<char> span()
        {
            return { m_buffer,  m_byte_length };
        }
    
    private:
        char* m_buffer { nullptr };
        size_t m_byte_length { 0 };
    };
    
    [[nodiscard]] constexpr String operator""_s(const char* cstring, size_t length)
    {
        return String(cstring, length);
    }
    
    [[nodiscard]] constexpr size_t codepoint_length_of(const String& s)
    {
        auto begin = s.begin();
        auto end = s.end();
        size_t count = 0;
        while (begin++ != end)
            count++;
        return count;
    }
    
    struct StringHasher
    {
        static constexpr size_t hash(const AsciiString& str)
        {
            char* data =  (char*) str;
            size_t size = str.size();
            size_t result = data[size-1];
            while(size--) result += result ^ data[size] ^ (~(result*result+3241));
            return result;
        }
        
        static constexpr size_t hash(const String& str)
        {
            char* data = (char*) str;
            size_t size = str.size();
            size_t result = data[size-1];
            while(size--) result += result ^ data[size] ^ (~(result*result+3241));
            return result;
        }
    };
}
using neo::String;
using neo::Utf8Char;
using neo::operator""_s;
using neo::StringHasher;