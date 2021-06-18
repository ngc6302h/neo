# neo
A modern (C++20) C++ standard library.

# What is neo?
**neo** is an implementation of a C++ standard library, leveraging all the new features introduced to the language in the latest standards.

# Why neo?
After using [SerenityOS's AK](https://github.com/SerenityOS/serenity/tree/master/AK) (the standard library used for development of [SerenityOS](https://github.com/SerenityOS/serenity)
I realized how cluttered and bloated the STL was(and is). STL needs to keep backwards compatibility with old c++ standards not to break ABI as many programs depend on it. But that's not for me.
I want to use a modern library that doesn't fall behind or is against breaking change on the excuse of backwards compatibility.

# FAQ
## Is it complete?
No. I've only recently begun writing it.

## Is it stable?
Not yet. It's on a very early stage so everything is subject to change.

## Is it usable?
Mostly, if you don't care about the usual API breaks.

## How do I use neo?
Just clone the repository and include the **neo** directory in your project. Then you can just #include <Vector.h>.

## What determines what is added to neo?
Features are added on demand.

## I discovered a bug
Open an issue please.

## Why is almost everything marked constexpr?
Because I want everything to be able to execute at compile time! Yes, I know some stuff can't be run at compile time yet, like heap allocations or accesing the byte representation of objects, but I expect it to be possible in the future. Some functions that use things like memcpy or strlen have compile time switches that make it use either manual loops (for compile time) or calling those functions (for runtime). And because it's easier to mark everything constexpr now while I build it instead of going through everything at a later stage.

## Do you accept contributions?
Not yet. Maybe in the future when it's more advanced. If this interests you, however, you can instead head to SerenityOS and contribute to the project!

## Who are you?
I'm Iori Torres. You can contact me at shortanemoia@protonmail.com.
