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
#include "Concepts.h"
#include "Assert.h"
#include "Vector.h"

namespace neo
{
    template<typename T, size_t Arity = 2>
    class TreeNode
    {
    public:
        explicit TreeNode(T const& value) :
            m_value(value)
        {
        }

        explicit TreeNode(T&& value) :
            m_value(move(value))
        {
        }

        constexpr TreeNode*& left() requires(Arity == 2)
        {
            return m_children[0];
        }

        constexpr TreeNode*& right() requires(Arity == 2)
        {
            return m_children[1];
        }

        constexpr TreeNode const* const& left() const requires(Arity == 2)
        {
            return m_children[0];
        }

        constexpr TreeNode const* const& right() const requires(Arity == 2)
        {
            return m_children[1];
        }

        constexpr T& value()
        {
            return m_value;
        }

        constexpr T const& value() const
        {
            return m_value;
        }

        constexpr TreeNode*& get(size_t children_index)
        {
            VERIFY(children_index < Arity);
            return m_children[children_index];
        }

        constexpr TreeNode const* const& get(size_t children_index) const
        {
            VERIFY(children_index < Arity);
            return m_children[children_index];
        }

        constexpr Vector<TreeNode*>& children()
        {
            return m_children;
        }

        constexpr Vector<TreeNode*> const& children() const
        {
            return m_children;
        }

    private:
        T m_value {};
        Vector<TreeNode*, Arity> m_children {};
    };

    namespace detail
    {
        template<typename TKey, typename TValue>
        class KeyValuePair
        {
        public:
            KeyValuePair(TKey const& key, TValue const& value) :
                m_key(key), m_value(value) { }
            TKey& key() { return m_key; }
            TKey const& key() const { return m_key; }
            TValue& value() { return m_value; }
            TValue const& value() const { return m_value; }

        private:
            TKey m_key;
            TValue m_value;
        };
    }

    template<typename TKey, typename TValue>
    class Trie
    {
    public:
        template<IteratorLike TKeyIterator>
        requires(Same<decltype(*declval<TKeyIterator>()), TKey>) void insert(TKeyIterator begin, TKeyIterator end, TValue const& value)
        {
            insert_internal(&m_root, begin, end, value);
        }

        template<IteratorLike TKeyIterator>
        requires(Same<decltype(*declval<TKeyIterator>()), TKey>)
            TValue* find(TKeyIterator begin, TKeyIterator end)
        {
            return find_internal(&m_root, begin, end);
        }

        template<IteratorLike TKeyIterator>
        requires(Same<decltype(*declval<TKeyIterator>()), TKey>) bool contains(TKeyIterator begin, TKeyIterator end)
        {
            return find_internal(&m_root, begin, end) != nullptr;
        }

    private:
        template<IteratorLike TKeyIterator>
        TValue* find_internal(TreeNode<detail::KeyValuePair<TKey, TValue>, 0>* node, TKeyIterator begin, TKeyIterator end)
        {
            if (begin == end)
            {
                return node->value().value();
            }
            for (TreeNode<detail::KeyValuePair<TKey, TValue>, 0>* child : node->children())
            {
                if (child->value().key() == *begin)
                {
                    return contains_internal(child, ++begin, end);
                }
            }
            return nullptr;
        }

        template<IteratorLike TKeyIterator>
        void insert_internal(TreeNode<detail::KeyValuePair<TKey, TValue>, 0>* node, TKeyIterator begin, TKeyIterator end, TValue const& value)
        {
            if (begin == end)
            {
                return;
            }
            else
            {
                for (TreeNode<detail::KeyValuePair<TKey, TValue>, 0>* child : node->children())
                {
                    if (child->value().key() == *begin)
                    {
                        insert_internal(child, ++begin, end, value);
                        return;
                    }
                }

                if (skip(begin, 1) == end)
                    node->children().template append(new TreeNode<detail::KeyValuePair<TKey, TValue>, 0>({ *begin++, value }));
                else
                {
                    auto new_node = new TreeNode<detail::KeyValuePair<TKey, TValue>, 0>({ *begin, nullptr });
                    node->children().template append(new_node);
                    insert_internal(new_node, ++begin, end, value);
                }
            }
        }

        TreeNode<detail::KeyValuePair<TKey, TValue>, 0> m_root;
    };

}
