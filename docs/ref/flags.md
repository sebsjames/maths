---
title: sm::flags
parent: Reference
nav_order: 10
layout: page
permalink: /ref/flags
---
# sm::flags
{: .no_toc}
## Boolean flags
{: .no_toc}

```c++
#include <sm/flags>
```
Header file: [<sm/flags>](https://github.com/sebsjames/maths/blob/main/sm/flags).

**Table of Contents**

- TOC
{:toc}

## Summary

A Boolean flags class. `sm::flags` is for Boolean state or options in your programs.
It's as compact as using a C-style approach (integer types, preprocessor definitions and bit-wise operations), but it has a semantically meaningful C++ syntax.
Although you can store state in `std::bitset`, there is no way to refer to elements of the state in terms of an enum without ugly use of static casts.

This code was adapted from an idea in the Vulkan code base via [this gist](https://gist.github.com/fschoenberger/54c5342f220af510e1f78308a8994a45) which makes it possible to have neat, meaningful and efficient code like:

```c++
bool ready = fl.test (core::engine::ready);
```

## Usage

The flags class is templated on type `E` which is required to be an enumerated class:

```c++
    template <typename E> requires std::is_enum_v<E>
    struct flags
    {
        using I = std::underlying_type_t<E>;
        // ...
```

The underlying type of E is whatever integer type was chosen for the class.

### Quick start

You generally create your own enum class to use with a flags instance.

```c++
enum class myflags : uint64_t { // up to 64 flags are possible with this underlying type
    flag_one, // Name the class and flags in some way that is right for your application
    flag_two  // It is not necessary to be explicit about the values of each flag
};
```

It is up to you to ensure that your enum class does not contain more flags than there are bits in the underlying type!
The size of the underlying type determines how much storage an instance of the flags class will use. In this case, it's 64 bits or 8 bytes.

Now you can create an object of type `sm::flags<>`:
```c++
sm::flags<myflags> fl;
```
By default, all flags here will be 0 or *false*.

Set a flag on `fl` to *true*:
```c++
fl.set (myflags::flag_one);
```

Test the flag is set:
```c++
bool flag_one_is_set = fl.test (myflags::flag_one);
```

### Set flags

You can set a single flag as above with `fl.set (myflags::flag_one)`.

If you need to clear a flag (i.e. set it to *false*) you can either call `set` with an additional argument:

```c++
fl.set (myflags::flag_one, false);
```
or you can use the `reset` method (the `flags` methods are chosen to be similar to those in `std::bitset`):
```c++
fl.reset (myflags::flag_one);
```
You can reset *all* the flags with `fl.reset()`.

You can also set several flags to **true** at once with an initializer list:

```c++
fl.set ({myflags::flag_one, myflags::flag_two});
```
This can also be called with a value argument:
```c++
fl.set ({myflags::flag_one, myflags::flag_two}, false);
```
You can flip (i.e. toggle) a flag:
```c++
fl.flip (myflags::flag_one);
```

### Test flags

The `test` method allows you to query a flag. For a single flag it's just `fl.test (myflags::flag_two)` which returns a `bool`.

You can test several flags in one call to test with an initializer list:
```c++
bool flags_one_AND_two_set = fl.test ({myflags::flag_one, myflags::flag_two});
```
You can also use the alias `all_of` which returns true if all of the flags in the list are true:
```c++
bool flags_one_AND_two_set = fl.all_of ({myflags::flag_one, myflags::flag_two});
```
To test whether *any* of a list of flags is set, use `any_of()`:
```c++
bool flags_one_OR_two_set = fl.any_of ({myflags::flag_one, myflags::flag_two});
```

### Writing a default settings function

Often, flags are a member of a class, and you want a set of default flags to be set at construction.
This can be a `constexpr` function for maximum runtime efficiency.

```c++
enum class myflags : uint8_t { one, two, three, four };
struct A // A class with flags
{
    sm::flags<myflags> fl;

    // Set default flags for this class ('two' and 'three' are true)
    constexpr void default_flags()
    {
        this->fl.reset();
        this->fl.set (myflags::two);
        this->fl.set (myflags::three);
    }

    // Constructor calls default_flags() method
    A() { default_flags(); }
}
```
### Output

You can stream your flags object to stdout:
```c++
enum class myflags : uint8_t { one, two, three, four };
std::cout << sm::flags<myflags>{myflags::one} << std::endl;
std::cout << sm::flags<myflags>{myflags::three} << std::endl;
```
This gives output:
```
00000001b
00000100b
```
The output is a binary string representation, with '1' and '0' characters followed by a 'b'.
The number of digits depends on the size of the underlying type (in this case, there are 8 bits in uint8_t).
The binary string shows the first flag as the least significant bit to the right.
For the example above, we get `00000001b` for `myflags::one` and `00000100b` for `myflags::three`
