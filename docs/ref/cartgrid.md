---
layout: page
title: sm::cartgrid
parent: Reference
nav_order: 9
permalink: /ref/cartgrid/
---
# sm::cartgrid
{: .no_toc}
## A rectangular Cartesian grid that can be clipped to an arbitrary boundary
{: .no_toc}
```c++
import sm.cartgrid;
```

Module file: [sm/cartgrid.cppm](https://github.com/sebsjames/maths/blob/main/sm/cartgrid.cppm). Test code:
[tests/cartgrid1](https://github.com/sebsjames/maths/blob/main/tests/cartgrid1.cpp)
[tests/cartgrid_gridshiftcoords](https://github.com/sebsjames/maths/blob/main/tests/cartgrid_gridshiftcoords.cpp)
[tests/cartgrid_shiftindicesbymetric](https://github.com/sebsjames/maths/blob/main/tests/cartgrid_shiftindicesbymetric.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::cartgrid` is a Cartesian grid of [`sm::rect`](https://github.com/sebsjames/maths/blob/main/sm/rect.cppm) elements that, unlike [`sm::grid`](/maths/ref/grid/), can be clipped to an arbitrary domain boundary (a closed curve, rather than just a rectangle). If you plan to use only rectangular boundaries, prefer the simpler, templated (and more recent) `sm::grid` instead; `cartgrid` exists specifically for the case where you need to use an irregularly shaped domain of rectangular elements.

An `sm::cartgrid` always starts out as a plain rectangular lattice of `sm::rect` elements, each of which knows its own Cartesian position, its integer row/column indices, and (once neighbours have been wired up) iterators to up to eight neighbouring rects (E, NE, N, NW, W, SW, S, SE). You can use the grid exactly as-is (a plain rectangle), or clip it down to an arbitrary shape by supplying a boundary — a closed [`sm::bezcurvepath`](https://github.com/sebsjames/maths/blob/main/sm/bezcurvepath.cppm) or a raw list of boundary points. Clipping discards every `rect` outside the boundary and re-links the neighbour relationships of what remains, so the grid you're left with contains only the elements inside your chosen shape.

Unlike `sm::grid`, `sm::cartgrid` is not a template; coordinates are always `float`, and indices are always `std::uint32_t` (though they may be cast to signed 32 bit integers in some functions). It also does not derive from `sm::grid`; the two classes only share three enum types (`griddomainshape`, `griddomainwrap` and `gridorder`, all defined in [`sm/grid.cppm`](https://github.com/sebsjames/maths/blob/main/sm/grid.cppm) and re-exported by `sm.cartgrid`). Every method in `cartgrid` is its own, independent implementation, built around the boundary-clipping linked-list-of-rects model rather than `sm::grid`'s dense arithmetic indexing. (`gridorder` is re-exported but not actually used anywhere in `cartgrid`.)

Defined as:
```c++
export namespace sm
{
    class alignas(8) cartgrid
    {
        // ...
        std::list<rect> rects;
```

## Create a cartgrid

`cartgrid` contains a bit of a jumble of constructors.
This is due to the fact that, since `sm::grid` was created, use of `cartgrid` is now rare, so there hasn't been motivation to continue improving the design of the class.

The most convenient constructors build a **symmetric**, zero-centred rectangle of square or rectangular elements:
```c++
sm::cartgrid cg (2.0f, 8.0f);              // square elements, d = 2, spanning 8 units in both x and y
sm::cartgrid cg2 (1.0f, 1.0f, 4.0f, 4.0f);  // dx, dy, x-span, y-span
```
Both throw `std::runtime_error` if the spacing doesn't divide evenly into a zero-centred grid (i.e. if `x_span / d` isn't an even integer).

If you need an asymmetric box, or want to set the [wrapping](#wrapping) behaviour, use the six-argument constructor, which specifies opposite corners `(x1,y1)` and `(x2,y2)` directly:
```c++
sm::cartgrid cg3 (0.05f, 0.05f, 0.0f, 0.0f, 0.2f, 0.2f, 0.0f,
                   sm::griddomainshape::rectangle, sm::griddomainwrap::horizontal);
```
This is the only constructor that accepts a `griddomainwrap` — the symmetric constructors always leave wrapping at its default, `griddomainwrap::none`.

The default constructor sets the spacing/span member variables but does **not** build the grid — you must follow it with `init()`:
```c++
sm::cartgrid cg4;
cg4.init (1.0f, 10.0f); // d, x_span — builds a symmetric square-element grid
```

## Setting an arbitrary boundary

**Before** calling any of the boundary-setting methods below (other than `set_boundary_only`), set `domain_shape` to `sm::griddomainshape::boundary` — the default is `griddomainshape::rectangle`, and calling `set_boundary` while `domain_shape` is still `rectangle` throws `std::runtime_error`:
```c++
sm::cartgrid cg (0.1f, 4.0f);
cg.domain_shape = sm::griddomainshape::boundary;
cg.set_circular_boundary (1.5f); // discards every rect further than 1.5 units from the origin
```
`set_circular_boundary` and `set_elliptical_boundary` are convenience wrappers that compute boundary points on a circle or ellipse and pass them to `set_boundary`. You can also supply your own boundary as a closed Bezier path, or as a raw vector of points:
```c++
sm::bezcurvepath<float> path = /* ... a closed curve ... */;
cg.set_boundary (path); // samples the path, marks the nearest rects, then clips

std::vector<sm::bezcoord<float>> points = /* ... */;
cg.set_boundary (points);
```
Whichever overload you use, `set_boundary` marks the rects nearest to the boundary points, checks that they form a single contiguous ring (throwing `std::runtime_error` if not), and then discards every rect outside that ring, re-numbering the survivors and rebuilding the internal coordinate caches.

If you don't have an arbitrary shape and simply want the *whole* rectangle to count as its own boundary (for example, so that `get_boundary()` or `compute_distance_to_boundary()` become meaningful), use:
```c++
sm::cartgrid cg (2.0f, 8.0f);
cg.domain_shape = sm::griddomainshape::boundary;
cg.set_boundary_on_outer_edge(); // marks the outermost ring of the rectangle as the boundary
```
This is the pattern used throughout the test suite to finalize a plain rectangular `cartgrid`.

If you want to mark a boundary purely for inspection, *without* discarding any rects, use `set_boundary_only` (with a Bezier path or a point vector) instead of `set_boundary`.

`get_boundary()` returns a copy of the current boundary rects, and `compute_distance_to_boundary()` computes, for every rect, its distance to the nearest boundary rect (`0` for boundary rects themselves, and a sentinel of `-100.0f` for any rect outside the boundary — this is a different sentinel convention from `sm::grid`, which uses `std::numeric_limits::max()`).

### Temporary regions

Separately from the (destructive) boundary machinery, `get_region` lets you mark and retrieve a *temporary* sub-region defined by its own closed path, without discarding anything from the grid:
```c++
sm::vec<float, 2> region_centroid;
std::vector<std::list<sm::rect>::iterator> region = cg.get_region (path, region_centroid);
// ... use region ...
cg.clear_region_boundary_flags(); // clear the temporary region flags when you're done
```

## Coordinates, indices and shifting

`get_coordinates3()` and `get_coordinates2()` return every element's position as a `std::vector` of `sm::vec<float, 3>` or `sm::vec<float, 2>`; `get_coords()` and `get_coords_2d()` are equivalent but return an `sm::vvec`. **Note:** `get_coords()` always sets the z component to `0`, whereas `get_coordinates3()` uses the grid's actual `z` — prefer `get_coordinates3()` if you need a real z value.

For rectangular (non-boundary-clipped) grids, `index_from_coord` converts a metric coordinate to a flat index:
```c++
sm::cartgrid cg (1.0f, 10.0f);
std::int32_t idx = cg.index_from_coord ({ 2.0f, 3.0f });
```

`shift_coords` and `shift_indices_by_metric` — again, for rectangular grids only — shift a set of coordinates or flat indices by (the nearest whole number of grid-steps to) a metric offset. Any point that would fall outside the grid is silently dropped from the result, rather than being replaced with a sentinel:
```c++
sm::cartgrid cg (1.0f, 1.0f, 4.0f, 4.0f); // 25 elements, spanning -2..2 in x and y
sm::vvec<sm::vec<float, 2>> orig = { {1, 0}, {2, 0}, {1, -1}, {2, -1} };

auto shifted = cg.shift_coords (orig, -2, 1);
// shifted == { {-1, 1}, {0, 1}, {-1, 0}, {0, 0} }

auto shifted2 = cg.shift_coords (orig, 1, 2);
// shifted2 == { {2, 2}, {2, 1} } -- the other two points fell off the grid and were dropped
```
`shift_indices_by_metric` works the same way but on flat `d_*`-array indices rather than coordinates.

## Neighbours and the flat cache arrays

Each `sm::rect` in `cg.rects` carries iterators to up to eight neighbours (`ne`, `nne`, `nn`, `nnw`, `nw`, `nsw`, `ns`, `nse`), each with a matching `has_*()` presence check — the same pattern as `sm::hex`'s six-way neighbours. The simplest way to inspect a grid is to iterate `rects` directly:
```c++
for (const auto& r : cg.rects) {
    std::cout << r.output_cart() << std::endl; // or check r.has_ne(), r.ne->x, etc.
}
```
For fast, array-indexed access, `cartgrid` also maintains a set of flat cache vectors — `d_x`, `d_y`, `d_xi`, `d_yi`, `d_flags`, `d_dist_to_boundary`, and the eight `d_ne`/`d_nne`/`d_nn`/`d_nnw`/`d_nw`/`d_nsw`/`d_ns`/`d_nse` neighbour-index vectors (each holding `-1` where there is no such neighbour) — kept in sync with `rects` by `populate_d_vectors()`, which is called automatically whenever the boundary changes. You shouldn't normally need to call it yourself.

## Filtering, convolution and resampling

`oncentre_offsurround` and `boxfilter` both apply a spatial filter to a `std::vector`/`sm::vvec` of per-element data, walking the rect neighbour links rather than assuming a fixed array stride. `boxfilter_f` is a fast version of `boxfilter` which requires a rectangular, horizontally-wrapped grid (enabling the speed-up).
```c++
sm::vvec<float> vals (cg3.num(), 0.0f);
sm::vvec<float> filtered (cg3.num(), 0.0f);
cg3.boxfilter_f<float, 3, false> (vals, filtered); // 3x3 box filter; requires domain_wrap == horizontal
```
`convolve` performs a full 2D convolution of a data array against a kernel defined on a second `cartgrid` (which must share the same element spacing, `d`), and `resample_to_polar` resamples a rectangular image onto a polar `(r, φ)` grid, with an optional logarithmic radial scale (`sm::scaling_function`, from `sm.scale` — remember to `import sm.scale;` yourself if you want to name it, since `cartgrid` doesn't re-export it).

## Extents and geometry

As with `sm::grid`, geometric queries distinguish the boundary-rect bounding box from the original backing rectangle:
```c++
float w = cg.width();         // width of the *boundary* (from find_boundary_extents())
float d = cg.depth();         // depth of the boundary
sm::vec<float, 4> ext = cg.get_extents(); // {xmin, xmax, ymin, ymax} of the boundary
std::int32_t nw = cg.widthnum();          // element count across the boundary's width
```
`get_d()`, `get_v()` and `get_span()` return the element spacing and span; `get_sr()`/`get_lr()` return the 'short' and 'long' element radii; `get_rect_area()` returns the area of one element; and `centre_of_mass` computes the data-weighted centroid over all elements.

**Note:** `x_minmax` and `y_minmax` (public members) always describe the extents of the *original, unclipped* rectangular grid, not the smaller region left after `set_boundary` — they aren't updated by clipping. `shift_coords` and `is_inside_rectangular_boundary`-style checks test against this original rectangle, so they're only meaningful in the sense of "is this within the backing rectangle", not "is this within my clipped boundary shape".

## Wrapping

`domain_wrap` (a `griddomainwrap`, shared with `sm::grid` — see [its docs](/maths/ref/grid/#wrapping)) can only be set via the six-argument, asymmetric constructor. It's most often used to satisfy `boxfilter_f`, which requires `domain_wrap == griddomainwrap::horizontal` and `domain_shape == griddomainshape::rectangle`, throwing `std::runtime_error` otherwise.

## Saving and loading

Old `save()` and `load()` methods are disabled.
These are awaiting a treatment as used for `sm::hexgrid`, where an additional C++ module called `hexgrid_hdf` provides the functions to save and load hexgrid data to an HDF5 file.
This approach avoids the need for a link to the HDF5 library unless loading and saving is required.

*This page was authored with AI, based on human written code in cartgrid.cppm and reviewd by Seb James*
