---
layout: page
title: sm::algo::functions
parent: Reference
nav_order: 15
permalink: /ref/algo/
---
# sm::algo
{: .no_toc}
## Miscellaneous free functions
{: .no_toc}
```c++
import sm.algo;
import sm.algo.edgeconv_2d;
import sm.algo.onoff;
import sm.centroid;
import sm.boxfilter;
```

Module files: [sm/algo.cppm](https://github.com/sebsjames/maths/blob/main/sm/algo.cppm), [sm/centroid.cppm](https://github.com/sebsjames/maths/blob/main/sm/centroid.cppm), [sm/boxfilter.cppm](https://github.com/sebsjames/maths/blob/main/sm/boxfilter.cppm), [sm/edgeconv.cppm](https://github.com/sebsjames/maths/blob/main/sm/edgeconv.cppm), [sm/onoff.cppm](https://github.com/sebsjames/maths/blob/main/sm/onoff.cppm). Test code: [tests/algo_1](https://github.com/sebsjames/maths/blob/main/tests/algo_1.cpp), [tests/algo_sigfigs1](https://github.com/sebsjames/maths/blob/main/tests/algo_sigfigs1.cpp), [tests/algo_roundtocol1](https://github.com/sebsjames/maths/blob/main/tests/algo_roundtocol1.cpp), [tests/algo_ransac](https://github.com/sebsjames/maths/blob/main/tests/algo_ransac.cpp), [tests/zernike1](https://github.com/sebsjames/maths/blob/main/tests/zernike1.cpp), [tests/boxfilter1](https://github.com/sebsjames/maths/blob/main/tests/boxfilter1.cpp).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::algo` is a namespace of free functions for miscellaneous small algorithms including number formatting, angle wrapping, combinatorics and special functions, sorting, simple statistics and line fitting, centroids, box filtering, edge convolution and on-centre/off-surround filtering.

Unlike most of the other namespaces documented here, `sm::algo` is populated by five separate module files, so `import sm.algo;` alone doesn't give you everything. See [Importing](#importing) below.


## Importing

| To use... | `import` |
|---|---|
| significant figures, angle wrapping, factorial, spherical harmonics, Zernike polynomials, sorting, statistics, linear regression, ransac | `sm.algo` |
| Centroid functinos | `sm.centroid` |
| Box filter blurring functions | `sm.boxfilter` |
| 2D edge convolution | `sm.algo.edgeconv_2d` |
| Oncentre/offsurround filters | `sm.algo.onoff` |

All five modules put their functions into the same `sm::algo` namespace, so you call every one of them as `sm::algo::whatever(...)` regardless of which module you imported it from.

This design may be reviewed in future.

## Numbers and significant figures

`significant_cols` finds which base-10 columns of a floating point number are significant, returning an [`sm::interval`](/maths/ref/interval/) whose `max` is the order of magnitude of the largest column and `min` that of the smallest significant one:
```c++
sm::interval<int> sf = sm::algo::significant_cols (4.123f); // {-3, 0}
```
`significant_figs` is the number of significant figures implied by that interval (its span), and `round_to_col` rounds a number to a given base-10 column (column `0` is the units column, `1` is tens, `-1` is tenths, and so on):
```c++
int nsf = sm::algo::significant_figs (4.123f);  // 4
float r = sm::algo::round_to_col (1.2345f, -2); // 1.23f
```

## Angles

`zero_to_twopi` and `minus_pi_to_pi` wrap an angle (in radians) into `[0, 2*pi)` or `[-pi, pi)` respectively, modifying their argument in place:
```c++
float rad = 10.0f;
sm::algo::zero_to_twopi (rad); // rad == 3.716814693f
```

## Combinatorics and special functions

`factorial<T, I>(n)` returns *n!* as type `T`. `Nlm` computes the normalization constant for a real spherical harmonic, and `Plm` wraps `std::assoc_legendre` to allow a signed order `m` (it passes `abs(m)` through). `real_spherical_harmonic` puts the two together, with an overload that accepts a pre-computed `Nlm` if you're evaluating many `(l, m)` pairs at the same `Nlm`:
```c++
double n42 = sm::algo::Nlm<double> (4u, 2); // l=4, m=2
double y42 = sm::algo::real_spherical_harmonic<double> (4u, 2, phi, theta);
```
**Note:** `Plm` and `real_spherical_harmonic` are compiled out entirely on Apple/libc++ platforms, which lack `std::assoc_legendre` at the time of writing.

`zern_radial_poly` computes the Zernike radial polynomial `R_nm(rho)` for `rho` in `[0, 1]`, and `zern_polynomial` combines a radial value with the angular term to give the complex Zernike polynomial `V_nm(rho, theta)`:
```c++
double Rnm = sm::algo::zern_radial_poly (4u, 2, 0.4); // n=4, m=2, rho=0.4
std::complex<double> Vnm = sm::algo::zern_polynomial (2, Rnm, theta);
```

## Sorting

`bubble_sort_hi_to_lo` and `bubble_sort_lo_to_hi` sort a `std::vector<T>` indirectly: the input values are left untouched, and an index vector is filled in with the order that would sort them:
```c++
std::vector<float> values = { 3.0f, 1.0f, 2.0f };
std::vector<std::uint32_t> indices (values.size());
sm::algo::bubble_sort_hi_to_lo (values, indices); // indices == {0, 2, 1}
```

## Statistics and line fitting

`meansos` returns the mean and the sum of squared deviations from the mean of any container of a scalar type, as an `sm::vec<T, 2>`:
```c++
std::vector<double> data = { 1.0, 2.0, 3.0, 4.0 };
sm::vec<double, 2> ms = sm::algo::meansos<std::vector, double> (data); // {mean, sum-of-squared-deviations}
```
`covariance` computes the covariance of two equally-sized containers (throwing `std::runtime_error` if they're empty or mismatched in size); there's an overload that takes pre-computed means to avoid recomputing them. `linregr` performs ordinary least-squares linear regression, returning the gradient and offset (`m` and `c` from `y = mx + c`) as an `sm::vec<T, 2>`:
```c++
sm::vec<double, 2> mc = sm::algo::linregr<std::vector, double> (x, y); // {m, c}
```

`ransac` fits the same linear model, but robustly. It repeatedly picks two random points, fits a line through them, counts inliers within `inlier_threshold` of that line, and keeps the best-scoring line, before finally re-fitting `linregr` over just the inliers it found. This makes it far less sensitive to outliers than `linregr` on its own:
```c++
sm::vec<float, 2> mc = sm::algo::ransac<std::vector, float> (x, y, 300, 0.45f, 70, 42u);
// x, y, max_iterations=200, inlier_threshold=1, min_inliers=2, seed=(randomized if omitted)
```
It throws `std::runtime_error` for empty or mismatched-size inputs, fewer than two points, `max_iterations < 1` or a non-positive `inlier_threshold`. If it can't find at least `max(2, min_inliers)` inliers in any iteration, it falls back to plain `linregr` over all the data.

## Centroids

The generic `centroid` function computes the centroid of any container of point-like objects. It works both for objects with `.x`/`.y` members (such as `cv::Point`) and for any iterable fixed-dimension type such as `std::array<float, N>`:
```c++
std::vector<std::array<float, 3>> pts = { {0,0,0}, {1,0,0}, {0,1,0} };
std::array<float, 3> c = sm::algo::centroid<std::vector, std::array<float, 3>> (pts);
```
`centroid2D` and `centroid3D` are simpler, more specific alternatives: they take either a `std::vector<sm::vec<T, 2>>`, or a flat, interleaved `std::vector<T>` of `x1,y1,x2,y2,...` (or `x1,y1,z1,x2,y2,z2,...` for 3D), and return an `sm::vec<T, 2>` (or `std::array<T, 3>`):
```c++
sm::vec<float, 2> c2 = sm::algo::centroid2D (std::vector<sm::vec<float, 2>>{ {0,0}, {2,0}, {0,2} });
std::array<float, 3> c3 = sm::algo::centroid3D (std::vector<float>{ 0,0,0, 1,0,0, 0,1,0 });
```
There's also a `centroid3D` overload specifically for exactly four 3D points, packed into a `std::array<T, 12>`.

## Box filtering

`import sm.boxfilter;` gives you three overloads of `boxfilter_2d`, all applying a square, horizontally-wrapping box filter to image-like data (a flat array interpreted as a rectangle of width `w`, or `w` by `h`), and all requiring `boxside` to be odd:
```c++
sm::vvec<float> input (img_w * img_h, 0.0f);
sm::vvec<float> output (img_w * img_h, 0.0f);
sm::algo::boxfilter_2d<float, 17, img_w> (input, output); // 17x17 box filter, compile-time width img_w
```
The first overload (above) takes `sm::vvec`s and a compile-time width `w`; a second takes fixed-size `std::array`s with both `w` and `h` given at compile time; a third takes `sm::vvec`s with the width `w` passed as a runtime argument instead of a template parameter. All three accept a template parameter `onlysum` (default `false`) to sum the box's contents without dividing by its area, and a template parameter `T_o` (default `T`) so the output can be a different type from the input.

**Note:** the `sm::vvec`-based overloads (the first and third) check for an even `boxside` at runtime, inside an `if constexpr`, so if you instantiate one with an even `boxside`, it will always throw `std::runtime_error` when called. The fixed-size `std::array` overload instead uses `static_assert`, so an even `boxside` there is a compile error.

## Edge convolution

`import sm.algo.edgeconv_2d;` for `edgeconv_2d`, which computes the vertical and horizontal first differences ("edges") of image-like data laid out bottom-left to top-right, with horizontal wrapping (the rightmost column's vertical edge wraps to the leftmost column) but no vertical wrapping (the top row's horizontal edges are set to zero):
```c++
sm::vvec<float> data (w * h, 0.0f);
sm::vvec<float> v_edges (w * h, 0.0f);
sm::vvec<float> h_edges (w * h, 0.0f);
sm::algo::edgeconv_2d<float, w> (data, v_edges, h_edges);
```
Template booleans `invert_vert_edges` and `invert_horz_edges` (both default `false`) flip the sign convention. A second overload does the same thing for fixed-size `sm::vec<T, w*h>` data instead of `sm::vvec<T>`.

## On-centre, off-surround filtering

`import sm.algo.onoff;` for `oncentre_offsurround`, which computes, for each pixel, its own value minus the mean of its (up to 8) neighbours, for image-like data of width `w`:
```c++
sm::vvec<float> data (w * h, 0.0f);
sm::vvec<float> result (w * h, 0.0f);
sm::algo::oncentre_offsurround<float, w> (data, result); // horz_wrap defaults to true
```
The template boolean `horz_wrap` (default `true`) controls whether the leftmost and rightmost columns wrap around to one another; there's no equivalent option for the top and bottom rows, which always just have fewer neighbours. Throws `std::runtime_error` if `result` is a different size from `data`, or if they alias the same memory.

*This page was authored with AI, based on human written code in algo.cppm, centroid.cppm, boxfilter.cppm, edgeconv.cppm and onoff.cppm. It was reviewed by Seb James*
