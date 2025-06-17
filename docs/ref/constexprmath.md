---
layout: page
title: sm::cem::functions
parent: Reference
permalink: /ref/constexprmath/
---
# sm::cem::functions
{: .no_toc}

```c++
#include <sm/constexpr_math>
```

Header file: [<sm/constexpr_math>](https://github.com/sebsjames/maths/blob/main/sm/constexpr_math).

**Table of Contents**

- TOC
{:toc}

## Summary

`constexpr` capable basic maths functions that will work in your C++20
and C++23 programs. These functions should all be superceded by C++26
functions in the `std` namespace.

This file is about 700 lines of cherry-picked code from Keith O'Hara's
GCE Math library (thanks Keith!).

## Usage

Use the functions just like their `std::` equivalents, but optionally in your `constexpr` functions.

The functions provided are:

* `sm::cem::abs` maps to `std::abs`
* `sm::cem::isinf` maps to `std::isinf`
* `sm::cem::isfinite` maps to `std::isfinite`
* `sm::cem::sqrt` maps to `std::sqrt`
* `sm::cem::pow` maps to `std::pow`
* `sm::cem::exp` maps to `std::exp`
* `sm::cem::log` maps to `std::log`
* `sm::cem::log10` maps to `std::log10`
* `sm::cem::signbit` maps to `std::signbit`
* `sm::cem::sgn` is the signum function (bonus)
* `sm::cem::ceil` maps to `std::ceil` (already constexpr in C++23)
* `sm::cem::floor` maps to `std::floor` (already constexpr in C++23)
* `sm::cem::trunc` maps to `std::trunc` (already constexpr in C++23)
* `sm::cem::round` maps to `std::round` (already constexpr in C++23)
* `sm::cem::sin` maps to `std::sin`
* `sm::cem::cos` maps to `std::cos`
* `sm::cem::tan` maps to `std::tan`
* `sm::cem::asin` maps to `std::asin`
* `sm::cem::acos` maps to `std::acos`
* `sm::cem::atan` maps to `std::atan`
* `sm::cem::atan2` maps to `std::atan2`
