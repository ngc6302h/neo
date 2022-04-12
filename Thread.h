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
#include "Util.h"
#include "SmartPtr.h"
#include "Concepts.h"
#include "String.h"
#include "ResultOrError.h"
#include "OSError.h"
#include "SmartPtr.h"
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
            virtual ~generic_callable_view() = default;
            virtual void* function_ptr() = 0;
        };

        template<typename TCallable>
        struct callable_storage : public generic_callable_view
        {
            explicit callable_storage(TCallable&& f) :
                callable(move(f)) { }
            explicit callable_storage(TCallable const& f) :
                callable(f) { }

            virtual ~callable_storage()
            {
            }

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
        [[nodiscard]] static ResultOrError<RefPtr<Thread, true>, OSError> create(TFunc&& start_function)
        {
            auto entry_point_storage = new detail::callable_storage<TFunc>(forward<TFunc>(start_function));
            if (entry_point_storage == nullptr)
                return OSError::OutOfMemory;

            RefPtr<Thread, true> thread(new Thread(entry_point_storage));
            if (!thread.is_valid())
                return OSError(OSError::OutOfMemory);

            RefPtr<Thread, true>* temp_storage = new RefPtr<Thread, true>(thread);

            asm volatile(""
                         :
                         :
                         : "memory");

            auto result = pthread_create(
                &thread->m_tid, nullptr, [](void* thread_ptr) -> void*
                {
                    auto* this_thread = reinterpret_cast<RefPtr<Thread, true>*>(thread_ptr);
                    u64 result;

                    if constexpr(!CallableWithReturnType<TFunc, void>)
                    result = (*this_thread->leak()->template get_start_function_ptr<TFunc>())();
                    else
                    {
                        (*this_thread->leak()->template get_start_function_ptr<TFunc>())();
                        result = 0;
                    }

                    this_thread->leak()->m_tid = 0;
                    delete this_thread;
                    return (void*)(ptr_t)result; },
                temp_storage);

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

        ~Thread()
        {
            if (m_tid != 0)
                pthread_detach(m_tid);
            m_tid = 0;
            delete m_entry_point_storage;
        }

        Thread& operator=(Thread const&) = delete;
        Thread& operator=(Thread&& other) = delete;

        ResultOrError<String, OSError> get_name() const
        {
            char buf[16] {};
            auto result = pthread_getname_np(m_tid, buf, sizeof(buf));
            if (result != 0)
                return OSError(result);
            return String(buf);
        }

        // returns end code
        ResultOrError<void*, OSError> wait_for_thread_exit()
        {
            VERIFY(m_tid > 0);

            void* end_code {};
            auto result = pthread_join(m_tid, &end_code);
            if (result != 0)
                return OSError(result);
            return end_code;
        }

    private:
        explicit Thread(detail::generic_callable_view* entry_point_storage) :
            m_tid(0), m_entry_point_storage(entry_point_storage)
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
