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

#include "Test.h"
#include <Vector.h>
#include <GenericLexer.h>
#include <TypeExtras.h>

#define ISALPHA(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#define ISNUM(c) (c >= '0' && c <= '9')
static constexpr auto string_contains_char = [](String const& a, Utf32Char b)
{ return a.contains(b); };

int main()
{
    // sample taken from the linux kernel at https://github.com/torvalds/linux/blob/master/net/ipv4/ah4.c
    auto sample = "struct ah_skb_cb {\n"
                  "\tstruct xfrm_skb_cb xfrm;\n"
                  "\tvoid *tmp;\n"
                  "};\n"
                  "\n"
                  "#define AH_SKB_CB(__skb) ((struct ah_skb_cb *)&((__skb)->cb[0]))\n"
                  "\n"
                  "static void *ah_alloc_tmp(struct crypto_ahash *ahash, int nfrags,\n"
                  "\t\t\t  unsigned int size)\n"
                  "{\n"
                  "\tunsigned int len;\n"
                  "\n"
                  "\tlen = size + crypto_ahash_digestsize(ahash) +\n"
                  "\t      (crypto_ahash_alignmask(ahash) &\n"
                  "\t       ~(crypto_tfm_ctx_alignment() - 1));\n"
                  "\n"
                  "\tlen = ALIGN(len, crypto_tfm_ctx_alignment());\n"
                  "\n"
                  "\tlen += sizeof(struct ahash_request) + crypto_ahash_reqsize(ahash);\n"
                  "\tlen = ALIGN(len, __alignof__(struct scatterlist));\n"
                  "\n"
                  "\tlen += sizeof(struct scatterlist) * nfrags;\n"
                  "\n"
                  "\treturn kmalloc(len, GFP_ATOMIC);\n"
                  "}\n"
                  "\n"
                  "static inline u8 *ah_tmp_auth(void *tmp, unsigned int offset)\n"
                  "{\n"
                  "\treturn tmp + offset;\n"
                  "}\n"
                  "\n"
                  "static inline u8 *ah_tmp_icv(struct crypto_ahash *ahash, void *tmp,\n"
                  "\t\t\t     unsigned int offset)\n"
                  "{\n"
                  "\treturn PTR_ALIGN((u8 *)tmp + offset, crypto_ahash_alignmask(ahash) + 1);\n"
                  "}\n"
                  "\n"
                  "static inline struct ahash_request *ah_tmp_req(struct crypto_ahash *ahash,\n"
                  "\t\t\t\t\t       u8 *icv)\n"
                  "{\n"
                  "\tstruct ahash_request *req;\n"
                  "\n"
                  "\treq = (void *)PTR_ALIGN(icv + crypto_ahash_digestsize(ahash),\n"
                  "\t\t\t\tcrypto_tfm_ctx_alignment());\n"
                  "\n"
                  "\tahash_request_set_tfm(req, ahash);\n"
                  "\n"
                  "\treturn req;\n"
                  "}\n"
                  "\n"
                  "static inline struct scatterlist *ah_req_sg(struct crypto_ahash *ahash,\n"
                  "\t\t\t\t\t     struct ahash_request *req)\n"
                  "{\n"
                  "\treturn (void *)ALIGN((unsigned long)(req + 1) +\n"
                  "\t\t\t     crypto_ahash_reqsize(ahash),\n"
                  "\t\t\t     __alignof__(struct scatterlist));\n"
                  "}"_sv;

    auto separator_lex_rule = neo::GenericLexer::LexingRule {
        90,
        [](Utf32Char c)
        { static Vector<Utf32Char> separators {'(', ')', ';', ':', '{', '}', ',', '\'', '"'}; return separators.to_iterable_collection().contains(c); },
        neo::GenericLexer::LexingRuleAction::Read,
        [](Utf32Char c, StringView const&)
        { static Vector<Utf32Char> separators {'(', ')', ';', ':', '{', '}', ',', '\'', '"'}; return !separators.to_iterable_collection().contains(c) && !isspace(c) && !ISALPHA(c) && !ISNUM(c); },
        [](String const& word)
        { static Vector<String> separators {"(", ")", ";", ":", "{", "}", ",", "'", "\""}; return separators.to_iterable_collection().contains(word); },
        neo::GenericLexerTokenType::Separator
    };

    auto keyword_lex_rule = neo::GenericLexer::LexingRule {
        100,
        [](Utf32Char c)
        { return ISALPHA(c); },
        neo::GenericLexer::LexingRuleAction::Read,
        [](Utf32Char c, StringView const&)
        { return ISALPHA(c); },
        [](String const& word)
        { static Vector<String> keywords {"void", "struct", "static", "inline", "int", "unsigned", "short", "char", "long", "signed", "const", "if", "for", "do", "while", "goto", "return"}; return keywords.to_iterable_collection().contains(word); },
        neo::GenericLexerTokenType::Keyword
    };

    auto operator_lex_rule = neo::GenericLexer::LexingRule {
        200,
        [](Utf32Char c)
        { static Vector<String> operators {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "!", "%"}; return operators.to_iterable_collection().contains(c, string_contains_char); },
        neo::GenericLexer::LexingRuleAction::Read,
        [](Utf32Char c, StringView const&)
        { static Vector<String> operators {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "!", "%"}; return operators.to_iterable_collection().contains(c, string_contains_char); },
        [](String const& word)
        { static Vector<String> operators {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "!", "%"}; return operators.to_iterable_collection().contains(word); },
        neo::GenericLexerTokenType::Operator
    };

    [[maybe_unused]] auto identifier_lex_rule = GenericLexer::LexingRule {
        400,
        [](Utf32Char c)
        { return ISALPHA(c) || c == '_'; },
        neo::GenericLexer::LexingRuleAction::Read,
        [](Utf32Char c, StringView const&)
        { return ISALPHA(c) || ISNUM(c) || c == '_' || c == '$'; },
        [](String const&)
        { return true; },
        neo::GenericLexerTokenType::Identifier
    };

    [[maybe_unused]] auto numeric_lex_rule = GenericLexer::LexingRule {
        500,
        [](Utf32Char c)
        { return c >= '0' && c <= '9'; },
        neo::GenericLexer::LexingRuleAction::Read,
        [](Utf32Char c, StringView const&)
        { return c >= '0' && c <= '9'; },
        [](String const&)
        { return true; },
        neo::GenericLexerTokenType::Literal
    };

    GenericLexer lexer(Vector<GenericLexer::LexingRule> { separator_lex_rule, keyword_lex_rule, operator_lex_rule, identifier_lex_rule, numeric_lex_rule });
    auto tokens = lexer.tokenize(sample);
    for (const auto& t : tokens)
        __builtin_printf("At: L%zuP%zu Type: %s Value: %s\n", t.linepos.line, t.linepos.pos, dynamic_nameof(t.type).non_null_terminated_buffer(), t.value.null_terminated_characters());
    for (const auto& t : tokens)
        __builtin_printf("%s", t.value.null_terminated_characters());
    __builtin_printf("\n%zu\n", tokens.size());
    TEST_EQUAL(tokens.size(), 550);
    return 0;
}
