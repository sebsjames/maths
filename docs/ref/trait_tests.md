---
layout: page
title: sm::trait_tests
parent: Reference
nav_order: 32
permalink: /ref/trait_tests/
---
# sm::trait_tests
{: .no_toc}
## Compile-time type trait tests
{: .no_toc}
```c++
import sm.trait_tests;
```

Module file: [sm/trait_tests.cppm](https://github.com/sebsjames/maths/blob/main/sm/trait_tests.cppm). Test code: [tests/test_trait_tests](https://github.com/sebsjames/maths/blob/main/tests/test_trait_tests.cpp), [tests/trait_tests_number_type](https://github.com/sebsjames/maths/blob/main/tests/trait_tests_number_type.cpp), [tests/trait_tests_has_size](https://github.com/sebsjames/maths/blob/main/tests/trait_tests_has_size.cpp).

**Note:** despite the module's name, everything it exports lives directly in `sm::` - there's no `sm::trait_tests` sub-namespace.

**Table of Contents**

- TOC
{:toc}

## Summary

`sm.trait_tests` is a collection of SFINAE-based type traits (each a small class exposing a `static constexpr bool value`, in the classic `std::is_*`-style idiom) used throughout this library for `if constexpr` branches and `std::enable_if_t` overload selection - for example, [`sm::algo::centroid`](/maths/ref/algo/#centroids) uses `has_xy_members` to decide whether to average `.x`/`.y` or iterate a general container, and [`sm::mat`](/maths/ref/mat/#complex-matrices) uses `is_complex` to allow complex matrix elements.

## Arithmetic capability

```c++
sm::has_subtraction<float>::value;               // true
sm::has_subtraction<std::vector<float>>::value;  // false - std::vector has no operator-
sm::has_addition<sm::vec<float, 3>>::value;      // true - sm::vec defines operator+
```
`has_subtraction<T>` and `has_addition<T>` test whether `T{} - T{}` (respectively `+`) is a valid expression.

## Point-like and pair-like shapes

These three test for the shape of a type, rather than a specific base class - useful for accepting 'anything that looks like a 2D point' (e.g. `cv::Point`) generically:
```c++
sm::has_xy_methods<T>::value;          // does T have callable .x() and .y() methods?
sm::has_xy_members<T>::value;          // does T have plain .x and .y data members?
sm::has_firstsecond_members<T>::value; // does T have .first and .second members (e.g. std::pair)?
```

## Container tests

```c++
sm::array_access_possible<T>::value;  // can you write t[0] on a T?
sm::is_copyable_container<T>::value;  // does T::const_iterator satisfy copy/assign/destruct/swap/== ?
sm::has_resize_method<T>::value;      // does T have a resize(size_t) method?
sm::has_size_const_method<T>::value;  // does T have a const size() method?
```
Verified while writing this page: `has_size_const_method` is `true` for `sm::vec<float>`, `std::deque<int>` and `std::string`, and `false` for `std::complex<float>`; `has_resize_method` is `true` for `sm::vvec<float>` and `false` for `float` or `sm::vec<float, 5>` (a fixed-size type).

`is_copyable_fixedsize<T>` combines several of the above into a single, more useful test: 'is `T` a simple, fixed-size, copyable container?' (true for `std::array`/`sm::vec`, false for resizable containers like `std::vector`/`sm::vvec`/`std::list`/`std::deque`, and false for plain scalars):
```c++
sm::is_copyable_fixedsize<std::array<float, 2>>::value; // true
sm::is_copyable_fixedsize<sm::vec<double, 56>>::value;  // true
sm::is_copyable_fixedsize<std::vector<double>>::value;  // false (it's copyable, but resizable)
sm::is_copyable_fixedsize<double>::value;               // false (not a container at all)
```
It works by requiring `is_copyable_container == true` **and** `has_size_const_method == true` **and** `has_resize_method == false` - i.e. it distinguishes `std::array`/`sm::vec` from `std::vector`/`sm::vvec` precisely by the *absence* of a `resize` method.

## Numeric type tests

```c++
sm::is_complex<T>::value; // does T have .real() and .imag() methods? (true for std::complex<...>)
```
`value_type<T>`/`value_type_t<T>` generalize `T::value_type` to also work for scalar types that don't have one:
```c++
sm::value_type_t<std::array<float, 2>>; // float  (T::value_type)
sm::value_type_t<float>;                // float  (T itself, since float has no value_type)
```

`number_type<T>` is the most-used of these traits (see [`sm::scale`](/maths/ref/scale/)): it classifies `T` into one of five categories via its `value` member:

| `value` | Meaning |
|---|---|
| `1` | a scalar (`float`, `double`, `int`, ...) |
| `2` | a complex scalar (`std::complex<float>`, ...) |
| `0` | a container of scalars - i.e. a mathematical vector (`sm::vec<float,3>`, `std::vector<int>`, `std::deque<double>`, ...) |
| `3` | a container of complex scalars (`sm::vec<std::complex<float>, 2>`, ...) |
| `-1` | none of the above (e.g. `std::pair<float,float>`, or a `std::complex<std::array<float,3>>` - a 'complex of vectors' isn't accepted as meaningful) |

Verified while writing this page (`tests/trait_tests_number_type.cpp`): all five cases above match these exact values, including the deliberately-rejected `std::complex<std::array<float,3>>` and `std::pair<float,float>` cases.

## Other

```c++
sm::is_constexpr_constructible<T>(0); // true if T{} is a valid constant expression
```

*This page was authored with AI, based on human written code in trait_tests.cppm.*
