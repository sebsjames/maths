---
layout: page
title: sm::geometry (namespace)
parent: Reference
nav_order: 36
permalink: /ref/geometry/
---
# sm::geometry
{: .no_toc}
## 2D and 3D geometric algorithms
{: .no_toc}
```c++
import sm.geometry;
```

Module file: [sm/geometry.cppm](https://github.com/sebsjames/maths/blob/main/sm/geometry.cppm). Test code:
[tests/geometry_2](https://github.com/sebsjames/maths/blob/main/tests/geometry_2.cpp) (AABB/line)
[tests/geometry_3](https://github.com/sebsjames/maths/blob/main/tests/geometry_3.cpp) (ray/plane/triangle)
[tests/geometry_4](https://github.com/sebsjames/maths/blob/main/tests/geometry_4.cpp) (point-to-segment/edge distance)
[tests/geometry_5](https://github.com/sebsjames/maths/blob/main/tests/geometry_5.cpp) (plane projection)
[tests/geometry_6](https://github.com/sebsjames/maths/blob/main/tests/geometry_6.cpp) (triangle area)
[tests/graham_scan_1](https://github.com/sebsjames/maths/blob/main/tests/graham_scan_1.cpp) (convex hull)
[tests/geometry_xyz_to_latlong](https://github.com/sebsjames/maths/blob/main/tests/geometry_xyz_to_latlong.cpp) and [tests/geometry_sph_projections](https://github.com/sebsjames/maths/blob/main/tests/geometry_sph_projections.cpp) (spherical projections)

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::geometry` is a namespace of functions for computing geometry. There are: 2D line algorithms (point orientation, segment intersection), an implementation of Graham's scan to find a convex hull, 3D ray-intersection tests (against planes, discs, triangles, spheres and axis-aligned boxes) and point-to-segment/edge distances. A nested namespace `sm::geometry::spherical_projection` provides functions that convert between 3D points on a sphere and various 2D map projections.

**A note on 'ray' terminology:** several functions here take a ray defined as an origin `l0` and a vector `l` that is described in the source as giving 'direction and length'. Most functions ignore the ray length, but `ray_point_intersection` will *not* match a point that lies beyond the end of the ray from l0 to l.

## 2D lines

`orientation (p, q, r)` tells you whether three points turn clockwise, anticlockwise, or are colinear:
```c++
sm::rotation_sense rs = sm::geometry::orientation (p, q, r);
// rs may have the value: sm::rotation_sense::clockwise, ::anticlockwise or ::colinear
```
`onsegment (p, q, r)` checks whether `q` lies on the segment `p`-`r`, but only gives a meaningful answer if you already know `p`, `q`, `r` are colinear (e.g. from `orientation`).

`segments_intersect (p1, q1, p2, q2)` tests whether line segments `p1`-`q1` and `p2`-`q2` cross, returning a `std::bitset<2>` (`bit 0`: they intersect; `bit 1`: they're colinear):
```c++
auto bits = sm::geometry::segments_intersect ({0,0}, {2,2}, {0,2}, {2,0});
bool crosses = bits[0]; // true - the two diagonals of a square cross
```
Once you know two segments intersect, `crossing_point (p1, q1, p2, q2)` gives you where - **call it only after `segments_intersect` has confirmed an intersection**, since it assumes one exists:
```c++
sm::vec<float, 2> where = {};
if (crosses == true) {
    where = sm::geometry::crossing_point (p1, q1, p2, q2); // (1, 1) for the example above
}
```

## Convex hull

`sm::geometry::graham_scan (points)` computes the convex hull of a set of 2D points via Graham's scan, returning the hull vertices in anticlockwise order, starting from the lowest (then leftmost) point:
```c++
sm::vvec<sm::vec<float, 2>> points = { {0,0}, {0,1}, {1,0}, {1,1}, {0.2f,0.3f}, {0.1f,0.9f} };
sm::vvec<sm::vec<float, 2>> hull = sm::geometry::graham_scan (points);
// hull == { {0,0}, {1,0}, {1,1}, {0,1} } - the square's corners; the two interior points are discarded
```

## Axis-aligned boxes

`sm::geometry::aabb_line_intersect (box, p0, p1)` tests whether the infinite line through `p0` and `p1` passes through a 3D axis-aligned box (an `sm::interval<sm::vec<T,3>>`, i.e. a `[min, max]` pair of corners):
```c++
sm::interval<sm::vec<float>> box = { {0,0,0}, {1,1,1} };
bool hits = sm::geometry::aabb_line_intersect (box, {0.5f,-2,0.5f}, {0.5f,2,0.5f}); // true
```

## Oriented bounding boxes

`sm::geometry::obb_collision_detect` can be used to detect a collision between two arbitrarily oriented cuboids.
It implements the separating axis test due to [Gottschalk](https://gamma.cs.unc.edu/SSV/obb.pdf).
Oriented bounding boxes are defined by a centre vector along with three orthogonal 'half-extent' vectors (from the origin to a face in each of the three axes of the cuboid).
`sm::geometry::obb_collision_detect<T>` expects to receive these vectors packed into two 3x4 matrices (`sm::mat<T, 3, 4>`); one for each box. col(0) of the matrix holds the centre vector, and the x-y-z half-extent vectors are in col(1), col(2) and col(3).
The function returns `true` if the boxes overlap and false otherwise.

```c++
// mathplot VisualModels have a function to return an oriented bounding box
sm::mat<float, 3, 4> obb1 = mathplot_visualmodel1->get_viewmatrix_obb();
sm::mat<float, 3, 4> obb2 = mathplot_visualmodel2->get_viewmatrix_obb();

bool overlapping = sm::geometry::obb_collision_detect (obb1, obb2);
```

## Rays, planes, discs, triangles and spheres

`sm::geometry::ray_plane_intersection (p0, n, l0, l)` returns the distance along `l` to a plane through `p0` with normal `n`, or `std::numeric_limits<T>::max()` if the ray is parallel to the plane (verified for both exactly- and almost-parallel cases):
```c++
float t = sm::geometry::ray_plane_intersection (p0, n, l0, l);
sm::vec<float, 3> hit_point = l0 + l * t;
```
`sm::geometry::ray_disc_intersection (p0, n, r, l0, l)` builds on this to test whether the ray hits a disc of radius `r` centred at `p0` on that plane.

`sm::geometry::vector_plane_projection (n, v)` projects a vector `v` onto the hyperplane whose normal is `n` (works for any dimension `N >= 2`, so it also projects a 2D vector onto a 1D line):
```c++
sm::vec<float> proj = sm::geometry::vector_plane_projection ({0,0,1}, {0,1,1}); // (0,1,0) - the xy-plane projection
```

`sm::geometry::tri_area (t0, t1, t2)` returns a 3D triangle's area (half the magnitude of its edges' cross product).

`sm::geometry::ray_tri_intersection<T, Ti, include_boundary, sided, debug_rti> (t0, t1, t2, l0, l)` tests a ray against a triangle, returning `std::tuple<bool, sm::vec<T,3>>` (whether it hit, and where):
```c++
auto [hit, p] = sm::geometry::ray_tri_intersection (t0, t1, t2, l0, l);
```
Its template parameters (all defaulted) are: `Ti`, the type used internally for the computation (defaults to `T`, but you can request higher precision); `include_boundary` (default `true`), whether a hit exactly on an edge or vertex counts; `sided` (default `true`), whether only hits on the triangle's 'front' face register - front is determined by the winding order of `t0, t1, t2`, so reversing two of the three points flips which side is considered front; and `debug_rti` (default `false`), which prints step-by-step diagnostics to `std::cout` when `true`.

`sm::geometry::ray_sphere_intersection (s0, s, l0, l)` finds where a ray meets a sphere of radius `s` centred at `s0`, returning up to two points as `sm::vec<sm::vec<T,3>, 2>` (each unfound point is `{T_max, T_max, T_max}`):
```c++
auto hits = sm::geometry::ray_sphere_intersection (s0, 1.0f, l0, l); // e.g. (-1,0,0) and (1,0,0) for a ray through a unit sphere at the origin
```
This uses its own `solve_quadratic (a, b, c)` helper internally. It returns a `sm::vec<F, 2>` of up to two real roots, filling in `std::numeric_limits<F>::max()` for any root that doesn't exist:
```c++
sm::vec<float, 2> roots = sm::geometry::solve_quadratic (1.0f, -5.0f, 6.0f); // (2, 3)
sm::vec<float, 2> none  = sm::geometry::solve_quadratic (1.0f, 0.0f, 1.0f);  // (F_max, F_max) - no real roots
```

## Distance to a line segment or triangle edge

`sm::geometry::dist_to_lineseg (v0, v1, p)` (and its squared form, `dist_to_lineseg_sq`) give the distance from `p` to the *nearest point on the segment* `v0`-`v1` - not to the infinite line through `v0` and `v1`, so the answer is clamped to one of the endpoints once `p` projects outside the segment:
```c++
float d = sm::geometry::dist_to_lineseg ({0,0,0}, {1,0,0}, {-0.1f,0,0}); // 0.1 - clamped to the nearest endpoint
```
`sm::geometry::dist_to_tri_edge`/`dist_to_tri_edge_sq` return the smallest of the three `dist_to_lineseg`/`_sq` distances to a triangle's three edges.

`sm::geometry::ray_point_intersection (p0, l0, l, close_enough = epsilon)` tests whether `p0` lies within `close_enough` of the segment from `l0` to `l0 + l` (see the [terminology note](#summary) above - this one really is bounded by `l`'s length, unlike the other 'ray' functions here).

## Spherical projections

`sm::geometry::spherical_projection` converts between 3D points on a sphere and several 2D map projections. It also declares an enum, `spherical_projection::type` (`mercator`, `equirectangular`, `cassini`), which may be used in client code (see for example, [healpixviewer](https://github.com/sebsjames/healpixviewer/blob/main/viewer.cpp#L201)).

```c++
sm::vec<float, 2> latlong = sm::geometry::spherical_projection::xyz_to_latlong (xyz); // {lat, long}, radians
sm::vec<float, 3> xyz2 = sm::geometry::spherical_projection::latlong_to_xyz (latlong, r_sph);
```
`xyz_to_latlong` assumes `xyz`'s length already *is* the sphere's radius; longitude `0` is along `+x`, and increases towards `+y`.

Three forward projections map `{lat, long}` (radians) onto a 2D plane, each accepting a central-meridian offset `lambda0`, and each wrapping the shifted longitude into `(-pi, pi]` for you:
```c++
sm::vec<float, 2> xy = sm::geometry::spherical_projection::mercator (latlong, r_sph, lambda0);
sm::vec<float, 2> xy2 = sm::geometry::spherical_projection::equirectangular (latlong, r_sph, lambda0, phi0, phi1); // + central-parallel offset phi0, true-scale latitude phi1
sm::vec<float, 2> xy3 = sm::geometry::spherical_projection::cassini (latlong, r_sph, lambda0);
```
Each has a matching `inverse_*` (back to `{lat, long}`) and `inverse_*_xyz` (straight back to a 3D point) function - `inverse_mercator`, `inverse_mercator_xyz`, `inverse_equirectangular`, `inverse_equirectangular_xyz`, `inverse_cassini`, `inverse_cassini_xyz`.

`mercator` also guards against the pole singularity: at latitude `-pi/2` exactly, its internal tangent calculation would be `0` (leading to `log(0)`), so it's substituted with `tan(pi/2)` instead.

*This page was authored with AI, based on human written code in geometry.cppm and reviewed by Seb James.*
