---
layout: page
title: sm::mathconst
parent: Reference
nav_order: 4
permalink: /ref/mathconst/
---
# sm::mathconst<>
{: .no_toc }
## Templated mathematical constants
{: .no_toc }

```c++
#include <sm/mathconst>
```
Header file: [<sm/mathconst>](https://github.com/sebsjames/maths/blob/main/sm/mathconst).

**Table of Contents**

- TOC
{:toc}

## Summary

Templated mathematical constants in the `sm` namespace.
Write constant values into your programs in unambiguous type.

For example, for the constant Pi in single precision floating point:

```c++
float pi = sm::mathconst<float>::pi;
```
or in double precision:
```c++
double pi = sm::mathconst<double>::pi;
```

Here's a list of the available values, presented in single precision:

```c++
float c = {};

c = sm::mathconst<float>::zero;  // In case you want 0 in this 'language'
c = sm::mathconst<float>::one;   // ...or '1'

// Logs
c = sm::mathconst<float>::e;     // base of the natural log
c = sm::mathconst<float>::ln_10; // Natural log of 10
c = sm::mathconst<float>::ln_2;  // Natural log of 2
c = sm::mathconst<float>::ln_pi; // Natural log of pi

// Roots
c = sm::mathconst<float>::root_2;          // square root of 2
c = sm::mathconst<float>::one_over_root_2; // 1 / sqrt (2)
c = sm::mathconst<float>::root_3;          // square root of 3
c = sm::mathconst<float>::one_over_root_3;
c = sm::mathconst<float>::two_over_root_3;
c = sm::mathconst<float>::one_over_2_root_3;
c = sm::mathconst<float>::root_3_over_2;
c = sm::mathconst<float>::root_4;
c = sm::mathconst<float>::root_5; // etc up to root_10

// Pi
c = sm::mathconst<float>::pi;
c = sm::mathconst<float>::pi_over_2;
c = sm::mathconst<float>::three_pi_over_2;
c = sm::mathconst<float>::pi_over_3;
c = sm::mathconst<float>::two_pi_over_3;
c = sm::mathconst<float>::four_pi_over_3;
c = sm::mathconst<float>::five_pi_over_3;

c = sm::mathconst<float>::pi_over_4;
c = sm::mathconst<float>::three_pi_over_4
c = sm::mathconst<float>::five_pi_over_4
c = sm::mathconst<float>::seven_pi_over_4

c = sm::mathconst<float>::pi_over_5; // etc up to pi_over_10 plus:
c = sm::mathconst<float>::pi_over_16;
c = sm::mathconst<float>::pi_over_18;
c = sm::mathconst<float>::pi_over_32;

c = sm::mathconst<float>::two_pi; // etc up to nine_pi

c = sm::mathconst<float>::one_over_pi;
c = sm::mathconst<float>::one_over_two_pi; // etc to one_over_nine_pi

c = sm::mathconst<float>::pi_over_360;
c = sm::mathconst<float>::two_pi_over_360;

c = sm::mathconst<float>::deg2rad; // 2pi / 360
c = sm::mathconst<float>::rad2deg; // 360 / 2pi
```
