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
#include "Atomic.h"
#include "Types.h"
#include "TypeTraits.h"
#include "New.h"

namespace neo
{
    template<typename T, bool Nullable>
    class OwnPtrImpl
    {
    public:
        OwnPtrImpl& operator=(OwnPtrImpl const&) = delete;
        OwnPtrImpl(OwnPtrImpl const&) = delete;

        constexpr explicit OwnPtrImpl(T* obj) :
            m_data(obj)
        {
            if constexpr (!Nullable)
                ENSURE(obj != nullptr);
        }

        constexpr OwnPtrImpl(OwnPtrImpl&& other) :
            m_data(other.m_data)
        {
            other.m_data = nullptr;
        }

        constexpr OwnPtrImpl& operator=(OwnPtrImpl&& other)
        {
            if (this == &other)
                return *this;

            this->~OwnPtrImpl();
            new (this) OwnPtrImpl(std::move(other));

            return *this;
        }

        constexpr ~OwnPtrImpl()
        {
            delete m_data;
            m_data = nullptr;
        }

        static constexpr OwnPtrImpl adopt(T* ptr)
        {
            if constexpr (!Nullable)
                ENSURE(ptr != nullptr);

            return { ptr };
        }

        [[nodiscard]] constexpr T* release()
        {
            auto ptr = m_data;
            m_data = nullptr;
            return ptr;
        }

        [[nodiscard]] constexpr T const* leak_ptr() const requires(Nullable)
        {
            return m_data;
        }

        [[nodiscard]] constexpr T* leak_ptr() requires(Nullable)
        {
            return m_data;
        }

        [[nodiscard]] constexpr T const& leak_ref() const
        {
            ENSURE(m_data != nullptr);
            return *m_data;
        }

        [[nodiscard]] constexpr T& leak_ref()
        {
            ENSURE(m_data != nullptr);
            return *m_data;
        }

        [[nodiscard]] constexpr T& operator*()
        {
            ENSURE(m_data != nullptr);
            return *m_data;
        }

        [[nodiscard]] constexpr T const& operator*() const
        {
            ENSURE(m_data != nullptr);
            return *m_data;
        }

        [[nodiscard]] constexpr T* operator->()
        {
            ENSURE(m_data != nullptr);
            return m_data;
        }

        [[nodiscard]] constexpr T const* operator->() const
        {
            ENSURE(m_data != nullptr);
            return m_data;
        }

        [[nodiscard]] constexpr OwnPtrImpl<T, false> release_nonnull() requires(Nullable)
        {
            ENSURE(m_data != nullptr);
            auto* data = m_data;
            m_data = nullptr;
            return OwnPtrImpl<T, false>(data);
        }

    private : OwnPtrImpl() = default;

        T* m_data { nullptr };
    };

    template<typename T>
    using OwnPtr = OwnPtrImpl<T, false>;
    template<typename T>
    using NullableOwnPtr = OwnPtrImpl<T, true>;

    template<typename>
    class WeakPtr;

    namespace detail
    {
        struct RefPtrControlBlock
        {
            Atomic<size_t> reference_count;
            Atomic<size_t> weak_reference_count;
        };
    }

    template<typename T, bool Nullable>
    class RefPtrImpl
    {
        friend WeakPtr<T>;

        template<typename T_, bool B>
        friend class RefPtrImpl;

    public:
        using type = T;
        static constexpr bool nullable = Nullable;

        constexpr explicit RefPtrImpl(T* obj) :
            m_data(obj)
        {
            if constexpr (!Nullable)
                ENSURE(obj != nullptr);
            m_control = new detail::RefPtrControlBlock { 1, 0 };
        }

        constexpr RefPtrImpl(const RefPtrImpl& other) :
            m_data(other.m_data), m_control(other.m_control)
        {
            VERIFY(other.m_data != nullptr);
            ENSURE(m_control != nullptr);
            m_control->reference_count.add_fetch(1, MemoryOrder::AcquireRelease);
        }

        constexpr RefPtrImpl(RefPtrImpl&& other) :
            m_data(other.m_data), m_control(other.m_control)
        {
            other.m_data = nullptr;
            other.m_control = nullptr;
        }

        static constexpr RefPtrImpl adopt(T* ptr)
        {
            if constexpr (!Nullable)
                ENSURE(ptr != nullptr);

            return { ptr };
        }

        template<typename TBase>
        requires BaseOf<TBase, T>
        constexpr operator RefPtrImpl<TBase, Nullable>()
        {
            m_control->reference_count.add_fetch(1, MemoryOrder::AcquireRelease);

            RefPtrImpl<TBase, Nullable> base;
            base.m_control = m_control;
            base.m_data = m_data;
            return base;
        }

        constexpr operator RefPtrImpl<T, true>() requires(!Nullable)
        {
            return release_nonnull();
        }

        constexpr RefPtrImpl<T, false> release_nonnull()
        {
            ENSURE(m_data != nullptr);

            m_control->reference_count.add_fetch(1, MemoryOrder::AcquireRelease);

            RefPtrImpl<T, false> copy;
            copy.m_data = m_data;
            copy.m_control = m_control;

            return copy;
        }

        template<typename TDerived>
        requires BaseOf<T, TDerived>
        constexpr RefPtrImpl(RefPtrImpl<TDerived, Nullable> const& other) :
            m_data(other.m_data), m_control(other.m_control)
        {
            VERIFY(other.m_data != nullptr);
            ENSURE(m_control != nullptr);
            m_control->reference_count.add_fetch(1, MemoryOrder::AcquireRelease);
        }

        constexpr WeakPtr<T> make_weak() const
        {
            return WeakPtr<T>(*this);
        }

        constexpr RefPtrImpl& operator=(RefPtrImpl const& other)
        {
            if (this == &other)
                return *this;

            VERIFY(other.m_control != nullptr);

            this->~RefPtrImpl();
            new (this) RefPtrImpl(other);

            return *this;
        }

        constexpr RefPtrImpl& operator=(const T*& other)
        {
            if (this == &other)
                return *this;

            this->~RefPtrImpl();
            new (this) RefPtrImpl(other);

            return *this;
        }

        constexpr RefPtrImpl& operator=(RefPtrImpl&& other)
        {
            VERIFY(other.m_control != nullptr);

            unref();
            new (this) RefPtrImpl(std::move(other));

            return *this;
        }

        constexpr void unref()
        {
            if (m_control != nullptr)
            {
                size_t refs = m_control->reference_count.sub_fetch(1, AcquireRelease);

                if (refs == 0)
                {
                    delete m_data;
                    m_data = nullptr;
                    if (m_control->weak_reference_count == 0)
                    {
                        delete m_control;
                        m_control = nullptr;
                    }
                }

                m_data = nullptr;
                m_control = nullptr;
            }
        }

        constexpr ~RefPtrImpl()
        {
            unref();
        }

        [[nodiscard]] constexpr T const* leak_ptr() const requires(Nullable)
        {
            return m_data;
        }

        [[nodiscard]] constexpr T* leak_ptr() requires(Nullable)
        {
            return m_data;
        }

        [[nodiscard]] constexpr T const& leak_ref() const
        {
            if constexpr (!Nullable)
            {
                ENSURE(m_data != nullptr);
            }
            else
            {
                VERIFY(m_data != nullptr);
            }
            return *m_data;
        }

        [[nodiscard]] constexpr T& leak_ref()
        {
            if constexpr (!Nullable)
            {
                ENSURE(m_data != nullptr);
            }
            else
            {
                VERIFY(m_data != nullptr);
            }
            return *m_data;
        }

        [[nodiscard]] constexpr T const& operator*() const
        {
            return leak_ref();
        }

        [[nodiscard]] constexpr T& operator*()
        {
            return leak_ref();
        }

        [[nodiscard]] constexpr T const* operator->() const
        {
            if constexpr (!Nullable)
            {
                ENSURE(m_data != nullptr);
            }
            else
            {
                VERIFY(m_data != nullptr);
            }
            return m_data;
        }

        [[nodiscard]] constexpr T* operator->()
        {
            if constexpr (!Nullable)
            {
                ENSURE(m_data != nullptr);
            }
            else
            {
                VERIFY(m_data != nullptr);
            }
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
        RefPtrImpl() = default;

        T* m_data { nullptr };
        detail::RefPtrControlBlock* m_control { nullptr };
    };

    template<typename T>
    using RefPtr = RefPtrImpl<T, false>;
    template<typename T>
    using NullableRefPtr = RefPtrImpl<T, true>;

    template<typename T>
    class WeakPtr
    {
    public:
        constexpr WeakPtr(const RefPtr<T>& other) :
            m_data(other.m_data), m_control(other.m_control)
        {
            VERIFY(other.is_valid());
            m_control->weak_reference_count.add_fetch(1, AcquireRelease);
        }

        constexpr WeakPtr(WeakPtr const& other) :
            m_data(other.m_data), m_control(other.m_control)
        {
            m_control->weak_reference_count.add_fetch(1, AcquireRelease);
        }

        constexpr WeakPtr(WeakPtr&& other) :
            m_data(other.m_data), m_control(other.m_control)
        {
            other.m_data = nullptr;
            other.m_control = nullptr;
        }

        constexpr WeakPtr& operator=(const WeakPtr& other)
        {
            VERIFY(other.is_valid());

            this->~WeakPtr();
            new (this) WeakPtr(other);

            return *this;
        }

        constexpr WeakPtr& operator=(WeakPtr&& other)
        {
            VERIFY(other.is_valid());

            this->~WeakPtr();
            new (this) WeakPtr(std::move(other));

            return *this;
        }

        constexpr ~WeakPtr()
        {
            if (m_control == nullptr)
                return;

            size_t refs = m_control->weak_reference_count.sub_fetch(1, AcquireRelease);
            if (m_control->reference_count.load(Acquire) == 0 && refs == 0)
            {
                delete m_data;
                m_data = nullptr;
                delete m_control;
                m_control = nullptr;
            }
        }

        [[nodiscard]] constexpr T* leak()
        {
            return m_data;
        }

        [[nodiscard]] constexpr const T* leak() const
        {
            return m_data;
        }

        [[nodiscard]] constexpr T& operator*()
        {
            VERIFY(is_valid());
            return *m_data;
        }

        [[nodiscard]] constexpr const T& operator*() const
        {
            VERIFY(is_valid());
            return *m_data;
        }

        [[nodiscard]] constexpr T* operator->()
        {
            VERIFY(is_valid());
            return m_data;
        }

        [[nodiscard]] constexpr const T* operator->() const
        {
            VERIFY(is_valid());
            return m_data;
        }

        [[nodiscard]] constexpr size_t ref_count() const
        {
            return m_control->reference_count;
        }

        [[nodiscard]] constexpr size_t weak_ref_count() const
        {
            return m_control->reference_count;
        }

        [[nodiscard]] constexpr bool is_valid() const
        {
            return m_data != nullptr;
        }

    private:
        WeakPtr() = default;

        T* m_data;
        detail::RefPtrControlBlock* m_control;
    };
}
using neo::NullableOwnPtr;
using neo::NullableRefPtr;
using neo::OwnPtr;
using neo::RefPtr;
using neo::WeakPtr;
