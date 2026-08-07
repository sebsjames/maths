---
layout: page
title: sm::bezcurve
parent: Reference
nav_order: 26
permalink: /ref/bezcurve/
---
# sm::bezcurve
{: .no_toc}
## A general-order Bezier curve
{: .no_toc}
```c++
import sm.bezcurve;
```

Module file: [sm/bezcurve.cppm](https://github.com/sebsjames/maths/blob/main/sm/bezcurve.cppm). Test and example code:
[examples/bezcurve](https://github.com/sebsjames/maths/blob/main/examples/bezcurve.cpp)
[tests/bez1](https://github.com/sebsjames/maths/blob/main/tests/bez1.cpp)
[tests/bezcurves](https://github.com/sebsjames/maths/blob/main/tests/bezcurves.cpp)
[tests/bezfit](https://github.com/sebsjames/maths/blob/main/tests/bezfit.cpp)
[tests/bezsplit](https://github.com/sebsjames/maths/blob/main/tests/bezsplit.cpp)
[tests/twocurves](https://github.com/sebsjames/maths/blob/main/tests/twocurves.cpp)
[tests/bezmatrix](https://github.com/sebsjames/maths/blob/main/tests/bezmatrix.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::bezcurve<F, order>` represents a single Bezier curve of a fixed polynomial degree, `order` (`1` for a line, `2` for a quadratic, `3` for a cubic, and so on) - `order` has no default value and must always be given explicitly. Its `order + 1` control points are stored as an `sm::mat<F, order + 1, 2>` (one row per control point, columns for x and y) rather than as an array of `sm::vec`s.

The class follows Cohen & Riesenfeld's (1982) general matrix representation of Bezier curves: it precomputes a basis-conversion matrix `M` and caches `M * C` (`C` being the control-point matrix) so that evaluating a point at parameter `t` is a single small matrix multiply, valid for any `order`. It also provides the classical direct Bernstein-polynomial summation as a cross-check. There's a compile-time ceiling on `order` of around 19 (`static_assert`, tied to a Pascal's-triangle lookup table sized for orders up to 20) - plenty for any practical use.

`sm::bezcurve` isn't `constexpr`-capable (it uses `std::cout`, `std::format`, exceptions, and - for one curve-fitting overload - an internal [`sm::nm_simplex`](/maths/ref/nm_simplex/) optimization).

## Creating a curve

The most general constructor takes all `order + 1` control points (including both endpoints) as an `sm::vvec`:
```c++
sm::vvec<sm::vec<float,2>> c = { {1,1}, {2,8}, {9,8}, {10,1} };
sm::bezcurve<float, 3> cv (c); // order 3 -> needs exactly 4 points
```
**Careful:** this constructor's control-point-copying loop unconditionally prints two lines of debug output per control point to `std::cout` - this isn't gated by any flag; it's just how the current code behaves. (The source comment above the `std::cout` call reads "A cout here seems to be critical to avoid segfault", suggesting it's a workaround left in place rather than deliberate logging.)

You can also pass the control points directly as an `sm::mat<F, order+1, 2>`, or use one of the fixed-order convenience constructors:
```c++
sm::vec<float,2> ip = {1,1}, fp = {10,1}, c1 = {5,5}, c2 = {2,-4};
sm::bezcurve<float, 3> cubic  (ip, fp, c1, c2); // order == 3 only: initial point, final point, 2 control points
sm::bezcurve<float, 2> quad   (ip, fp, c1);     // order == 2 only: initial point, final point, 1 control point
sm::bezcurve<float, 1> line   (ip, fp);         // order == 1 only: just the two endpoints
```
Or, for any order, give the endpoints separately from the interior control points:
```c++
sm::vvec<sm::vec<float,2>> interior = { c1, c2 }; // order-1 interior points
sm::bezcurve<float, 3> cv (ip, fp, interior);
```
All the point-count-based constructors throw `std::runtime_error` if you supply the wrong number of points for the curve's `order`.

To replace all of a curve's control points after construction, use `update_controls` (also throws on a size mismatch):
```c++
cv.update_controls (c);
```

## Evaluating the curve

`compute_point(t)` evaluates the curve at parameter `t` in `[0, 1]`, throwing `std::runtime_error` outside that range:
```c++
sm::bezcoord<float> pt = cv.compute_point (0.4f);
```
Internally, this dispatches on `order`: closed-form evaluation for `order` 1-3, or the general matrix-multiply method for `order >= 4`. You can also call the underlying evaluators directly, regardless of the curve's actual order, if you want a specific one:
```c++
sm::bezcoord<float> bm = cv.compute_point_matrix (0.4f);   // Cohen-Riesenfeld matrix method (T * M * C)
sm::bezcoord<float> bg = cv.compute_point_general (0.4f);  // direct Bernstein-polynomial summation
sm::bezcoord<float> bc = cv.compute_point_cubic (0.4f);    // closed-form cubic (only meaningful if order == 3)
```
`tests/bezmatrix.cpp` confirms the matrix and general methods agree to within `1e-5` for a cubic curve.

`compute_point(t, l)` starts at parameter `t` and moves a further Euclidean distance `l` along the curve, via a binary search over `t` (the search's tolerance is set as a percentage of `l` with `set_lthresh(F)`, default `1`). If there isn't `l` worth of curve left after `t`, or the search doesn't converge, it returns a **null** `bezcoord` (`is_null() == true`) whose `remaining` field holds the actual distance left. This convention is exactly what lets [`sm::bezcurvepath`](/maths/ref/bezcurvepath/) stitch samples smoothly across a sequence of curves.

For sampling many points at once:
```c++
std::vector<sm::bezcoord<float>> pts    = cv.compute_points (40u);         // 40 points, evenly spaced in t
std::vector<sm::bezcoord<float>> pts_l  = cv.compute_points (1.0f);        // points spaced by Euclidean arc length 1.0
std::vector<sm::bezcoord<float>> pts_x  = cv.compute_points_horz (1.0f);   // points spaced by horizontal (x) distance
```
For the arc-length and horizontal-distance overloads, the *last* element of the returned vector is a null `bezcoord` carrying whatever distance was left over past the last full step - again, the mechanism `bezcurvepath` relies on.

`compute_tangent_normal(t)` returns a `std::pair` of unit `bezcoord`s, `{tangent, normal}`:
```c++
auto [tangent, normal] = cv.compute_tangent_normal (0.4f);
```
For `order > 1` this is built from `derivative<F>()`, which returns the `order` control points of a curve one degree lower (the derivative curve). **Careful:** `derivative`'s template parameter isn't deducible (it's never used in a way the compiler can infer from arguments), so you must always supply it explicitly, e.g. `cv.derivative<float>()`.

## Fitting a curve to points

```c++
cv.fit (points); // points.size() must equal order + 1, exactly
```
This is an *exact interpolation* through the given points (parameterized by their estimated arc-length positions along the curve), not a least-squares fit over more points than the curve has degrees of freedom - `fit` throws `std::runtime_error` if `points.size() != order + 1`. Internally, the fit is computed in `double` precision even when `F` is `float`, because (per the source's own rationale) single precision only gives reliable fits up to around order 4 or 5.
```c++
sm::vvec<sm::vec<float,2>> c = { {-0.28f,0.0f}, {0.28f,0.0f}, {0.28f,0.45f}, {-0.28f,0.45f} };
sm::bezcurve<float, 3> cv;
cv.fit (c);
std::cout << cv.get_order() << std::endl; // 3
```
There's also a three-argument overload, `fit (points, preceding, optimize = false)`, which additionally smooths the tangent direction across the join with a `preceding` curve, and - if `optimize` is `true` - refines the interior control points with an `sm::nm_simplex` search that minimizes `compute_objective(points)`. **Careful:** unlike the single-argument `fit`, this overload (and `optimize = true` in particular) isn't exercised by any test in this repository, so its correctness is less well-established.

`compute_objective(points)` is itself public: it returns the sum of squared distances between arc-length-sampled points on the curve and `points` (or `F{-1}` - a sentinel, not an exception - if the sizes don't line up), and is the quantity the optimizing `fit` overload minimizes.

## Splitting a curve

```c++
auto [c1, c2] = cv.split (0.5f); // control-point matrices of the two halves, split at t=0.5
sm::bezcurve<float, 3> cv1 (c1);
sm::bezcurve<float, 3> cv2 (c2);
```
**Careful:** the only place in this repository that exercises `split` (`tests/bezsplit.cpp`) has that code wrapped in `#if 0` (disabled), immediately after a `// FIXME MAY BE IN WRONG DIRECTION NOW` comment in `bezcurve.cppm` itself. Treat `split`'s correctness as unverified until that FIXME is resolved.

## Scale and other queries

```c++
cv.set_scale (2.0f); // see caveat below
float len_x2 = cv.get_initial_point_scaled().length(); // scale applied
std::uint32_t ord = cv.get_order();
sm::vvec<sm::vec<float,2>> ctrls = cv.get_controls(); // unscaled
```
**Careful:** `set_scale` only affects the coordinates returned by `compute_point`/`compute_points` for `order` 1, 2 and 3 - their closed-form evaluators explicitly multiply by `scale`. For `order >= 4`, `compute_point` dispatches to `compute_point_matrix`, which does **not** apply `scale` at all - so `set_scale` silently has no effect on evaluated points for higher-order curves, even though `get_initial_point_scaled()`/`get_final_point_scaled()` are still computed correctly.

## Output and debugging

```c++
std::cout << cv.output (40u);       // 40 evenly-spaced-in-t sample points, one 'x,y'-ish line each
std::cout << cv.output (0.1f);      // sample points spaced by arc-length step 0.1
std::cout << cv.output_control();   // the raw control-point matrix
```
From `examples/bezcurve.cpp` (verified while writing this page), fitting a cubic to a rounded-rectangle-ish set of 4 points and printing its control matrix:
```
Defined a 3 nd/rd/th order curve
c=[
|    -0.28        ~0       |
|   0.533498   -0.663498   |
|   0.533498     1.1135    |
|    -0.28        0.45     |
];
```

*This page was authored with AI, based on human written code in bezcurve.cppm. Not yet reviewed*
