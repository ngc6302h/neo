//
// Created by ngc on 1/11/21.
//

#include "Test.h"
#include <Variant.h>
#include <String.h>
#include <TypeExtras.h>

int main()
{
    Variant<int, String, LifetimeLogger, bool> variant {LifetimeLogger() };
    __builtin_printf("%s\n", nameof<decltype(variant)>.non_null_terminated_buffer());
    TEST_EQUAL(nameof<decltype(variant)>, "neo::Variant<int, neo::String, LifetimeLogger, bool>");
    variant = "Hello world"_s;
    TEST(variant.check_type_active<String>());
    TEST_FALSE(variant.check_type_active<int>());
}