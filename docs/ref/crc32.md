---
layout: page
title: sm::crc32
parent: Reference
nav_order: 30
permalink: /ref/crc32/
---
# sm::crc32
{: .no_toc}
## A constexpr CRC-32 checksum
{: .no_toc}
```c++
import sm.crc32;
```

Module file: [sm/crc32.cppm](https://github.com/sebsjames/maths/blob/main/sm/crc32.cppm)

**Table of Contents**

- TOC
{:toc}

## Summary

a `constexpr`-capable port of [Ivor Wanders' CRC-32 gist](https://gist.github.com/iwanders/8e1cb7b92af2ccf8d1a73450d771f483) (MIT licensed), itself based on the 'crc-32' polynomial from Python's `crcmod` module.

`sm::crc32` is a single function computing the standard CRC-32 checksum of a string:
```c++
std::uint32_t crc = sm::crc32 (a_string); // a_string is a std::string, or anything that converts to std::string_view
```
Because it takes a `std::string_view` and is `constexpr`, it can be evaluated at compile time - most usefully, as a `case` label in a `switch` statement, letting you dispatch on string content without a chain of `if`/`else` string comparisons:
```c++
using std::literals::string_view_literals::operator""sv;

switch (sm::crc32 (some_runtime_string)) {
case sm::crc32 ("abc"sv):
    std::cout << "Matched abc" << std::endl;
    break;
default:
    std::cout << "No match" << std::endl;
}
```

*This page was authored with AI, based on human written code in crc32.cppm. Reviewed by Seb James*
