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

        constexpr TreeNode*& left()
        {
            return m_children[0];
        }

        constexpr TreeNode*& right()
        {
            return m_children[1];
        }

        constexpr TreeNode const* const& left() const
        {
            return m_children[0];
        }

        constexpr TreeNode const* const& right() const
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

    private:
        T m_value {};
        TreeNode* m_children[Arity] {};
    };
}