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
            MultilineStringIterator(StringIterator iterator) : it(iterator), linepos()
            {}
            
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
            
            Utf8Char operator*() const
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
            bool (*when)(Utf8Char);
            LexingRuleAction do_;
            bool (*while_)(Utf8Char);
            bool (*save_if)(String const&);
            GenericLexerTokenType save_as;
        };
        
        
        GenericLexer(Vector<LexingRule> const& lexing_rules, bool(*is_whitespace)(Utf8Char) = [](Utf8Char c) -> bool { return isspace(c); }) :
        m_lexing_rules(lexing_rules),
        is_whitespace(is_whitespace)
        {
            VERIFY(lexing_rules.size()>0);
        }
        
        [[nodiscard]] Vector<GenericLexerToken> tokenize(String const& source)
        {
#define DO_CUSTOM_LEXING_RULE(do_when) \
            rules = m_lexing_rules     \
                .filter([](LexingRule const& rule) { return rule.priority do_when; })                                   \
                .sort([](GenericLexer::LexingRule const& a, GenericLexer::LexingRule const& b) { return a.priority < b.priority; });               \
            for(const auto& rule : rules)                                                                               \
            {                                                                                                           \
                if (rule->when(*current))                                                                               \
                {                                                                                                       \
                    tmp = current;                                                                                      \
                    do     \
                    {                  \
                    ++tmp; \
                    }                  \
                    while(tmp != end && rule->while_(*tmp)); \
                    chunk = neo::String::substring(current.it, tmp.it);                                                 \
                    if (rule->do_ == LexingRuleAction::Read && rule->save_if(chunk))                                    \
                    {                                                                                                   \
                        tokens.construct(rule->save_as, current.linepos, chunk);                                        \
                    }                                                                                                   \
                    else                                                                                                \
                    {                                                                                                   \
                        continue;                                                                                       \
                    }                                                                                                   \
                    current = tmp;                                                                                      \
                    goto main_loop;                                                                                     \
                }                                                                                                       \
            }                                                                                                           \
            
            Vector<GenericLexerToken> tokens;
            MultilineStringIterator current = source.begin(), end = source.end(), tmp = source.begin();
            String chunk;
            
            Vector<ReferenceWrapper<LexingRule>> rules;
            while(current != end)
            {
                DO_CUSTOM_LEXING_RULE(< 0);
                if (is_whitespace(*current))
                {
                    tokens.append({GenericLexerTokenType::Whitespace, current.linepos, String(current.it.ptr(), 1)});
                    ++current;
                }
                else
                {
                    DO_CUSTOM_LEXING_RULE(> 0);
                    
                    tokens.append({GenericLexerTokenType::Unknown, current.linepos, String(current++.it.ptr(), 1)});
                }
                main_loop:
                continue;
            }
            return tokens;
#undef DO_CUSTOM_LEXING_RULE
        }
    private:
        Vector<LexingRule> m_lexing_rules;
        
         bool (*is_whitespace)(const Utf8Char);
    };
}
using neo::GenericLexer;
