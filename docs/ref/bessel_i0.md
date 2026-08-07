---
layout: page
title: sm::bessel_i0
parent: Reference
nav_order: 17
permalink: /ref/bessel_i0/
---
# sm::bessel_i0
{: .no_toc}
## Cylindrical Bessel function of the first kind, order 0
{: .no_toc}
```c++
import sm.bessel_i0;
```

Module file: [sm/bessel_i0.cppm](https://github.com/sebsjames/maths/blob/main/sm/bessel_i0.cppm). This is a small, self-contained port of the order-0 case of [Boost's cylindrical Bessel function implementation](https://www.boost.org/doc/libs/release/libs/math/doc/html/math_toolkit/bessel/bessel_first.html) (Copyright Xiaogang Zhang 2006, John Maddock 2017; used under the Boost Software License), reused here by `sm::random`'s von Mises distribution.

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::bessel_i0` is a single function template computing `I0(x)`, the modified Bessel function of the first kind, order 0:

```c++
double y = sm::bessel_i0 (1.0); // 1.26606587099...
```

It's evaluated with one of three rational/exponential polynomial approximations depending on the size of `x` (`x < 7.75`, `7.75 <= x < 50`, and `x >= 50`), each using [`sm::polysolve::evaluate`](/maths/ref/polysolve/) (Horner's method) to evaluate the relevant polynomial. Boost's documented error bounds for these approximations are on the order of 1e-7&ndash;1e-9.

```c++
sm::bessel_i0 (0.0);   // 1
sm::bessel_i0 (5.0);   // 27.2398720809...
sm::bessel_i0 (10.0);  // 2815.71653734...
sm::bessel_i0 (50.0);  // 2.93255377862e+20...
```

`x` can be any floating point type (`float`, `double`, `long double`); there's no upper limit enforced on `x`, but as with any implementation of `I0`, the result grows exponentially, so it will overflow for large enough `x` in whichever floating point type you use.

## Where it's used in this library

[`sm::random`](/maths/ref/randn/)'s von Mises distribution (a circular analogue of the normal distribution) uses `sm::bessel_i0` to normalize its probability density function:

```c++
T prob_density (const T x) const noexcept
{
    return std::exp (this->kappa * std::cos (x - this->mu)) / (sm::mathconst<T>::two_pi * sm::bessel_i0 (this->kappa));
}
```

*This page was authored with AI, based on human written code in bessel_i0.cppm. Checked by Seb James*
