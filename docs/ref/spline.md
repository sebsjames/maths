---
layout: page
title: sm::spline
parent: Reference
nav_order: 33
permalink: /ref/spline/
---
# sm::spline
{: .no_toc}
## Natural cubic spline interpolation
{: .no_toc}
```c++
import sm.spline;
```

Module file: [sm/spline.cppm](https://github.com/sebsjames/maths/blob/main/sm/spline.cppm). Test code: [tests/spline1](https://github.com/sebsjames/maths/blob/main/tests/spline1.cpp). Used internally by [`sm::random_walk`](/maths/ref/random_walk/) to smooth a randomly-generated acceleration profile.

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::spline<F, N>` fits a natural cubic spline through a fixed number, `N`, of 2D points, then lets you evaluate the resulting piecewise-cubic curve at any x. 'Natural' means the boundary condition at each end of the spline is that the second derivative is zero there - the usual default for cubic spline interpolation.

`N` is a template parameter, not a runtime size - you need to know how many points you're fitting through at compile time.

## Creating a spline

```c++
sm::spline<float, 4> spl;
spl.p[0] = {1, 2};
spl.p[1] = {3, 3};
spl.p[2] = {5, 9};
spl.p[3] = {8, 10};
spl.compute_coefficients();
```
Or, construct directly from a filled-in `sm::vec<sm::vec<F,2>, N>` of points, which computes the coefficients for you:
```c++
sm::vec<sm::vec<float, 2>, 4> pts = { sm::vec<float,2>{1,2}, {3,3}, {5,9}, {8,10} };
sm::spline<float, 4> spl (pts);
```
**Your points must be supplied in increasing order of x** - `compute()` (below) relies on this ordering and doesn't check or sort for you.

Internally, `compute_coefficients()` sets up and solves a `4(N-1) x 4(N-1)` linear system for the `4` coefficients of each of the `N-1` cubic segments, using [`sm::mat`](/maths/ref/mat/#solving-linear-systems)'s `row_echelon_form_inplace`/`back_substitution`. The equations enforce: each segment passing exactly through its two endpoints, matching first derivatives at every interior knot, matching second derivatives at every interior knot, and the two natural (zero second-derivative) boundary conditions at the very first and very last point.

## Evaluating the spline

```c++
float y = spl.compute (4.0f);              // evaluate at a single x
sm::vvec<float> ys = spl.compute (xs);      // evaluate at every x in an sm::vvec<float>
```
`compute` finds which of the `N-1` segments `x` falls into by walking the points in order and picking the first one where `x <= p[i][0]`, then evaluates that segment's cubic.

**Careful:** for any `x` greater than the last point's x-coordinate, `compute` returns `0`, not an extrapolated value and not an error - the search loop never finds a segment to use, and the return value is left at its initial `0`. Verified while writing this page (via `sm::cubic_spline_expansion`, which calls `compute` in a loop): requesting values past the last knot silently gives a run of zeros rather than sensible (or even garbage) extrapolated numbers. Only evaluate `compute` within `[p[0][0], p[N-1][0]]`.

## Worked example

From `tests/spline1.cpp`, fitting through 4 points and sampling 40 values across the domain (verified while writing this page):
```c++
sm::spline<float, 4> spl;
spl.p[0] = {1, 2}; spl.p[1] = {3, 3}; spl.p[2] = {5, 9}; spl.p[3] = {8, 10};
spl.compute_coefficients();

sm::vvec<float> x;
x.linspace (1, 8, 40);
sm::vvec<float> y = spl.compute (x);
// spl.compute(1) == 2, spl.compute(3) == 3, spl.compute(5) == 9, spl.compute(8) == 10
```

*This page was authored with AI, based on human written code in spline.cppm.*
