---
layout: page
title: sm::base64 (namespace)
parent: Reference
nav_order: 16
permalink: /ref/base64/
---
# sm::base64
{: .no_toc}
## Base64 encoding and decoding
{: .no_toc}
```c++
import sm.base64;
```

Module file: [sm/base64.cppm](https://github.com/sebsjames/maths/blob/main/sm/base64.cppm). This module is adapted, with thanks, from [a base64 snippet by mvorbrodt](https://github.com/mvorbrodt/blog/blob/master/src/base64.hpp).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::base64::encode` and `sm::base64::decode` convert between raw bytes (`std::vector<std::uint8_t>`) and base64-encoded text (`std::string`), using the standard alphabet (`A`&ndash;`Z`, `a`&ndash;`z`, `0`&ndash;`9`, `+`, `/`) and `=` padding.

```c++
std::vector<std::uint8_t> data = { 'H', 'e', 'l', 'l', 'o' };
std::string encoded = sm::base64::encode (data);
// encoded == "SGVsbG8="

std::vector<std::uint8_t> decoded = sm::base64::decode (encoded);
// decoded == data
```

Binary (non-text) data round-trips just as well:
```c++
std::vector<std::uint8_t> bytes = { 0xDE, 0xAD, 0xBE, 0xEF };
std::string encoded = sm::base64::encode (bytes); // "3q2+7w=="
```

## Decoding errors

`decode` throws `std::runtime_error` in three cases:
* the input's length isn't a multiple of 4 (`"Invalid base64 length!"`);
* the input contains a character outside the base64 alphabet and padding character (`"Invalid character in base64!"`);
* a `=` padding character appears somewhere other than in a valid position at the end of the input (`"Invalid padding in base64!"`).

```c++
try {
    sm::base64::decode ("abc"); // length 3, not a multiple of 4
} catch (const std::exception& e) {
    std::cout << "caught: " << e.what() << std::endl; // "caught: Invalid base64 length!"
}
```

*This page was authored with AI, based on human written code in base64.cppm. Checked by Seb James*
