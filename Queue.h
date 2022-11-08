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

#include "Assert.h"
#include "Iterator.h"
#include "Types.h"

namespace neo
{
    template<typename T>
    struct QueueNode
    {
        T* data { nullptr };
        QueueNode* next { nullptr };
        QueueNode* back { nullptr };

        constexpr T& operator*()
        {
            return *data;
        }

        constexpr ~QueueNode()
        {
            delete data;
            delete next;
            data = nullptr;
            next = nullptr;
        }

        constexpr QueueNode operator++(int)
        {
            return { next->data, next->next, this };
        }

        constexpr QueueNode operator--(int)
        {
            return { back->data, this, back->back };
        }

        constexpr bool operator==(const QueueNode& other) const
        {
            return data == other.data;
        }

        constexpr bool operator!=(const QueueNode& other) const
        {
            return data != other.data;
        }
    };

    template<typename T>
    struct QueueNodeView
    {
        T* data;
        QueueNodeView* next;
        QueueNodeView* back;

        constexpr QueueNodeView operator++(int)
        {
            auto current = *this;
            if (next == nullptr)
            {
                data = nullptr;
            }
            else
            {
                data = next->data;
                next = next->next;
                back = this;
            }
            return current;
        }

        constexpr QueueNodeView operator--(int)
        {
            auto current = *this;
            data = back->data;
            next = this;
            back = back->back;
            return current;
        }

        constexpr bool operator!=(const QueueNodeView& other) const
        {
            return data != other.data;
        }

        constexpr const T& operator*() const
        {
            return *data;
        }

        constexpr T& operator*()
        {
            return *data;
        }

        constexpr ~QueueNodeView() {};
    };

    template<typename T>
    class Queue
    {
    public:
        constexpr Queue() = default;

        constexpr ~Queue()
        {
            delete m_first;
            m_first = nullptr;
        }

        constexpr void add_back(T const& element)
        {
            auto ptr = new QueueNode<T> { new T(element) };
            if (m_size == 0)
                m_first = m_last = ptr;
            else
            {
                m_last->next = ptr;
                ptr->back = m_last;
                m_last = ptr;
            }
            m_size++;
        }

        constexpr void add_back(T&& element)
        {
            auto ptr = new QueueNode<T> { new T(std::move(element)) };
            if (m_size == 0)
                m_first = m_last = ptr;
            else
            {
                m_last->next = ptr;
                ptr->back = m_last;
                m_last = ptr;
            }
            m_size++;
        }

        constexpr void add_front(T const& element)
        {
            auto ptr = new QueueNode { new T(element) };
            if (m_size == 0)
                m_first = m_last = ptr;
            else
            {
                ptr->next = m_first;
                m_first->back = ptr;
                m_first = ptr;
            }
            m_size++;
        }

        constexpr void add_front(T&& element)
        {
            auto ptr = new QueueNode { new T(std::move(element)) };
            if (m_size == 0)
                m_first = m_last = ptr;
            else
            {
                ptr->next = m_first;
                m_first->back = ptr;
                m_first = ptr;
            }
            m_size++;
        }

        [[nodiscard]] constexpr const T& peek_back() const
        {
            VERIFY(m_size > 0);
            return *m_last->data;
        }

        [[nodiscard]] constexpr T& peek_back()
        {
            VERIFY(m_size > 0);
            return *m_last->data;
        }

        [[nodiscard]] constexpr const T& peek_front() const
        {
            VERIFY(m_size > 0);
            return *m_first->data;
        }

        [[nodiscard]] constexpr T& peek_front()
        {
            VERIFY(m_size > 0);
            return *m_first->data;
        }

        constexpr T pop_back()
        {
            VERIFY(m_size > 0);
            T value = std::move(*m_last->data);
            auto new_back = m_last->back;
            new_back->next = nullptr;
            delete m_last;
            m_last = new_back;
            m_size--;
            return value;
        }

        constexpr T pop_front()
        {
            VERIFY(m_size > 0);
            T value = std::move(*m_first->data);
            auto next = m_first->next;
            next->back = nullptr;
            delete m_first;
            m_first = next;
            m_size--;
            return value;
        }

        [[nodiscard]] constexpr const QueueNodeView<T> first() const
        {
            return *reinterpret_cast<QueueNodeView<T>*>(m_first);
        }

        [[nodiscard]] constexpr QueueNodeView<T> first()
        {
            return *reinterpret_cast<QueueNodeView<T>*>(m_first);
        }

        [[nodiscard]] constexpr QueueNodeView<T> last()
        {
            return *reinterpret_cast<QueueNodeView<T>*>(m_last);
        }

        [[nodiscard]] constexpr const QueueNodeView<T> last() const
        {
            return *reinterpret_cast<QueueNodeView<T>*>(m_last);
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_size;
        }

    private:
        QueueNode<T>* m_first { nullptr };
        QueueNode<T>* m_last { nullptr };
        size_t m_size { 0 };
    };

}
using neo::Queue;
using neo::QueueNodeView;
