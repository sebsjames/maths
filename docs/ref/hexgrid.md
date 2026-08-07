---
layout: page
title: sm::hexgrid
parent: Reference
nav_order: 10
permalink: /ref/hexgrid/
---
# sm::hexgrid
{: .no_toc}
## A hexagonal grid with an arbitrary boundary
{: .no_toc}
```c++
import sm.hexgrid;
```

Module file: [sm/hexgrid.cppm](https://github.com/sebsjames/maths/blob/main/sm/hexgrid.cppm). See also [sm/hexgrid_hdf.cppm](https://github.com/sebsjames/maths/blob/main/sm/hexgrid_hdf.cppm) for HDF5 save/load support, and [sm/hex.cppm](https://github.com/sebsjames/maths/blob/main/sm/hex.cppm) for the per-element `sm::hex` class. Example and test code:
[examples/hexgrid](https://github.com/sebsjames/maths/blob/main/examples/hexgrid.cpp)
[tests/bez2](https://github.com/sebsjames/maths/blob/main/tests/bez2.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::hexgrid` is a hexagonal-tiling counterpart to [`sm::grid`](/maths/ref/grid/): rather than using rectangular elements, it lays out a grid of hexagons (each an `sm::hex`) to manage spatial information for an associated computation.
It was designed for a study of [reaction-diffusion systems across two dimensional domains](https://elifesciences.org/articles/55588). `std::vector` arrays held the system state variables, and the spatial information for each element was managed in the hexgrid. Here, it was useful to use a hexagonal grid, because this made the [computation of the Laplacian easy](https://elifesciences.org/articles/55588#s4).

The design of `hexgrid` differs from that of `sm::grid`, being more similar to [`sm::cartgrid`](/maths/ref/cartgrid/). `sm::hexgrid` defines an initial hexagonal grid of hexagons which you can then clip to an arbitrary boundary in exactly the same spirit as `cartgrid` clips its rectangular lattice.
Where `cartgrid` and `sm::grid` share three boundary/wrap-related enums, `hexgrid` predates that enum-based design; its boundary shape is chosen by which `set_*_boundary` method you call, rather than by setting a `domain_shape` member. `hexgrid` was written before `cartgrid`, which predated `grid`.


`sm::hexgrid` always starts out as a  filled hexagon of hexagonal elements, built outwards ring-by-ring from a single centre hex until it reaches a requested diameter. As with `cartgrid`, you can leave the grid as this default hexagonal shape, or clip it down to an arbitrary boundary; a circle, ellipse, rectangle, parallelogram, or a boundary you supply yourself as a closed [`sm::bezcurvepath`](https://github.com/sebsjames/maths/blob/main/sm/bezcurvepath.cppm) or list of points. Clipping discards every hex outside the boundary and re-links the neighbour relationships of what remains.

`sm::hexgrid` is a non-templated class with coordinates of type `float` and indices of type `std::uint32_t` (sometimes cast to `std::int32_t`). It does not derive from, or share any types with, `sm::grid`; it does, however, share a very similar design and method-naming convention with `sm::cartgrid`. `hexgrid` was designed first, then `cartgrid` was coded up using the same ideas. (The `set_boundary`/`set_boundary_only`/`set_boundary_on_outer_edge`/`get_region` family, and the flat `d_*` cache-vector convention, are essentially the same functions applied to hexes instead of rects.)

Defined as:
```c++
export namespace sm
{
    class alignas(8) hexgrid
    {
        // ...
        std::list<hex> hexen;
```

### Hex coordinates

Each `sm::hex` (defined in `sm/hex.cppm`, and re-exported by `sm.hexgrid`, so `import sm.hexgrid;` is enough to use it) stores an axial coordinate `{ri, gi, bi}` alongside its Cartesian `{x, y, z}` position, a 32-bit `flags` word (`HEX_IS_BOUNDARY`, `HEX_INSIDE_BOUNDARY`, `HEX_INSIDE_DOMAIN`, `HEX_IS_REGION_BOUNDARY`, `HEX_INSIDE_REGION`, plus 16 bits reserved for your own use as `HEX_USER_FLAG_0`..`HEX_USER_FLAG_15`), and six neighbour iterators (`ne`, `nne`, `nnw`, `nw`, `nsw`, `nse`; see [Neighbours](#neighbours-in-the-six-hex-directions)). The hexes are 'point-up', spaced `d` apart within a row and `v = d * sqrt(3)/2` apart between rows.

## Create a hexgrid

```c++
sm::hexgrid hg (0.01f, 3.0f, 0.0f); // d (hex spacing), x_span (diameter), z (layer)
```
This builds a full hexagon of hexes with hex-to-hex spacing `d` and a horizontal diameter of approximately `x_span`. `init (d_, x_span_, z_)` re-runs the same construction on an existing `hexgrid`, and the default constructor `hexgrid()` leaves `d = x_span = 1.0f` but, like `cartgrid`'s default constructor, does not build the grid for you.

## Setting a boundary

The convenience methods compute the boundary points for a given shape and clip the grid to them in one call:
```c++
sm::hexgrid hg (0.01f, 3.0f, 0.0f);
hg.set_circular_boundary (0.6f);         // radius 0.6, centred at the origin by default
std::cout << "Number of hexes in grid: " << hg.num() << std::endl;
```
`set_elliptical_boundary`, `set_rectangular_boundary` and `set_parallelogram_boundary` work the same way for their respective shapes. For an arbitrary shape, supply a closed Bezier path:
```c++
sm::bezcurvepath<float, 3> bound = /* ... four curve segments forming a closed loop ... */;
auto hgrid = std::make_unique<sm::hexgrid> (0.02f, 4.0f, 0.0f);
hgrid->set_boundary (bound);
std::cout << "Number of hexes is: " << hgrid->num() << std::endl;
```
You can also pass a raw `std::vector<sm::bezcoord<float>>` of boundary points, or a `std::list<hex>` whose positions are matched against the grid's own hexes. Every one of these throws `std::runtime_error` if the resulting boundary doesn't form a single contiguous ring of hexes.

If you'd rather use the grid's natural hexagonal outline as its own boundary (so that `get_boundary()`, `compute_distance_to_boundary()` etc. become meaningful without clipping to anything smaller), use:
```c++
hg.set_boundary_on_outer_edge();
```
And if you just want to *mark* a boundary for inspection without discarding any hexes, use one of the `set_boundary_only` overloads instead of `set_boundary`.

`get_boundary()` returns a copy of the current boundary hexes, and `compute_distance_to_boundary()` fills each hex's `dist_to_boundary` (`0` on the boundary itself, `-100.0f` for any hex outside the boundary, otherwise the distance to the nearest boundary hex).

### Temporary regions

As with `cartgrid`, `get_region` (given a Bezier path or point vector) and `get_hexagonal_region` (given a centre hex index and radius) let you mark and retrieve a sub-set of hexes without discarding anything from the grid; useful for querying, e.g., "which hexes fall within this circle" while leaving the grid itself untouched:
```c++
sm::vec<float, 2> region_centroid;
auto region = hg.get_region (path, region_centroid);   // arbitrary sub-path
auto disc = hg.get_hexagonal_region (centre_vi, 0.3f); // hexes within ~0.3 units of hex `centre_vi`
hg.clear_region_boundary_flags();                      // clear the temporary region flags afterwards
```
Unlike the main boundary-setting methods, `get_region` does not throw on a non-contiguous region; it simply returns an empty result.

## Coordinates and indices

`num()` returns the number of hexes currently in the grid, and `find_hex_nearest` / `find_hex_at` look up a hex by Cartesian position or by axial coordinate, respectively (both can return an invalid, non-dereferenceable iterator; `hexen.end()`; if the query is off-grid, so check before dereferencing):
```c++
auto nearest = hg.find_hex_nearest ({ 0.1f, 0.2f });
auto at_axial = hg.find_hex_at ({ 2, -1, 0 }); // {ri, gi, bi}
```
`output()` renders every hex, grouped by ring, as a human-readable string, and `extent()` prints the Cartesian corners of the *original* hexagon (this becomes meaningless once the grid has been clipped down, at which point `extent()` says so instead).

## Neighbours in the six hex directions

Each hex has up to six neighbours; East, North-East, North-West, West, South-West and South-East; reachable either through the hex object's own iterators, or via a flat domain index:
```c++
auto hi = hg.hexen.begin();
if (hi->has_ne()) {
    auto east_neighbour = hi->ne; // std::list<hex>::iterator
}
// or, given a flat domain index `di` (hex::di):
if (hg.has_ne (di)) {
    std::int32_t east_di = hg.ne (di); // -1 if there's no such neighbour
}
```
**Note:** the domain-index `has_ne`/`has_nw`/`has_nne`/`has_nnw`/`has_nse`/`has_nsw` functions return `std::int32_t`, not `bool`, even though they behave as a boolean presence check (they evaluate to `0` or `1`).

## Wrapping

There's no general wrap enum for `hexgrid`; the only wrapping support is `set_parallelogram_wrap (bool on_r, bool on_g)`, which re-wires the neighbour links at the edges of a parallelogram-shaped domain to point at the opposite edge. **At present it only supports wrapping both axes together**; it throws `std::runtime_error` unless both `on_r` and `on_g` are `true`.

## Convolution, resampling and shifting data

`convolve` performs a 2D convolution of per-hex data against a kernel defined on a second `hexgrid` (which must share the same `d`), walking neighbour links rather than assuming a fixed array stride, so it works correctly on boundary-clipped domains. `resample_image` Gaussian-resamples a rectangular pixel image onto the hex centres, much like the equivalent methods in `sm::grid` and `sm::cartgrid`.

`shiftdata` translates per-hex data by an arbitrary Cartesian vector, splitting the shift into whole hex-hops (following neighbour links, so any wrapping you've set up is respected) plus a sub-hex remainder distributed by exact hex-overlap-area weighting:
```c++
sm::vvec<float> image_data (hg.num(), 0.0f);
bool ok = hg.shiftdata (image_data, sm::vec<float, 2>{ 0.003f, -0.001f });
```
It returns `false` (leaving `image_data` unmodified) if the overlap geometry couldn't be resolved for the given shift. The `compute_hex_overlap`/`compute_overlap_*`/`setup_hexoverlap_geometry` methods it relies on are public, but are internal machinery for `shiftdata`; you shouldn't normally need to call them directly.

## Geometry

```c++
float w = hg.width();           // Cartesian width of the boundary
float dp = hg.depth();          // Cartesian depth of the boundary
float d_ = hg.get_d();          // hex-to-hex spacing
float v_ = hg.get_v();          // row-to-row spacing, d * sqrt(3)/2
float sr = hg.get_sr();         // 'short radius', d/2
float lr = hg.get_lr();         // 'long radius' (centre to vertex), d/sqrt(3)
float area = hg.get_hex_area(); // area of one hex
```
`get_x_min(phi)`/`get_x_max(phi)` give the extent of the grid along an axis rotated by angle `phi` (default `0`, i.e. along x).

## Saving and loading

HDF5 persistence lives in a separate module, `sm.hexgrid.hdf` (which re-exports `sm.hexgrid`, so importing it gives you everything above too):
```c++
import sm.hexgrid.hdf;

sm::hexgrid_save (hg, "myhexgrid.h5");

sm::hexgrid hg2;
sm::hexgrid_load (hg2, "myhexgrid.h5");
```
Loading reconstructs each hex's six neighbour relationships by matching saved indices against the freshly-loaded hex list; an O(n²) operation for large grids; and throws `std::runtime_error` if any expected neighbour can't be matched. The boundary curve itself (as a `bezcurvepath`) is not saved; only the resulting hex positions, flags and neighbour relationships are.

*This page was authored with AI, based on human written code in hexgrid.cppm and reviewed by Seb James.*
