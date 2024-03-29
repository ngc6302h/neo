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
#include "Assert.h"
#include "Vector.h"
#include "Optional.h"
#include "Array.h"

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
        Vector<TreeNode*> m_children {};
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
        requires(Same<Naked<decltype(*declval<TKeyIterator>())>, TKey>) void insert(TKeyIterator const& begin, TKeyIterator const& end, TValue const& value)
        {
            insert_internal(&m_root, begin, end, value);
        }

        template<IteratorLike TKeyIterator>
        requires(Same<Naked<decltype(*declval<TKeyIterator>())>, TKey>)
            Optional<ReferenceWrapper<TValue>> find(TKeyIterator const& begin, TKeyIterator const& end)
        const
        {
            return find_internal(&m_root, begin, end);
        }

        template<IteratorLike TKeyIterator>
        requires(Same<Naked<decltype(*declval<TKeyIterator>())>, TKey>) bool contains(TKeyIterator const& begin, TKeyIterator const& end) const
        {
            return find_internal(&m_root, begin, end) != nullptr;
        }

    private:
        template<IteratorLike TKeyIterator>
        Optional<ReferenceWrapper<TValue>> find_internal(TreeNode<detail::KeyValuePair<TKey, TValue>, 0>* node, TKeyIterator begin, TKeyIterator const& end)
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
        void insert_internal(TreeNode<detail::KeyValuePair<TKey, TValue>, 0>* node, TKeyIterator begin, TKeyIterator const& end, TValue const& value)
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

    namespace detail
    {

        template<typename TNode>
        class NodeIteratorContainer
        {
            constexpr auto& operator*()
            {
                return m_node->data;
            }

            constexpr auto const& operator*() const
            {
                return m_node->data;
            }

            constexpr NodeIteratorContainer& operator++()
            {
            }

            TNode* m_node;
        };

        template<typename TIterator, typename TData>
        struct Node
        {
            using iterator = Iterator<Node>;
            Node() = default;

            Node(Node&& other) :
                sequence(std::move(other.sequence)), data(std::move(other.data)), children(std::move(other.children))
            {
            }

            Node(Vector<typename TIterator::type>&& seq, Optional<TData>&& dat, Vector<Node>&& ch) :
                sequence(std::move(seq)), data(std::move(dat)), children(std::move(ch))
            {
            }

            Node& operator=(Node&& other)
            {
                if (this == &other)
                    return *this;

                sequence = std::move(other.sequence);
                data = std::move(other.data);
                children = std::move(other.children);

                return *this;
            }

            auto begin()
            {
                return iterator { *this };
            }

            auto end()
            {
                return iterator { *this };
            }

            Vector<typename TIterator::type> sequence;
            Optional<TData> data;
            Vector<Node> children;
        };
    }

    template<typename TIterator, typename TData>
    class RadixTree
    {
        using Node = detail::Node<TIterator, TData>;
        using sequence_type = typename TIterator::type;

    public:
        RadixTree() :
            m_root { Vector<sequence_type>::create_with_capacity(1), {}, {} }
        {
        }

        RadixTree(TIterator begin, TIterator end, Optional<TData>&& data) :
            m_root { Vector<sequence_type>::from_range(begin, end), std::move(data), {} }
        {
        }

    private:
        void reorganize(Node& node, auto const& old_range_end)
        {
            auto break_node_from_here = node.sequence.begin();
            while (break_node_from_here != old_range_end)
                ++break_node_from_here;
            ENSURE(break_node_from_here != node.sequence.end());
            Node new_node { Vector<sequence_type>::from_range(break_node_from_here, node.sequence.end()), std::move(node.data), std::move(node.children) };
            node.children = Vector<Node>();
            node.children.append(std::move(new_node));
            auto new_sequence = Vector<sequence_type>::from_range(node.sequence.begin(), break_node_from_here);
            node.sequence = std::move(new_sequence);
        }

        void insert_internal(Node& node, TIterator begin, TIterator const& end, TData const& data)
        {
            auto begin_copy = begin;
            auto seq_begin = node.sequence.begin();
            auto seq_end = node.sequence.end();
            for (; begin != end && seq_begin != seq_end && *begin == *seq_begin; begin++, seq_begin++)
            {
                if (begin == end || seq_begin == seq_end)
                    break;
            }
            if (begin != end)
            {
                if (seq_begin == seq_end)
                {
                    Node* closest_match = nullptr;
                    for (Node& n : node.children)
                    {
                        size_t length = 0;
                        auto n_begin = n.sequence.begin();
                        auto begin_ = begin;
                        while (n_begin != n.sequence.end() && begin_ != end && *begin_ == *n_begin)
                        {
                            length++;
                            begin_++;
                            n_begin++;
                        }
                        if (length > 0)
                        {
                            closest_match = &n;
                            break;
                        }
                    }
                    if (closest_match != nullptr)
                        insert_internal(*closest_match, begin, end, data);
                    else
                        node.children.template construct(Vector<sequence_type>::from_range(begin, end), data, Vector<Node> {});
                }
                else // (seq_begin != seq_end)
                {
                    reorganize(node, seq_begin);
                    insert_internal(node, begin_copy, end, data);
                }
            }
            else // (begin == end)
            {
                if (seq_begin == seq_end)
                {
                    node.data = data;
                }
                else // (seq_begin != seq_end)
                {
                    reorganize(node, seq_begin);
                    insert_internal(node, begin_copy, end, data);
                }
            }
        }

        Node const* get_internal(Node const& node, TIterator begin, TIterator const& end) const
        {
            if (begin == end)
                return &node;

            for (auto& child : node.children)
            {
                if (*child.sequence.begin() == *begin)
                {
                    auto seq_begin = child.sequence.begin();
                    while (seq_begin != child.sequence.end() && begin != end && *seq_begin == *begin)
                    {
                        seq_begin++;
                        begin++;
                    }
                    return get_internal(child, begin, end);
                }
            }
            return nullptr;
        }

        void remove_internal(Node& node, TIterator begin, TIterator const& end)
        {
        }

    public:
        Optional<const ReferenceWrapper<TData>> get(TIterator begin, TIterator const& end) const
        {
            Node const* maybe_node = get_internal(m_root, begin, end);
            if (maybe_node && maybe_node->data.has_value())
                return { ref(maybe_node->data.value()) };
            else
                return {};
        }

        Optional<const ReferenceWrapper<TData>> get(auto const& sequence) const
        {
            return get(sequence.begin(), sequence.end());
        }

        Optional<ReferenceWrapper<TData>> get(TIterator begin, TIterator const& end)
        {
            Node* maybe_node = const_cast<Node*>(get_internal(m_root, begin, end));
            if (maybe_node && maybe_node->data.has_value())
                return { ref(maybe_node->data.value()) };
            else
                return {};
        }

        Optional<ReferenceWrapper<TData>> get(auto const& sequence)
        {
            return get(sequence.begin(), sequence.end());
        }

        Node const* get_node(auto const& sequence) const
        {
            return get_internal(m_root, sequence.begin(), sequence.end());
        }

        Node* get_node(TIterator begin, TIterator const& end) const
        {
            return get_internal(m_root, begin, end);
        }

        Node* get_node(auto const& sequence)
        {
            return const_cast<Node*>(get_internal(m_root, sequence.begin(), sequence.end()));
        }

        Node* get_node(TIterator begin, TIterator const& end)
        {
            return const_cast<Node*>(get_internal(m_root, begin, end));
        }

        void insert(TIterator begin, TIterator const& end, TData const& data)
        {
            insert_internal(m_root, begin, end, data);
        }

        void insert(auto const& sequence, TData const& data)
        {
            insert_internal(m_root, sequence.begin(), sequence.end(), data);
        }

        void remove(TIterator begin, TIterator const& end)
        {
            remove_internal(m_root, begin, end);
        }

        void remove(auto const& sequence)
        {
            remove_internal(m_root, sequence.begin(), sequence.end());
        }

    private:

        template<typename SequencePrinter, typename DataPrinter>
        void debug_print_internal(Node& node, SequencePrinter sequencePrinter, DataPrinter dataPrinter, size_t depth)
        {
            __builtin_printf("%lu ", depth);
            for (size_t i = 0; i < depth; i++)
                __builtin_printf("-");
            sequencePrinter(node.sequence);
            __builtin_printf(":");
            dataPrinter(node.data);
            __builtin_printf("\n");
            for (auto& c : node.children)
                debug_print_internal(c, sequencePrinter, dataPrinter, depth + 1);
        }

    public:
        template<typename SequencePrinter, typename DataPrinter>
        void debug_print(SequencePrinter sequencePrinter, DataPrinter dataPrinter)
        {
            __builtin_printf("-----\n");
            debug_print_internal(m_root, sequencePrinter, dataPrinter, 0);
            __builtin_printf("-----\n");
        }

    private:
        Node m_root;
    };

}
using neo::RadixTree;
using neo::Trie;
