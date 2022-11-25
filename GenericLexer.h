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

#include "TypeExtras.h"
#include "Vector.h"
#include "String.h"
#include "IterableUtil.h"
#include "ResultOrError.h"

namespace neo
{
    STRINGIFIABLE_ENUM(GenericLexerTokenType, Identifier, Keyword, Separator, Operator, Literal, Comment, Whitespace, Unknown);

    struct LinePos
    {
        size_t line;
        size_t pos;
    };

    struct GenericLexerToken
    {
        GenericLexerTokenType type;
        LinePos linepos;
        String value;
    };

    class GenericLexer
    {
        struct MultilineStringIterator
        {
            MultilineStringIterator(StringViewIterator const& iterator) :
                it(iterator), linepos()
            {
            }

            StringViewIterator it;
            LinePos linepos;

            MultilineStringIterator& operator++()
            {
                ++it;
                if (!it.is_end() && *it == '\n')
                {
                    linepos.line++;
                    linepos.pos = 0;
                }
                else
                {
                    linepos.pos++;
                }

                return *this;
            }

            MultilineStringIterator operator++(int)
            {
                auto v = *this;
                ++(*this);
                return v;
            }

            Utf32Char operator*() const
            {
                return *it;
            }

            bool operator==(MultilineStringIterator const& other) const
            {
                return it == other.it;
            }
        };

    public:
        enum class LexingRuleAction
        {
            Read,
            Skip
        };

        struct LexingRule
        {
            i64 priority;
            bool (*when)(Utf32Char);
            LexingRuleAction do_;
            bool (*while_)(Utf32Char, StringView const&);
            bool (*save_if)(String const&);
            GenericLexerTokenType save_as;
        };

        GenericLexer(Vector<LexingRule> const& lexing_rules) :
            m_lexing_rules(lexing_rules)
        {
            VERIFY(lexing_rules.size() > 0);
            sort(m_lexing_rules, [](GenericLexer::LexingRule const& a, GenericLexer::LexingRule const& b)
                { return a.priority < b.priority; });
        }

        [[nodiscard]] Vector<GenericLexerToken> tokenize(String const& source)
        {
            Vector<GenericLexerToken> tokens;
            MultilineStringIterator current = source.to_view().begin(), end = source.to_view().end(), tmp = source.to_view().begin();
            String chunk;

            while (current != end)
            {
                for (const auto& rule : m_lexing_rules)
                {
                    if (rule.when(*current))
                    {
                        tmp = current;
                        do
                        {
                            ++tmp;
                        } while (tmp != end && rule.while_(*tmp, StringView::substring(current.it, tmp.it)));
                        chunk = neo::String::substring(current.it, tmp.it);
                        if (rule.do_ == LexingRuleAction::Read && rule.save_if(chunk))
                        {
                            tokens.construct(rule.save_as, current.linepos, chunk);
                        }
                        else
                            continue;

                        current = tmp;
                        goto main_loop;
                    }
                }

                tokens.append(GenericLexerToken { GenericLexerTokenType::Unknown, current.linepos, String(current++.it.ptr(), 1) });
            main_loop:
                continue;
            }
            return tokens;
        }

    private:
        Vector<LexingRule> m_lexing_rules;
    };
}
using neo::GenericLexer;
using neo::GenericLexerTokenType;
