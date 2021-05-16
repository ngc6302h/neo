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

#include "AsciiString.h"
#include "Hashmap.h"
#include "Preprocessor.h"
#include "Queue.h"
#include "Stack.h"
#include "String.h"
#include "TypeTraits.h"
#include "Variant.h"
#include "Vector.h"
#include "SmartPtr.h"
#include <stdio.h>

int main()
{

    AsciiString string { "This is a test" };
    for (auto c : string)
        printf("%c_", c);
    printf("\n%s\n", (char*)string);
    
    Variant<char, long> v(0x242424L);
    String utf8string { "This is a UTF-8 string \U0001F304!! こ んにちは ہیلو Привет 你好" };
    printf("String is %zu codepoints long\n", codepoint_length_of(utf8string));
    for (Utf8Char c : utf8string) {
        printf("%c_", c);
    }
    printf("\n%s\n", (char*)utf8string);
    printf("string contains %s=%s\n", "UTF-8", utf8string.contains("UTF-8").has_value() ? "yes" : "no");

    printf("%d\n", PackContains<String, bool, String, int>);
    printf("%d\n", IndexOfType<String, bool, String, int>);
    printf("%d\n", IndexOfType<TypeOfIndex<1, bool, String, int>, bool, String, int>);
    printf("%s\n", (char*)"hello cat!"_s);
    Variant<int, bool, String> who(String("hello world"));
    printf("%s\n", (char*)who.get<String>());
    printf("%zu\n", codepoint_length_of("こんにちは"_s));
    printf("%s\n", (char*)Variant<long, String, AsciiString, Vector<bool>>::construct<String>("hello!").get<String>());

    Hashmap<String, String, StringHasher> lang_list;
    lang_list.insert("spanish", "hola");
    lang_list.insert("english", "hello");
    lang_list.insert("german", "gutten tag?");
    printf("%s\n", (char*)lang_list.get("german").value_or("not found"_s));
    printf("%s\n", (char*)lang_list.get("french").value_or("not found"_s));
    lang_list.getref("spanish").value().ref.span()[0] = 'm';
    printf("%s\n", (char*)lang_list.get("spanish").value());
    assert("hello"_s != "world"_s);
    assert("hell01"_s <=> "hell11"_s == -1);
    Stack<int> s;
    
    UniquePtr<String> p(new String("Hello"));
    [[maybe_unused]] auto ptr2 = move(p);
    auto ptr3 = UniquePtr<String>::make("hello everyone!");
    printf("%s\n", ptr3->span().data());
    RefPtr<int> ref_ptr = RefPtr<int>::make(1);
    auto copy = ref_ptr;
    auto weak = copy.make_weak();
    printf("is valid? %s. Refcount=%zu. Value=%d", weak.is_valid() ? "yes" : "no", weak.ref_count(), *weak);
    return 0;
}