---
layout: page
title: sm::binomial (namespace)
parent: Reference
nav_order: 28
permalink: /ref/binomial/
---
# sm::binomial
{: .no_toc}
## Binomial coefficients and Pascal's triangle
{: .no_toc}
```c++
import sm.binomial;
```

Module file: [sm/binomial.cppm](https://github.com/sebsjames/maths/blob/main/sm/binomial.cppm).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::binomial` is a namespace of small, `constexpr`-capable free functions for computing binomial coefficients, either one at a time or as a precomputed table (Pascal's triangle).

```c++
std::uint64_t c = sm::binomial::nk (5, 2); // (5 choose 2) == 10
```
`nk (n, k)` computes the binomial coefficient directly, using the symmetry `k = min(k, n-k)` and an iterative product/divide (rather than computing large factorials), so it stays within range for much larger `n` than a naive `n!/(k!(n-k)!)` would.

It is used internally by [`sm::bezcurve`](/maths/ref/bezcurve/) to look up the binomial coefficients needed for its Bernstein-polynomial and matrix-basis Bezier curve evaluators.
## Building and looking up a table

If you need many binomial coefficients (e.g. all the coefficients for a fixed-degree Bernstein basis), it's cheaper to build a table once, ahead of time, and look values up from it:
```c++
constexpr auto pt = sm::binomial::make_pascals_triangle<7>(); // first 7 rows of Pascal's triangle
// pt.size() == sm::binomial::pascal_size<7>() == 28 (the triangular number 7*8/2)

std::uint64_t c = sm::binomial::lookup<7> (4, 2, pt); // (4 choose 2) == 6
double        d = sm::binomial::lookup<7, double> (4, 2, pt); // same value, returned as double
```
`make_pascals_triangle<N>()` returns a flat `std::array<std::uint64_t, N*(N+1)/2>` holding rows `0` to `N-1` of Pascal's triangle, packed row after row (row `n` has `n + 1` entries, for `(n choose 0)` through `(n choose n)`). `pascal_size<N>()` is just that array's size, computed the same way, in case you need it independently of actually building the table.

`lookup<N, T>(n, k, pascals_triangle)` finds `(n choose k)` in a table built with the same `N`, returning the result as type `T` (default `std::uint64_t`). If `(n, k)` falls outside the table - e.g. `n >= N` - it returns `std::numeric_limits<T>::max()` as a sentinel, rather than throwing or asserting:
```c++
auto oob = sm::binomial::lookup<7> (10, 3, pt); // row 10 doesn't exist for a 7-row table
// oob == std::numeric_limits<std::uint64_t>::max()
```

*This page was authored with AI, based on human written code in binomial.cppm. Reviewed by Seb James*
