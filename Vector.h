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

#include "Util.h"
#include "Assert.h"
#include "IterableUtil.h"
#include "Iterator.h"
#include "Span.h"
#include "TypeTags.h"
#include "TypeTraits.h"
#include "Types.h"
#include "Concepts.h"
#include "New.h"

namespace neo
{
    template<typename T, size_t InlineStorage = 0 /*, typename Allocator = neo::DefaultAllocator */>
    class Vector : public IContainer<Vector, T>, public IterableExtensions<Vector<T, InlineStorage>, RemoveReferenceWrapper<T>>
    {
        template<typename, size_t>
        friend class Vector;

    public:
        using iterator = Iterator<Vector>;
        using const_iterator = Iterator<const Vector>;
        static constexpr size_t DEFAULT_SIZE { 16 };
        static constexpr size_t InlineStorageSize = InlineStorage;

        constexpr Vector() :
            m_capacity(DEFAULT_SIZE)
        {
            allocate(DEFAULT_SIZE);
        }

        ~Vector()
        {
            clean();
            deallocate();
        }

        explicit constexpr Vector(size_t initial_capacity, bool resize_to_capacity = false) :
            m_capacity(initial_capacity), m_size(resize_to_capacity ? initial_capacity : 0)
        {
            VERIFY(initial_capacity > 0);
            allocate(initial_capacity);
        }

        template<size_t OtherInlineSize>
        constexpr Vector(Vector<T, OtherInlineSize> const& other)
        {
            clean();
            ensure_capacity(max(1UL, other.size()));

            if constexpr (InlineStorage != 0)
            {
                if (other.size() < InlineStorage)
                {
                    if constexpr (OtherInlineSize != 0)
                    {
                        Copy(other.size(), other.inline_storage(), inline_storage());
                        if (other.size() > OtherInlineSize)
                            Copy(other.size() - OtherInlineSize, other.m_data, inline_storage() + OtherInlineSize);
                    }
                    else
                    {
                        Copy(other.size(), other.m_data, inline_storage());
                    }
                }
                else
                {
                    if constexpr (OtherInlineSize != 0)
                    {
                        if constexpr (InlineStorage > OtherInlineSize)
                        {
                            Copy(OtherInlineSize, other.inline_storage(), inline_storage());
                            if (other.m_size < InlineStorage)
                                Copy(other.m_size - OtherInlineSize, other.m_data, inline_storage() + OtherInlineSize);
                            else
                            {
                                Copy(InlineStorage - OtherInlineSize, other.m_data, inline_storage() + OtherInlineSize);
                                Copy(other.m_size - InlineStorage, other.m_data + InlineStorage - OtherInlineSize, m_data);
                            }
                        }
                        else
                        {
                            Copy(InlineStorage, other.inline_storage(), inline_storage());
                            if (other.m_size > OtherInlineSize)
                            {
                                Copy(OtherInlineSize - InlineStorage, other.inline_storage() + InlineStorage, m_data);
                                Copy(other.m_size - OtherInlineSize, other.m_data, m_data + OtherInlineSize);
                            }
                            else
                            {
                                Copy(other.m_size - InlineStorage, other.inline_storage() + InlineStorage, m_data);
                            }
                        }
                    }
                    else
                    {
                        Copy(InlineStorage, other.m_data, inline_storage());
                        if (InlineStorage < other.m_size)
                            Copy(other.m_size - InlineStorage, other.m_data + InlineStorage, m_data);
                    }
                }
            }
            else
            {
                if constexpr (OtherInlineSize != 0)
                {
                    Copy(min(other.m_size, OtherInlineSize), other.inline_storage(), m_data);
                    if (other.m_size > OtherInlineSize)
                        Copy(other.m_size - OtherInlineSize, other.m_data, m_data + OtherInlineSize);
                }
                else
                {
                    Copy(other.m_size, other.m_data, m_data);
                }
            }
            m_size = other.m_size;
        }

        template<size_t OtherInlineSize>
        constexpr Vector(Vector<T, OtherInlineSize>&& other)
        {
            clean();
            ensure_capacity(max(1UL, other.size()));

            if (m_size != 0)
            {
                if constexpr (InlineStorage != 0)
                {
                    if (other.size() < InlineStorage)
                    {
                        if constexpr (OtherInlineSize != 0)
                        {
                            MoveOrCopy(other.size(), other.inline_storage(), inline_storage());
                            if (other.size() > OtherInlineSize)
                                MoveOrCopy(other.size() - OtherInlineSize, other.m_data,
                                    inline_storage() + OtherInlineSize);
                        }
                        else
                        {
                            MoveOrCopy(other.size(), other.m_data, inline_storage());
                        }
                    }
                    else
                    {
                        if constexpr (OtherInlineSize != 0)
                        {
                            if constexpr (InlineStorage > OtherInlineSize)
                            {
                                MoveOrCopy(OtherInlineSize, other.inline_storage(), inline_storage());
                                if (other.m_size < InlineStorage)
                                    MoveOrCopy(other.m_size - OtherInlineSize, other.m_data,
                                        inline_storage() + OtherInlineSize);
                                else
                                {
                                    MoveOrCopy(InlineStorage - OtherInlineSize, other.m_data,
                                        inline_storage() + OtherInlineSize);
                                    MoveOrCopy(other.m_size - InlineStorage,
                                        other.m_data + InlineStorage - OtherInlineSize, m_data);
                                }
                            }
                            else
                            {
                                MoveOrCopy(InlineStorage, other.inline_storage(), inline_storage());
                                if (other.m_size > OtherInlineSize)
                                {
                                    MoveOrCopy(OtherInlineSize - InlineStorage, other.inline_storage() + InlineStorage,
                                        m_data);
                                    MoveOrCopy(other.m_size - OtherInlineSize, other.m_data, m_data + OtherInlineSize);
                                }
                                else
                                {
                                    MoveOrCopy(other.m_size - InlineStorage, other.inline_storage() + InlineStorage,
                                        m_data);
                                }
                            }
                        }
                        else
                        {
                            MoveOrCopy(InlineStorage, other.m_data, inline_storage());
                            if (InlineStorage < other.m_size)
                                MoveOrCopy(other.m_size - InlineStorage, other.m_data + InlineStorage,
                                    m_data + InlineStorage);
                        }
                    }
                }
                else
                {
                    if constexpr (OtherInlineSize != 0)
                    {
                        MoveOrCopy(min(OtherInlineSize, other.m_size), other.inline_storage(), m_data);
                        if (other.m_size > OtherInlineSize)
                            MoveOrCopy(other.m_size - OtherInlineSize, other.m_data, m_data + OtherInlineSize);
                    }
                    else
                    {
                        MoveOrCopy(other.m_size, other.m_data, m_data);
                    }
                }
            }
            m_size = other.m_size;
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }

        /*
        constexpr Vector(Vector const& other)
        {
            ensure_capacity(max(1UL, other.m_size));
            m_size = other.m_size;
            if constexpr (InlineStorage != 0)
            {
                Copy(other.m_size, other.m_inline_storage, m_inline_storage);
                if (other.m_size > InlineStorage)
                    Copy(other.m_size - InlineStorage, other.m_data, m_data);
            }
            else
            {
                Copy(other.m_size, other.m_data, m_data);
            }
        }

        constexpr Vector(Vector&& other) :
            m_data(other.m_data), m_capacity(other.m_capacity), m_size(other.m_size)
        {
            other.m_capacity = 0;
            other.m_size = 0;
            other.m_data = nullptr;
        }
         */

        explicit constexpr Vector(Span<T> const& other) :
            m_capacity(other.size()), m_size(other.size())
        {
            clean();
            ensure_capacity(max(1UL, other.size()));
            Copy(other.size(), other.data(), m_data);
            m_size = other.size();
        }

    private:
        template<size_t ItemsLeft, typename TFirst, typename... TRest>
        constexpr void initializer_list_copy_helper(size_t index, TFirst&& first, TRest&&... rest)
        {
            if constexpr (ItemsLeft != 0)
            {
                if constexpr (InlineStorage != 0)
                {
                    if (index < InlineStorage)
                        new (inline_storage() + index) T(first);
                    else
                        new (m_data + index - InlineStorage) T(first);
                }
                else
                    new (m_data + index) T(first);
                if constexpr (ItemsLeft - 1 != 0)
                    initializer_list_copy_helper<ItemsLeft - 1, TRest...>(index + 1, forward<TRest>(rest)...);
            }
        }

    public:
        template<typename... Ts>
        explicit constexpr Vector(Ts&&... items) :
            m_capacity(sizeof...(Ts)), m_size(sizeof...(Ts))
        {
            allocate(sizeof...(items));
            initializer_list_copy_helper<sizeof...(Ts), Ts...>(0, forward<Ts>(items)...);
        }

        template<size_t OtherInlineSize = 0>
        constexpr Vector& operator=(Vector<T, OtherInlineSize> const& other)
        {
            if ((ptr_t)&other == (ptr_t)this)
                return *this;

            this->~Vector();
            new (this) Vector(other);

            return *this;
        }

        template<size_t OtherInlineSize = 0>
        constexpr Vector& operator=(Vector<T, OtherInlineSize>&& other)
        {
            if ((ptr_t)&other == (ptr_t)this)
                return *this;

            this->~Vector();
            new (this) Vector(move(other));

            return *this;
        }

        template<typename TT = T>
        requires Same<Naked<TT>, T>
        constexpr void append(TT&& e)
        {
            ensure_capacity(m_size + 1);
            if constexpr (InlineStorage != 0)
            {
                if (m_size < InlineStorage)
                    new (&inline_storage()[m_size++]) T { forward<TT>(e) };
                else
                    new (&m_data[m_size++ - InlineStorage]) T { forward<TT>(e) };
            }
            else
            {
                new (&m_data[m_size++]) T { forward<TT>(e) };
            }
        }

        template<typename TT = Span<T>>
        constexpr void append(TT&& items)
        {
            ensure_capacity(m_size + items.size());

            if constexpr (InlineStorage != 0)
            {
                if (m_size < InlineStorage)
                {
                    if constexpr (IsRvalueReference<TT>)
                        MoveOrCopy<T>(InlineStorage - m_size, items.data(), inline_storage() + m_size);
                    else
                        Copy<T>(InlineStorage - m_size, items.data(), inline_storage() + m_size);

                    if constexpr (IsRvalueReference<TT>)
                        MoveOrCopy<T>(items.size() - (InlineStorage - m_size), items.data() + InlineStorage - m_size, m_data + m_size);
                    else
                        Copy<T>(items.size() - (InlineStorage - m_size), items.data() + InlineStorage - m_size, m_data + m_size);
                }
                else
                {
                    if constexpr (IsRvalueReference<TT>)
                        MoveOrCopy<T>(items.size(), items.data(), m_data + m_size - InlineStorage);
                    else
                        Copy<T>(items.size(), items.data(), m_data + m_size - InlineStorage);
                }
            }
            else
            {
                if constexpr (IsRvalueReference<TT>)
                    MoveOrCopy<T>(items.size(), items.data(), m_data + m_size);
                else
                    Copy<T>(items.size(), items.data(), m_data + m_size);
            }
            m_size += items.size();
        }

        constexpr void resize(size_t new_size)
        {
            ensure_capacity(new_size);
            m_size = new_size;
        }

        constexpr void remove_at(size_t index)
        {
            VERIFY(index < m_size);
            if (index != m_size - 1)
            {
                if constexpr (InlineStorage != 0)
                {
                    if (index < InlineStorage)
                    {
                        MoveOrCopy(InlineStorage - index - 1, inline_storage() + index + 1, inline_storage() + index);
                        inline_storage()[InlineStorage - 1] = m_data[0];
                        MoveOrCopy(m_size - InlineStorage, m_data + 1, m_data);
                    }
                    else
                    {
                        MoveOrCopy(m_size - InlineStorage - 1, m_data + index - InlineStorage + 1, m_data + index - InlineStorage);
                    }
                }
                else
                    MoveOrCopy(m_size - index - 1, m_data + index + 1, m_data + index);
            }
            m_size--;
        }

        template<typename... Args>
        constexpr T& construct(Args... args)
        {
            ensure_capacity(m_size + 1);
            if (InlineStorage != 0)
            {
                if (m_size < InlineStorage)
                {
                    inline_storage()[m_size] = T { forward<Args>(args)... };
                }
                else
                {
                    m_data[m_size - InlineStorage] = T { forward<Args>(args)... };
                }
            }
            else
            {
                m_data[m_size] = T { forward<Args>(args)... };
            }
            if (InlineStorage != 0)
            {
                if (m_size < InlineStorage)
                    return inline_storage()[m_size++];
            }
            return m_data[m_size++];
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_size;
        }

        [[nodiscard]] constexpr size_t capacity() const
        {
            return m_capacity;
        }

        [[nodiscard]] constexpr bool is_empty() const
        {
            return m_size == 0;
        }

        constexpr void change_capacity(size_t new_capacity)
        {
            VERIFY(new_capacity > 0);

            if constexpr (InlineStorage != 0)
            {
                if (new_capacity < InlineStorage)
                    return;
            }

            T* new_buf = (T*)__builtin_calloc(new_capacity - InlineStorage, sizeof(T));

            if (m_data != nullptr)
            {
                MoveOrCopy(m_size - InlineStorage, m_data, new_buf);
                clean();
                deallocate();
            }
            m_data = new_buf;
            m_capacity = new_capacity;
        }

        constexpr void ensure_capacity(size_t needed_capacity)
        {
            VERIFY(needed_capacity > 0);
            if constexpr (InlineStorage != 0)
            {
                if (needed_capacity <= InlineStorage)
                    return;
            }
            if (m_capacity < needed_capacity)
            {
                change_capacity(needed_capacity * 2);
            }
        }

        [[nodiscard]] constexpr T& at(size_t index)
        {
            VERIFY(index < m_size);
            if constexpr (InlineStorage != 0)
            {
                if (index < InlineStorage)
                    return inline_storage()[index];
            }
            return m_data[index - InlineStorage];
        }

        [[nodiscard]] constexpr const T& at(size_t index) const
        {
            VERIFY(index < m_size);
            if constexpr (InlineStorage != 0)
            {
                if (index < InlineStorage)
                    return inline_storage()[index];
            }
            return m_data[index - InlineStorage];
        }

        [[nodiscard]] constexpr const T& first() const
        {
            return at(0);
        }

        [[nodiscard]] constexpr const T& last() const
        {
            return at(m_size - 1);
        }

        [[nodiscard]] constexpr T& first()
        {
            return at(0);
        }

        [[nodiscard]] constexpr T& last()
        {
            return at(m_size - 1);
        }

        constexpr T take_first()
        {
            T value = move(first());
            m_size--;
            if constexpr (InlineStorage == 0)
            {
                if (m_size < InlineStorage)
                {
                    MoveOrCopy(m_size, inline_storage() + 1, inline_storage());
                }
                else
                {
                    MoveOrCopy(InlineStorage - 1, inline_storage() + 1, inline_storage());
                    inline_storage()[InlineStorage - 1] = m_data[0];
                    MoveOrCopy(m_size - InlineStorage, m_data + 1, m_data);
                }
            }
            else
            {
                MoveOrCopy(m_size, m_data + 1, m_data);
            }
            return value;
        }

        constexpr T take_last()
        {
            T value = move(last());
            last().~T();
            m_size--;
            return value;
        }

        [[nodiscard]] constexpr T& operator[](size_t index)
        {
            return at(index);
        }

        [[nodiscard]] constexpr const T& operator[](size_t index) const
        {
            return at(index);
        }

        template<typename TOtherVector>
        [[nodiscard]] constexpr bool operator==(TOtherVector const& other)
        {
            if (size() != other.size())
                return false;

            if (size() == 0 && other.size() == 0)
                return true;

            if ((ptr_t)this == (ptr_t)&other)
                return true;

            auto this_begin = begin();
            auto other_begin = other.begin();
            while (*this_begin++ == *other_begin++)
            {
                if (this_begin.is_end())
                    return true;
            }

            return false;
        }

        [[nodiscard]] constexpr T* data() requires(InlineStorage == 0)
        {
            return m_data;
        }

        [[nodiscard]] constexpr const T* data() const requires(InlineStorage == 0)
        {
            return m_data;
        }

        constexpr void shrink_to_fit()
        {
            VERIFY(m_size > 0);
            change_capacity(m_size);
        }

        constexpr void clear()
        {
            clean();
            m_size = 0;
        }

        [[nodiscard]] constexpr Span<T> span() requires(InlineStorage == 0)
        {
            return { m_data, m_size };
        }

        [[nodiscard]] constexpr Span<const T> span() const requires(InlineStorage == 0)
        {
            return { m_data, m_size };
        }

        [[nodiscard]] constexpr const_iterator begin() const
        {
            return const_iterator(*this);
        }

        [[nodiscard]] constexpr iterator begin()
        {
            return iterator(*this);
        }

        [[nodiscard]] constexpr const_iterator end() const
        {
            return const_iterator(*this, m_size);
        }

        [[nodiscard]] constexpr iterator end()
        {
            return iterator(*this, m_size);
        }

    private:
        constexpr void clean()
        {
            if constexpr (!IsTriviallyDestructible<T>)
            {
                if constexpr (InlineStorage != 0)
                {
                    for (size_t i = 0; i < (m_size < InlineStorage ? m_size : InlineStorage); ++i)
                    {
                        inline_storage()[i].~T();
                    }

                    if (m_size > InlineStorage)
                    {
                        for (size_t i = 0; i < m_size - InlineStorage; ++i)
                            m_data[i].~T();
                    }
                }
                else
                {
                    for (size_t i = 0; i < m_size; ++i)
                        m_data[i].~T();
                }
            }
        }

        // size is number of T elements
        constexpr void allocate(size_t size)
        {
            if (size <= InlineStorage)
                return;
            m_data = (T*)__builtin_calloc(size - InlineStorage, sizeof(T));
        }

        constexpr void deallocate()
        {
            __builtin_free(m_data);
            m_data = nullptr;
        }

        T* inline_storage()
        {
            return reinterpret_cast<T*>(m_untyped_inline_storage);
        }

        T const* inline_storage() const
        {
            return reinterpret_cast<T const*>(m_untyped_inline_storage);
        }

        T* m_data { nullptr };
        u8 m_untyped_inline_storage[InlineStorage * sizeof(T)];

        size_t m_capacity { 0 };
        size_t m_size { 0 };
    };

}
using neo::Vector;
