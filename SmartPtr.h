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
    class UniquePtr
    {
    public:
        UniquePtr() = delete;
        UniquePtr& operator=(const UniquePtr&) = delete;

        constexpr explicit UniquePtr(T* obj) :
            m_data(obj)
        {
            VERIFY(obj != nullptr);
        }

        constexpr UniquePtr(UniquePtr&& other) :
            m_data(other.m_data)
        {
            other.m_data = nullptr;
        }

        template<typename... Args>
        static constexpr UniquePtr make(Args&&... args)
        {
            return UniquePtr(new T(forward<Args>(args)...));
        }

        constexpr UniquePtr& operator=(UniquePtr&& other)
        {
            m_data = other.m_data;
            other.m_data = nullptr;
        }

        constexpr ~UniquePtr()
        {
            delete m_data;
        }

        [[nodiscard]] constexpr T* get() const
        {
            return m_data;
        }

        [[nodiscard]] constexpr T* release()
        {
            auto ptr = m_data;
            m_data = nullptr;
            return ptr;
        }

        [[nodiscard]] constexpr T& operator*()
        {
            return *m_data;
        }

        [[nodiscard]] constexpr T* operator->() const
        {
            return m_data;
        }

    private:
        T* m_data { nullptr };
    };

    template<typename, bool>
    class WeakPtr;

    struct ControlBlock
    {
        size_t reference_count;
        size_t weak_reference_count;
    };

    template<typename T, bool SharedBetweenThreads = true>
    class RefPtr
    {
        friend WeakPtr<T, SharedBetweenThreads>;

    public:
        RefPtr() = delete;

        constexpr explicit RefPtr(T* obj) :
            m_data(obj)
        {
            VERIFY(obj != nullptr);
            m_control = new ControlBlock { 1, 0 };
        }

        constexpr RefPtr(const RefPtr& other) :
            m_data(other.m_data), m_control(other.m_control)
        {
            VERIFY(other.m_data != nullptr);
            if constexpr (SharedBetweenThreads)
                __atomic_add_fetch(&m_control->reference_count, 1, __ATOMIC_ACQ_REL);
            else
                m_control->reference_count++;
        }

        constexpr RefPtr(RefPtr&& other) :
            m_data(other.m_data), m_control(other.m_control)
        {
            other.m_data = nullptr;
        }

        template<typename... Args>
        static constexpr RefPtr make(Args&&... args)
        {
            return RefPtr(new T(forward<Args>(args)...));
        }

        constexpr WeakPtr<T, SharedBetweenThreads> make_weak() const
        {
            return WeakPtr<T, SharedBetweenThreads>(*this);
        }

        constexpr RefPtr& operator=(const RefPtr& other)
        {
            assert(*other.m_reference_counter != 0);

            m_data = other.m_data;
            m_control = other.m_control;
            if constexpr (SharedBetweenThreads)
                __atomic_add_fetch(&m_control->reference_count, 1, __ATOMIC_ACQ_REL);
            else
                m_control->reference_count++;
        }

        constexpr RefPtr& operator=(RefPtr&& other)
        {
            assert(*other.m_reference_counter != 0);

            m_data = other.m_data;
            m_control = other.m_control;
            other.m_data = nullptr;
        }

        constexpr ~RefPtr()
        {
            if constexpr (SharedBetweenThreads)
                __atomic_sub_fetch(&m_control->reference_count, 1, __ATOMIC_ACQ_REL);
            else
                m_control->reference_count--;

            if (m_control->reference_count == 0)
            {
                delete m_data;
                m_data = nullptr;
                if (m_control->weak_reference_count == 0)
                {
                    delete m_control;
                }
            }
        }

        [[nodiscard]] constexpr T* get() const
        {
            return m_data;
        }

        [[nodiscard]] constexpr T& operator*()
        {
            return *m_data;
        }

        [[nodiscard]] constexpr T* operator->() const
        {
            return m_data;
        }

        [[nodiscard]] constexpr size_t ref_count() const
        {
            return m_control->reference_count;
        }

        [[nodiscard]] constexpr bool is_valid() const
        {
            return m_data != nullptr;
        }

    private:
        T* m_data;
        ControlBlock* m_control;
    };

    template<typename T, bool SharedBetweenThreads = true>
    class WeakPtr
    {
    public:
        WeakPtr() = delete;

        constexpr WeakPtr(const RefPtr<T>& other) :
            m_data(other.m_data), m_control(other.m_control)
        {
            VERIFY(other.is_valid());
            if constexpr (SharedBetweenThreads)
                __atomic_add_fetch(&m_control->weak_reference_count, 1, __ATOMIC_ACQ_REL);
            else
                m_control->weak_reference_count++;
        }

        constexpr WeakPtr(WeakPtr&& other) :
            m_data(other.m_data), m_control(other.m_control)
        {
            other.m_data = nullptr;
        }

        constexpr WeakPtr& operator=(const WeakPtr& other)
        {
            VERIFY(other.is_valid());

            m_data = other.m_data;
            m_control = other.m_control;
            if constexpr (SharedBetweenThreads)
                __atomic_add_fetch(&m_control->weak_reference_count, 1, __ATOMIC_ACQ_REL);
            else
                m_control->weak_reference_count++;
        }

        constexpr WeakPtr& operator=(WeakPtr&& other)
        {
            VERIFY(other.is_valid());

            m_data = other.m_data;
            m_control = other.m_control;
            other.m_data = nullptr;
        }

        constexpr ~WeakPtr()
        {
            if constexpr (SharedBetweenThreads)
                __atomic_sub_fetch(&m_control->weak_reference_count, 1, __ATOMIC_ACQ_REL);
            else
                m_control->weak_reference_count--;
            if (m_control->reference_count == 0 && m_control->weak_reference_count == 0)
            {
                delete m_data;
                delete m_control;
            }
        }

        [[nodiscard]] constexpr T* get() const
        {
            VERIFY(is_valid());
            return m_data;
        }

        [[nodiscard]] constexpr T& operator*()
        {
            VERIFY(is_valid());
            return *m_data;
        }

        [[nodiscard]] constexpr T* operator->() const
        {
            VERIFY(is_valid());
            return m_data;
        }

        [[nodiscard]] constexpr size_t ref_count() const
        {
            return m_control->reference_count;
        }

        [[nodiscard]] constexpr bool is_valid() const
        {
            return m_data != nullptr;
        }

    private:
        T* m_data;
        ControlBlock* m_control;
    };
}
using neo::RefPtr;
using neo::UniquePtr;
using neo::WeakPtr;
