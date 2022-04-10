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
#include "SmartPtr.h"
#include "Concepts.h"
#include "String.h"
#include "ResultOrError.h"
#include "OSError.h"
#include <syscall.h>
#include <linux/sched.h>
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <linux/prctl.h>
#include <pthread.h>

namespace neo
{

    namespace detail
    {
        struct generic_callable_view
        {
            virtual void* function_ptr() = 0;
        };

        template<typename TCallable>
        struct callable_storage : public generic_callable_view
        {
            explicit callable_storage(TCallable&& f) :
                callable(move(f)) { }
            explicit callable_storage(TCallable const& f) :
                callable(f) { }

            void* function_ptr() override
            {
                return &callable;
            }

            TCallable callable;
        };
    }

    class Thread
    {
    public:
        template<VoidCallable TFunc>
        [[gnu::noinline]] [[nodiscard]] static ResultOrError<OwnPtr<Thread>, OSError> create(TFunc&& start_function)
        {
            OwnPtr<Thread> thread(new Thread());
            if (thread.is_null())
                return OSError(OSError::OutOfMemory);

            auto entry_point_storage = new detail::callable_storage<TFunc>(forward<TFunc>(start_function));
            if (entry_point_storage == nullptr)
                return OSError::OutOfMemory;

            thread->m_entry_point_storage = entry_point_storage;

            auto result = pthread_create(
                &thread->m_tid, nullptr, [](void* thread) -> void*
                {
                auto* this_thread = reinterpret_cast<Thread*>(thread);
                u64 result;

                if constexpr(!CallableWithReturnType<TFunc, void>)
                   result = (*this_thread->template get_start_function_ptr<TFunc>())();
                else
                {
                    (*this_thread->template get_start_function_ptr<TFunc>())();
                    result = 0;
                }
                this_thread->m_tid = 0;
                return (void*)(ptr_t)result; },
                thread.leak_ptr());

            if (result != 0)
                return OSError(result);

            return thread;
        }
        // name can be 15 bytes at most
        Optional<OSError> set_name(String const& name)
        {
            VERIFY(name.byte_size() < 16);
            VERIFY(m_tid != 0);
            auto result = pthread_setname_np(m_tid, name.null_terminated_characters());
            if (result != 0)
                return OSError(result);

            return {};
        }

    private:
        explicit Thread() :
            m_tid(0)
        {
        }

        template<typename TFunc>
        auto get_start_function_ptr()
        {
            return (TFunc*)m_entry_point_storage->function_ptr();
        }

        pthread_t m_tid;
        detail::generic_callable_view* m_entry_point_storage;
    };
}
using neo::Thread;
