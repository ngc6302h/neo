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

#include "Concepts.h"
#include "Tuple.h"
#include "Iterator.h"
#include "Optional.h"
#include "Util.h"

namespace neo
{
   //TODO: replace with a fast algorithm
   template<Iterable TContainer, CallableWithReturnType<bool, typename TContainer::type, typename TContainer::type> TComparerFunc>
   constexpr void sort(TContainer& what, TComparerFunc comparer)
   {
       for (auto& x : what)
       {
           for (auto& y : what)
           {
               if (comparer(x, y))
               {
                   swap(x, y);
               }
           }
       }
   }

   namespace detail
   {
       template<size_t Size, FixedContainer TTupleA, FixedContainer TTupleB, typename... TupleTypes>
       struct _zip
       {
           static constexpr decltype(auto) zip(const TTupleA& a, const TTupleB& b, TupleTypes... tuples)
           {
               return _zip<
                   Size - 1,
                   typename TTupleA::base_type, typename TTupleB::base_type, TupleTypes..., Tuple<typename TTupleA::first_type, typename TTupleB::first_type>>::zip(static_cast<typename TTupleA::base_type>(a),
                   static_cast<typename TTupleB::base_type>(b),
                   tuples...,
                   make_tuple<typename TTupleA::first_type, typename TTupleB::first_type>(a.template get<0>(), b.template get<0>()));
           }
       };

       template<typename TTupleA, typename TTupleB, typename... TupleTypes>
       struct _zip<1, TTupleA, TTupleB, TupleTypes...>
       {
           static constexpr decltype(auto) zip(const TTupleA& a, const TTupleB& b, TupleTypes... tuples)
           {
               return make_tuple<TupleTypes..., Tuple<typename TTupleA::first_type, typename TTupleB::first_type>>(
                   tuples..., make_tuple(a.template get<0>(), b.template get<0>()));
           }
       };
   }

   template<typename TTupleA, typename TTupleB>
   requires(TTupleA::size() == TTupleA::size())
       [[nodiscard]] constexpr decltype(auto) zip(const TTupleA& a, const TTupleB& b)
   {
       return detail::_zip<TTupleA::size(), TTupleA, TTupleB>::zip(a, b);
   }

   template<typename TTupleA, typename TTupleB, typename... TTuples>
   requires((TTupleA::size() == TTuples::size()) && ...)
       [[nodiscard]] constexpr decltype(auto) zip(const TTupleA& first, const TTupleB& second, const TTuples&... tuples)
   {
       return zip(zip(first, second), tuples...);
   }

   template<IteratorLike TIterator, typename T>
   constexpr bool contains(TIterator begin, TIterator end, T what, auto equality_comparer = DefaultEqualityComparer<typename TIterator::type>)
   {
       if (begin == end)
           return false;

       for (; begin != end; ++begin)
       {
           if (equality_comparer(*begin, what))
               return true;
       }

       return false;
   }

   template<IteratorLike TIterator, typename T, typename TComparer = decltype(DefaultEqualityComparer<typename TIterator::type>)>
   constexpr TIterator find(TIterator begin, TIterator end, T what, TComparer equality_comparer = DefaultEqualityComparer<typename TIterator::type>)
   {
       if (begin == end)
           return end;

       for (; begin != end; ++begin)
       {
           if (equality_comparer(*begin, what))
               return begin;
       }

       return end;
   }

   template<IteratorLike TIterator, typename TAggregate, Callable<TAggregate&, typename TIterator::type> TAggregatorFunc>
   constexpr TAggregate aggregate(TIterator begin, TIterator end, TAggregatorFunc&& aggregator, TAggregate initial_value = {})
   {
       for (; begin != end; ++begin)
           aggregator(initial_value, *begin);

       return initial_value;
   }

   template<IteratorLike TIterator, Callable<typename TIterator::type> TPredicate>
   constexpr bool all(TIterator begin, TIterator end, TPredicate&& predicate)
   {
       for (; begin != end; ++begin)
       {
           if (!predicate(*begin))
               return false;
       }

       return true;
   }

   template<IteratorLike TIterator, Callable<typename TIterator::type> TPredicate>
   constexpr bool any(TIterator begin, TIterator end, TPredicate&& predicate)
   {
       for (; begin != end; ++begin)
       {
           if (predicate(*begin))
               return true;
       }

       return false;
   }

   template<IteratorLike TIterator, CallableWithReturnType<bool, typename TIterator::type> TPredicate>
   constexpr auto skip_while(TIterator begin, TIterator end, TPredicate predicate)
   {
       while (begin != end && predicate(*begin))
           ++begin;

       return begin;
   }

   template<IteratorLike TIterator, Callable<typename TIterator::type&> TFunc>
   constexpr auto for_all(TIterator begin, TIterator end, TFunc&& func)
   {
       auto copy = begin;
       for (; begin != end; ++begin)
           func(*begin);

       return copy;
   }

   template<IteratorLike TIterator>
   constexpr auto first(TIterator begin, TIterator end)
   {
       if (begin != end)
           return Optional<typename TIterator::type> { *begin };
       else
           return Optional<typename TIterator::type>{};
   }

   template<IteratorLike TIterator, CallableWithReturnType<bool, typename TIterator::type const&> TPredicate>
   constexpr auto first(TIterator begin, TIterator end, TPredicate predicate)
   {
       while (begin != end)
       {
           if (predicate(*begin))
               return Optional<typename TIterator::type> { *begin };
           ++begin;
       }
       return Optional<typename TIterator::type>{};
   }

   template<IteratorLike TIterator>
   constexpr auto last(TIterator begin, TIterator end)
   {
       if (begin != end)
           return Optional<typename TIterator::type> { *--end };
       else
           return Optional<typename TIterator::type>{};
   }

   template<IteratorLike TIterator, CallableWithReturnType<bool, typename TIterator::type const&> TPredicate>
   constexpr auto last(TIterator begin, TIterator end, TPredicate predicate)
   {
       while (begin != end)
       {
           --end;
           if (predicate(*end))
               return Optional<typename TIterator::type> { *end };
       }
       return Optional<typename TIterator::type>{};
   }

   template<IteratorLike TIteratorSrc, IteratorLike TIteratorDst>
   constexpr void copy(TIteratorSrc source_begin, TIteratorSrc const& source_end, TIteratorDst destination_begin, TIteratorDst const& destination_end)
   {
       while(source_begin != source_end && destination_begin != destination_end)
       {
           *destination_begin++ = *source_begin++;
       }
   }

   template<IteratorLike THaystackIterator, IteratorLike TNeedleIterator>
   constexpr bool starts_with(THaystackIterator haystack_begin, THaystackIterator const& haystack_end, TNeedleIterator needle_begin, TNeedleIterator const& needle_end)
   {
       while (haystack_begin != haystack_end && needle_begin != needle_end && *haystack_begin++ == *needle_begin++)
       {
           if (needle_begin == needle_end)
               return true;
       }

       return false;
   }

   template<IteratorLike THaystackIterator, IteratorLike TNeedleIterator>
   constexpr bool ends_with(THaystackIterator const& haystack_begin, THaystackIterator haystack_end, TNeedleIterator const& needle_begin, TNeedleIterator needle_end)
   {
       --haystack_end;
       --needle_end;

       while (haystack_end != haystack_begin && needle_end != needle_begin && *--haystack_end == *--needle_end)
       {
           if (needle_end == needle_begin)
               return true;
       }
   }

   namespace detail
   {
       template<IteratorLike TFirstA, IteratorLike TFirstB, IteratorLike... TRest>
       class SequenceZipIterator : public SequenceZipIterator<TRest...>
       {
       public:
           using type = Tuple<ReferenceWrapper<typename TFirstA::type>, ReferenceWrapper<typename TRest::type>...>;
           using underlying_container_type = typename TFirstA::underlying_container_type;

           template<IteratorLike TTFirstBegin, IteratorLike TTFirstEnd, IteratorLike... TTRest>
           requires (Same<TFirstA, TTFirstBegin> && Same<TTFirstBegin, TTFirstEnd> && sizeof...(TRest) % 2 == 0)
               constexpr SequenceZipIterator(TTFirstBegin const& begin, TTFirstEnd const& end, TTRest const&... rest) :
               SequenceZipIterator<TTRest...>(rest...), m_begin(begin), m_end(end)
           {}

           constexpr SequenceZipIterator& operator++()
           {
               VERIFY(m_begin != m_end);
               ++m_begin;
               ++static_cast<SequenceZipIterator<TRest...>&>(*this);
               return *this;
           }

           constexpr SequenceZipIterator operator++(int)
           {
               auto copy = *this;
               ++*this;
               return copy;
           }

           constexpr SequenceZipIterator& operator--()
           {
               VERIFY(m_begin != m_end);
               --m_begin;
               --static_cast<SequenceZipIterator<TRest...>&>(*this);
               return *this;
           }

           constexpr SequenceZipIterator operator--(int)
           {
               auto copy = *this;
               --*this;
               return copy;
           }

           constexpr decltype(auto) operator*()
           {
               return deref();
           }

           constexpr bool operator==(SequenceZipIterator const& other) const
           {
               return m_begin == other.m_begin && static_cast<SequenceZipIterator<TRest...> const&>(*this) == static_cast<SequenceZipIterator<TRest...> const&>(other);
           }

           constexpr bool is_end() const
           {
               return m_begin == m_end || SequenceZipIterator<TRest...>::is_end();
           }

       protected:

           template<typename... Ts>
           constexpr decltype(auto) deref(Ts&... ts)
           {
               VERIFY(m_begin != m_end);
               if constexpr(sizeof...(Ts) > 0)
                   return SequenceZipIterator<TRest...>::deref(ts..., *m_begin);
               else
                   return SequenceZipIterator<TRest...>::deref(*m_begin);
           }

       private:

           TFirstA m_begin;
           TFirstA m_end;
       };

       template<IteratorLike TFirstA, IteratorLike TFirstB>
       class SequenceZipIterator<TFirstA, TFirstB>
       {
       public:
           using type = Tuple<ReferenceWrapper<typename TFirstA::type>>;
           using underlying_container_type = typename TFirstA::underlying_container_type;

           constexpr SequenceZipIterator(TFirstA const& begin, TFirstA const& end) : m_begin(begin), m_end(end)
           {}

           constexpr SequenceZipIterator& operator++()
           {
               VERIFY(m_begin != m_end);
               ++m_begin;
               return *this;
           }

           constexpr SequenceZipIterator operator++(int)
           {
               auto copy = *this;
               ++*this;
               return copy;
           }

           constexpr SequenceZipIterator& operator--()
           {
               VERIFY(m_begin != m_end);
               --m_begin;
               return *this;
           }

           constexpr SequenceZipIterator operator--(int)
           {
               auto copy = *this;
               --*this;
               return copy;
           }

           constexpr type operator*()
           {
               return deref();
           }

           constexpr bool operator==(SequenceZipIterator const& other) const
           {
               return m_begin == other.m_begin;
           }

           constexpr bool is_end() const
           {
               return m_begin == m_end;
           }

           template<typename... Ts>
           constexpr decltype(auto) deref(Ts&... ts)
           {
               VERIFY(m_begin != m_end);
               return make_tuple(ReferenceWrapper<Ts>(ts)..., ReferenceWrapper(*m_begin));
           }

       private:

           TFirstA m_begin;
           TFirstA m_end;
       };

       template<IteratorLike TIterator>
       class IteratorReverseWrapper
       {
       public:
           using type = typename TIterator::type;
           using iterator_type = TIterator;
           using underlying_container_type = typename TIterator::underlying_container_type;

           constexpr IteratorReverseWrapper(TIterator const& begin, TIterator const& end, bool at_end) : m_begin(begin), m_end(end), m_iterator(at_end ? begin : end)
           {}

           constexpr IteratorReverseWrapper& operator++()
           {
               VERIFY(m_iterator != m_begin);
               --m_iterator;
               return *this;
           }

           constexpr IteratorReverseWrapper operator++(int)
           {
               auto copy = *this;
               ++*this;
               return copy;
           }

           constexpr IteratorReverseWrapper& operator--()
           {
               VERIFY(m_iterator != m_end);
               ++m_iterator;
               return *this;
           }

           constexpr IteratorReverseWrapper operator--(int)
           {
               auto copy = *this;
               --*this;
               return copy;
           }

           constexpr decltype(auto) operator*()
           {
               return *rewind(m_iterator, 1);
           }

           constexpr bool operator==(IteratorReverseWrapper const& other) const
           {
               return m_iterator == other.m_iterator;
           }

           constexpr bool is_end() const
           {
               return m_iterator == m_begin;
           }

           constexpr IteratorReverseWrapper(IteratorReverseWrapper const&) = default;
           constexpr IteratorReverseWrapper& operator=(IteratorReverseWrapper const&) = default;

       private:
           TIterator m_begin;
           TIterator m_end;
           TIterator m_iterator;
       };
       template<IteratorLike TIterator>
       class RangeLimitedIterator
       {
       public:
           using type = typename TIterator::type;
           using iterator_type = TIterator;
           using underlying_container_type = typename TIterator::underlying_container_type;

           constexpr RangeLimitedIterator(TIterator const& iterator, TIterator const& end, size_t size, size_t index) :
               m_iterator(iterator), m_end(end), m_size(size), m_index(index)
           {
           }

           constexpr RangeLimitedIterator& operator++()
           {
               VERIFY(m_iterator != m_end);
               ++m_iterator;
               ++m_index;
               return *this;
           }

           constexpr RangeLimitedIterator operator++(int)
           {
               auto copy = *this;
               ++*this;
               return copy;
           }

           constexpr RangeLimitedIterator& operator--()
           {
               VERIFY(m_index != 0);
               --m_iterator;
               --m_index;
               return *this;
           }

           constexpr RangeLimitedIterator operator--(int)
           {
               auto copy = *this;
               --*this;
               return copy;
           }

           constexpr decltype(auto) operator*()
           {
               VERIFY(!is_end());
               return *m_iterator;
           }

           constexpr bool operator==(RangeLimitedIterator const& other) const
           {
               return m_iterator == other.m_iterator;
           }

           constexpr bool is_end() const
           {
               return m_iterator == m_end || m_index >= m_size;
           }

           constexpr size_t index() const
           {
               return m_index;
           }

           constexpr RangeLimitedIterator(RangeLimitedIterator const&) = default;
           constexpr RangeLimitedIterator& operator=(RangeLimitedIterator const& other) = default;

       private:
           constexpr decltype(auto) implementation()
           {
               return m_iterator.implementation();
           }

       private:

           TIterator m_iterator;
           TIterator m_end;
           size_t m_size;
           size_t m_index;
       };
   }

   template<IteratorLike TIterator, Callable<typename TIterator::type> TDereferenceFunc, Callable<TIterator> TIncrementFunc, Callable<TIterator> TDecrementFunc>
   class LazyIteratorWrapper
   {
   public:
       using type = ReturnType<TDereferenceFunc, typename TIterator::type>;
       using iterator_type = TIterator;
       using underlying_container_type = typename TIterator::underlying_container_type;

       constexpr LazyIteratorWrapper(TIterator const& iterator, TIterator const& end, TDereferenceFunc dereference,
           TIncrementFunc increment, TDecrementFunc decrement) :
           m_iterator(iterator), m_end(end), m_dereference(dereference), m_increment(increment), m_decrement(decrement)
       {
       }

       constexpr LazyIteratorWrapper& operator++()
       {
           m_increment(m_iterator);
           return *this;
       }

       constexpr LazyIteratorWrapper operator++(int)
       {
           auto copy = *this;
           ++*this;
           return copy;
       }

       constexpr LazyIteratorWrapper& operator--()
       {
           m_decrement(m_iterator);
           return *this;
       }

       constexpr LazyIteratorWrapper operator--(int)
       {
           auto copy = *this;
           --*this;
           return copy;
       }

       constexpr decltype(auto) operator*()
       {
           return m_dereference(*m_iterator);
       }

       constexpr bool operator==(LazyIteratorWrapper const& other) const
       {
           return m_iterator == other.m_iterator;
       }

       constexpr bool is_end() const
       {
           return m_iterator == m_end;
       }

       constexpr LazyIteratorWrapper(LazyIteratorWrapper const&) = default;
       constexpr LazyIteratorWrapper& operator=(LazyIteratorWrapper const&) = default;

   private:
       constexpr decltype(auto) implementation()
       {
           return m_iterator.implementation();
       }

       TIterator m_iterator;
       TIterator m_end;
       TDereferenceFunc m_dereference;
       TIncrementFunc m_increment;
       TDecrementFunc m_decrement;
   };

   namespace detail
   {
       template<IteratorLike TIterator, Callable<size_t, typename TIterator::type> TDereferenceWithIndexFunc>
       class IndexingIteratorWrapper
       {
       public:
           using type = typename TIterator::type;
           using iterator_type = TIterator;
           using underlying_container_type = typename TIterator::underlying_container_type;

           constexpr IndexingIteratorWrapper(TIterator const& iterator, TDereferenceWithIndexFunc dereference) :
               m_iterator(iterator), m_dereference(dereference), m_index(0)
           {
           }

           constexpr IndexingIteratorWrapper& operator++()
           {
               VERIFY(!m_iterator.is_end());
               ++m_iterator;
               ++m_index;
               return *this;
           }

           constexpr IndexingIteratorWrapper operator++(int)
           {
               auto copy = *this;
               ++*this;
               return copy;
           }

           constexpr IndexingIteratorWrapper& operator--()
           {
               VERIFY(m_index != 0);
               --m_iterator;
               --m_index;
               return *this;
           }

           constexpr IndexingIteratorWrapper operator--(int)
           {
               auto copy = *this;
               --*this;
               return copy;
           }

           constexpr decltype(auto) operator*()
           {
               VERIFY(!is_end());
               return m_dereference(index(), *m_iterator);
           }

           constexpr bool operator==(IndexingIteratorWrapper const& other) const
           {
               return m_iterator == other.m_iterator;
           }

           constexpr bool is_end() const
           {
               return m_iterator.is_end();
           }

           constexpr size_t index() const
           {
               return m_index;
           }

           constexpr IndexingIteratorWrapper(IndexingIteratorWrapper const&) = default;
           constexpr IndexingIteratorWrapper& operator=(IndexingIteratorWrapper const& other) = default;

       private:
           constexpr decltype(auto) implementation()
           {
               return m_iterator.implementation();
           }

       private:
           TIterator m_iterator;
           TDereferenceWithIndexFunc m_dereference;
           size_t m_index;
       };
   }

   template<IteratorLike TIterator>
   class IterableCollection
   {
   public:
       using type = typename TIterator::type;
       using iterator_type = TIterator;
       using underlying_container_type = typename TIterator::underlying_container_type;

       constexpr IterableCollection() = delete;

       template<IterableContainer T>
       constexpr IterableCollection(T& container) :
           m_begin(container.begin()), m_end(container.end())
       {
       }

       constexpr IterableCollection(TIterator const& begin, TIterator const& end) :
           m_begin(begin), m_end(end)
       {
       }

       constexpr TIterator begin()
       {
           return m_begin;
       }

       constexpr const TIterator begin() const
       {
           return m_begin;
       }

       constexpr TIterator end()
       {
           return m_end;
       }

       constexpr const TIterator end() const
       {
           return m_end;
       }

       constexpr size_t size() const
       {
           auto begin = m_begin;
           size_t counter = 0;

           while (begin++ != m_end)
               ++counter;

           return counter;
       }

       template<typename T, CallableWithReturnType<bool, type, T> TComparer>
       constexpr bool contains(T const& what, TComparer equality_comparer)
       {
           return neo::contains(begin(), end(), what, equality_comparer);
       }

       constexpr bool contains(type const& what)
       {
           return neo::contains(begin(), end(), what, DefaultEqualityComparer<type>);
       }

       template<IteratorLike TNeedleIterator>
       constexpr bool starts_with(TNeedleIterator const& needle_begin, TNeedleIterator const& needle_end)
       {
           return neo::starts_with(begin(), end(), needle_begin, needle_end);
       }

       template<IteratorLike TNeedleIterator>
       constexpr bool ends_with(TNeedleIterator const& needle_begin, TNeedleIterator const& needle_end)
       {
           return neo::ends_with(begin(), end(), needle_begin, needle_end);
       }

       template<typename T, CallableWithReturnType<bool, type, T> TComparer>
       constexpr auto find(T const& what, TComparer equality_comparer = DefaultEqualityComparer<type>)
       {
           return neo::find(begin(), end(), what, equality_comparer);
       }

       constexpr auto find(type const& what)
       {
           return neo::find(begin(), end(), what, DefaultEqualityComparer<type>);
       }

       template<CallableWithReturnType<bool, type const&> TPredicate>
       constexpr auto first(TPredicate predicate)
       {
           return neo::first(begin(), end(), predicate);
       }

       constexpr auto first()
       {
           return neo::first(begin(), end());
       }

       template<CallableWithReturnType<bool, type const&> TPredicate>
       constexpr auto last(TPredicate predicate)
       {
           return neo::last(begin(), end(), predicate);
       }

       constexpr auto last()
       {
           return neo::last(begin(), end());
       }


       template<Callable<type> TPredicate>
       constexpr bool all(TPredicate&& predicate)
       {
           return neo::all(begin(), end(), forward<TPredicate>(predicate));
       }

       template<Callable<type> TPredicate>
       constexpr bool any(TPredicate&& predicate)
       {
           return neo::any(begin(), end(), forward<TPredicate>(predicate));
       }

       template<typename TAggregate, Callable<TAggregate&, type> TAggregatorFunc>
       constexpr TAggregate aggregate(TAggregatorFunc&& aggregator, TAggregate initial_value = {})
       {
           return neo::aggregate(begin(), end(), forward<TAggregatorFunc>(aggregator), initial_value);
       }

       constexpr auto reverse()
       {
           return IterableCollection<detail::IteratorReverseWrapper<TIterator>>
               {
                   detail::IteratorReverseWrapper { m_begin, m_end, false },
                   detail::IteratorReverseWrapper { m_begin, m_end, true }
               };
       }

       template<CallableWithReturnType<bool, type> TPredicate>
       constexpr auto filter(TPredicate predicate)
       {
           auto increment = [predicate](TIterator& iterator) constexpr
           {
               while (!iterator.is_end())
               {
                   if ((++iterator).is_end())
                       return;
                   if (predicate(*iterator))
                       return;
               }
           };

           auto bg = m_begin;
           auto decrement = [predicate, bg](TIterator& iterator) constexpr
           {
               while (iterator != bg)
               {
                   --iterator;
                   if (predicate(*iterator))
                       return;
               }
           };

           auto begin_initialized = m_begin;
           if (!predicate(*begin_initialized))
               increment(begin_initialized);


           return IterableCollection<decltype(LazyIteratorWrapper { m_begin, m_end, default_dereference, increment, decrement })> {
               LazyIteratorWrapper { begin_initialized, m_end, default_dereference, increment, decrement },
               LazyIteratorWrapper { m_end, m_end, default_dereference, increment, decrement }
           };
       }

       template<Callable<type> TSelectorFunc>
       constexpr auto select(TSelectorFunc selector)
       {
           return IterableCollection<decltype(LazyIteratorWrapper(m_begin, m_end, selector, default_increment, default_decrement))>{
               LazyIteratorWrapper {m_begin, m_end, selector, default_increment, default_decrement },
               LazyIteratorWrapper {m_end, m_end, selector, default_increment, default_decrement} };
       }

       template<Callable<size_t, type> TSelectorFunc>
       constexpr auto select(TSelectorFunc selector)
       {
           return IterableCollection<decltype(detail::IndexingIteratorWrapper(begin(), selector))>
               {
                   detail::IndexingIteratorWrapper {begin(), selector}, detail::IndexingIteratorWrapper {end(), selector}
               };
       }

       template<Callable<type&> TFunction>
       constexpr auto for_all(TFunction&& function)
       {
           neo::for_all(begin(), end(), forward<TFunction>(function));
           return *this;
       }

       constexpr auto take(size_t n)
       {
           return IterableCollection<detail::RangeLimitedIterator<TIterator>> {
               detail::RangeLimitedIterator { begin(), m_end, n, 0 },
               detail::RangeLimitedIterator { neo::skip(m_begin, n), m_end, n, n }
           };
       }

       constexpr auto skip(size_t n)
       {
           return IterableCollection { neo::skip(m_begin, n), m_end };
       }

       template<CallableWithReturnType<bool, type> TPredicate>
       constexpr auto skip_while(TPredicate predicate)
       {
           return IterableCollection { neo::skip_while(begin(), end(), predicate), m_end };
       }

       constexpr auto skip_backwards(size_t n)
       {
           return IterableCollection { begin(), neo::rewind(m_end, n) };
       }

       template<CallableWithReturnType<bool, type> TPredicate>
       constexpr auto skip_backwards_while(TPredicate predicate)
       {
           auto end = m_end;
           while (predicate(*end))
               --end;

           return IterableCollection { m_begin, end };
       }

       template<IteratorLike TIteratorDst>
       constexpr auto copy_to(TIteratorDst destination_begin, TIteratorDst const& destination_end)
       {
           neo::copy(begin(), end(), destination_begin, destination_end);
       }

   private:
       static constexpr auto identity = [](auto& v) constexpr { return v; };
       static constexpr auto default_dereference = [](auto const& v) constexpr { return v; };
       static constexpr auto default_increment = [](auto& v) constexpr -> auto& { return ++v; };
       static constexpr auto default_decrement = [](auto& v) constexpr -> auto& { return --v; };

       TIterator m_begin;
       TIterator m_end;
   };

   template<IteratorLike... Ts>
   constexpr auto find_common_prefix_range(Ts const&... begin_end_pairs)
   {
       detail::SequenceZipIterator<Ts...> begin(begin_end_pairs...);
       detail::SequenceZipIterator<Ts...> end(begin_end_pairs...);
       size_t count = 0;
       while(!end.is_end() && tuple_are_equal(*end))
           ++count;

       return make_tuple(count, IterableCollection(begin, end));
   }

   template<IteratorLike... Ts>
   constexpr auto zip(Ts const&... begin_end_pairs)
   {
       return find_common_prefix_range(begin_end_pairs...).template get<1>();
   }

   template<typename TContainer, typename T>
   struct IterableExtensions
   {
       constexpr auto to_iterable_collection()
       {
           return IterableCollection<typename TContainer::iterator>(static_cast<TContainer&>(*this).begin(), static_cast<TContainer&>(*this).end());
       }
   };

}

using neo::aggregate;
using neo::all;
using neo::any;
using neo::contains;
using neo::find;
using neo::for_all;
using neo::skip_while;
using neo::skip;
using neo::sort;
using neo::zip;
using neo::first;
using neo::last;
using neo::find_common_prefix_range;
using neo::copy;
using neo::starts_with;
using neo::ends_with;
using neo::IterableCollection;
