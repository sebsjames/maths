---
layout: page
title: sm::evenspacing::functions
parent: Reference
nav_order: 31
permalink: /ref/evenspacing/
---
# sm::evenspacing
{: .no_toc}
## Evenly-spaced points along the graph of a 1D function
{: .no_toc}
```c++
import sm.evenspacing;
```

Module file: [sm/evenspacing.cppm](https://github.com/sebsjames/maths/blob/main/sm/evenspacing.cppm). Test code: [tests/evenspacing1](https://github.com/sebsjames/maths/blob/main/tests/evenspacing1.cpp).

**Table of Contents**

- TOC
{:toc}

## Summary

Given an arbitrary function `f(x)` (as a `std::function<F(const F)>`), `sm::evenspacing` finds points along its graph that are evenly spaced by *arc length* - i.e. by distance travelled along the curve - rather than evenly spaced in `x`. This matters whenever `f` isn't roughly linear: evenly-spaced-in-`x` samples bunch up wherever the curve is flat and thin out wherever it's steep, whereas evenly-spaced-by-arc-length samples give you a visually/geometrically uniform set of points.

## Estimating a curve's length

`estimate_length` approximates the length of `f(x)` between `x0` and `x1` by summing the straight-line distances between `n` samples, evenly spaced in `x`:
```c++
float f_lin (const float x) { return x; }
float len = sm::evenspacing::estimate_length<float> (0.0f, 10.0f, 2, f_lin); // 14.1421... (== sqrt(2) * 10)
```
This is a polyline approximation, not an exact integral - accuracy improves as `n` increases, at the cost of `n - 1` extra evaluations of `f`.

## Finding evenly-spaced coordinates

`find_coordinates` returns `n` coordinates along `f(x)` between `xs` and `xe`, evenly spaced by arc length, with the first and last always being exactly `(xs, f(xs))` and `(xe, f(xe))`:
```c++
sm::vvec<sm::vec<float, 2>> pts = sm::evenspacing::find_coordinates<float> (0.0f, 10.0f, 3, f_lin);
// pts == ( (0,0), (5,5), (10,10) )
```
Internally, it first calls `estimate_length` with `n * 100` samples to get a good target spacing, then finds each intermediate point with a bisection search along `f`, looking for the point at exactly that arc-length distance from the previous one. Verified while writing this page: for `f(x) = x` between 0 and 10, requesting 5 points gives `(0,0), (2.5,2.5), (4.998,4.998), (7.499,7.499), (10,10)` - the small deviations from an exact `2.5` step come from the bisection search's tolerance, not from `estimate_length`'s polyline approximation (which is exact for a straight line).

**Careful:**
* If `n < 2`, `find_coordinates` returns a vector of `n` zero-initialized coordinates without attempting to compute anything (no error is raised).
* Each intermediate point's bisection search is capped at 100,000 iterations; if it doesn't converge within that, the search simply stops and whatever point it had settled on so far is used, without raising an error.

*This page was authored with AI, based on human written code in evenspacing.cppm. Reviewed by Seb James*
