---
layout: page
title: sm::bezcurvepath
parent: Reference
nav_order: 27
permalink: /ref/bezcurvepath/
---
# sm::bezcurvepath
{: .no_toc}
## A path made of Bezier curves
{: .no_toc}
```c++
import sm.bezcurvepath;
```

Module file: [sm/bezcurvepath.cppm](https://github.com/sebsjames/maths/blob/main/sm/bezcurvepath.cppm). Test code: [tests/bez2](https://github.com/sebsjames/maths/blob/main/tests/bez2.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary

A `bezcurvepath<F, order>` is a sequence of same-order [`sm::bezcurve`](/maths/ref/bezcurve/) instances, joined end to end into a single path - think of it as one continuous curve made of several Bezier segments.

`bezcurvepath` was written to define boundary paths for the class `sm::hexgrid`.
For an example, see the test program [tests/bez2](https://github.com/sebsjames/maths/blob/main/tests/bez2.cpp), which builds a closed rectangular path and uses it as a [`sm::hexgrid`](/maths/ref/hexgrid/) boundary. [`sm::cartgrid`](/maths/ref/cartgrid/) uses a `bezcurvepath` for the same purpose.

`order` defaults to `3` (cubic), since that's what `sm::hexgrid` and `sm::cartgrid` require when you hand them a `bezcurvepath` as a domain boundary.

## Building a path

```c++
// Create some cubic bezcurves
sm::vec<float,2> v1 = {-0.28f, 0.0f}, v2 = {0.28f, 0.0f}, v3 = {0.28f, 0.45f}, v4 = {-0.28f, 0.45f};
sm::bezcurve<float, 3> c1 (v1, v2, (v1+v2)/2.0f, (v1+v2)/2.0f);
sm::bezcurve<float, 3> c2 (v2, v3, (v2+v3)/2.0f, (v2+v3)/2.0f);
sm::bezcurve<float, 3> c3 (v3, v4, (v3+v4)/2.0f, (v3+v4)/2.0f);
sm::bezcurve<float, 3> c4 (v4, v1, (v4+v1)/2.0f, (v4+v1)/2.0f);

// Combine them into a bezcurvepath
sm::bezcurvepath<float, 3> path;
path.add_curve (c1);
path.add_curve (c2);
path.add_curve (c3);
path.add_curve (c4); // a closed, rectangular boundary made of 4 cubic curves
```
`bezcurvepath::add_curve` appends a curve to the path's internal `std::list`; the first time it's called on an empty path, it also captures that curve's own (scaled) initial point as the path's `initial_coordinate`. `bezcurvepath::remove_curve` removes the last curve.

You can check if a path is empty: `bezcurvepath::is_null()` is `true` if the path has no curves at all.

`bezcurvepath::reset()` clears the bezcurvepath (curves, `initial_coordinate`, `scale`, `name`).

## Scaling

`set_scale (s)` sets the path's own `scale` member, multiplies it into `initial_coordinate`, and propagates the same call to every curve currently in the path:
```c++
path.set_scale (1.0f / 25.4f); // e.g. converting from inches to mm
```
Curves added *after* calling `set_scale` won't automatically pick up the path's scale - call `set_scale` again, or scale the curve yourself, if you add more curves later.

## Sampling the path

`compute_points (step, invert_y = false)` walks every curve in the path in order, sampling at even arc-length intervals of `step`, and stitches the sampling seamlessly across curve boundaries. It uses each curve's 'null coordinate with remaining distance' convention (see [`bezcurve`](/maths/ref/bezcurve/#evaluating-the-curve)) to correctly handle points that would otherwise fall exactly on a curve joint (and cause duplication), or spill over into the next curve and be skipped.

Once the path points have been computed, you can access the coordinates with `bezcurvepath::get_points`, and the curve tangents and normals with `get_tangents` and `get_normals`.

```c++
path.compute_points (0.1f);
std::vector<sm::bezcoord<float>> pts = path.get_points();    // sampled positions
std::vector<sm::bezcoord<float>> tans = path.get_tangents(); // unit tangent at each point
std::vector<sm::bezcoord<float>> norms = path.get_normals(); // unit normal at each point
```
If you'd rather specify how many points you want than what spacing to use, `compute_points (n_points, invert_y = false)` repeatedly adjusts the step size (successively doubling/halving it) until sampling produces exactly `n_points` points, falling back to a console message (rather than throwing) if it can't converge on a step size small enough to hit the target count exactly:
```c++
path.compute_points (5u); // exactly 5 points, spacing chosen automatically
```

Pass `invert_y = true` to either overload to flip every sampled point's y coordinate - useful because SVG paths (a common source for `bezcurvepath` data) use a left-handed coordinate system.

`bezcurvepath::get_end_to_end()` returns the straight-line ('as the crow flies') distance from `initial_coordinate` to the final point of the last curve in the path, using the path's `scale`.

## Other utilities

You can get the path's centroid with the static function `bezcurvepath::get_centroid`:
```c++
sm::vec<float, 2> centroid = sm::bezcurvepath<float, 3>::get_centroid (path.get_points());
```
The `output` function was written to help debug the class:
```c++
path.output(); // dump name, initial coordinate and curve count to std::cout
```
It's possible to save a set of points on the path into a CSV file:
```c++
path.save (0.1f);   // write sampled points to "<name>.csv", one curve's output(step) at a time
```
`save` uses the path's `name` member to build its filename, and expects the arc-length `step` you pass in to already be in the same (pre-scaled) units the curves themselves use.

*This page was authored with AI, based on human written code in bezcurvepath.cppm. Reviewed by Seb James.*
