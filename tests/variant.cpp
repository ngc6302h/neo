//
// Created by ngc on 1/11/21.
//

#include "Test.h"
#include <Variant.h>
#include <String.h>
#include <TypeExtras.h>

struct LifetimeLogger
{
    LifetimeLogger()
    {
        printf("Object constructed\n");
    }
    
    ~LifetimeLogger()
    {
        printf("Object destroyed\n");
    }
};

int main()
{
    Variant<int, String, LifetimeLogger, bool> variant {LifetimeLogger() };
    TEST_EQUAL(nameof<decltype(variant)>, "neo::Variant<int, neo::String, LifetimeLogger, bool>"_sv);
    variant = "Hello world"_s;
    TEST(variant.check_type_active<String>());
    TEST_FALSE(variant.check_type_active<int>());
}