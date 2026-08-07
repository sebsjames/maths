---
layout: page
title: sm::grid
parent: Reference
nav_order: 8
permalink: /ref/grid/
---
# sm::grid
{: .no_toc}
## A rectangular Cartesian grid for 2D computations
{: .no_toc}
```c++
import sm.grid;
```

A Cartesian grid class. `sm::grid` is a simpler version of [`sm::cartgrid`](https://github.com/sebsjames/maths/blob/main/sm/cartgrid.cppm). The difference is that any `sm::grid` is rectangular, whereas a `sm::cartgrid` may be constructed with an arbitrary domain boundary (`sm::hexgrid` objects can also have an arbitrary boundary). Unless you need non-rectangular boundaries for your Cartesian grids, prefer `sm::grid` over `sm::cartgrid`.

Module file: [sm/grid.cppm](https://github.com/sebsjames/maths/blob/main/sm/grid.cppm). Test and example code:
[tests/grid1](https://github.com/sebsjames/maths/blob/main/tests/grid1.cpp)
[tests/grid_rowcol1](https://github.com/sebsjames/maths/blob/main/tests/grid_rowcol1.cpp)
[tests/grid_neighbours1](https://github.com/sebsjames/maths/blob/main/tests/grid_neighbours1.cpp)
[tests/grid_indexlookup1](https://github.com/sebsjames/maths/blob/main/tests/grid_indexlookup1.cpp)
[tests/grid_shiftindex1](https://github.com/sebsjames/maths/blob/main/tests/grid_shiftindex1.cpp)
[tests/grid_getabscissae1](https://github.com/sebsjames/maths/blob/main/tests/grid_getabscissae1.cpp)
[tests/grid_suggest_dims1](https://github.com/sebsjames/maths/blob/main/tests/grid_suggest_dims1.cpp)
[tests/grid_profile1](https://github.com/sebsjames/maths/blob/main/tests/grid_profile1.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::grid` provides coordinates for each element of a rectangular grid, along with neighbour relationships between the elements.
The idea is that while running a computation over some kind of spatial field (with your state held in arrays, `sm::vec`, `sm::vvec` or similar), you can ask the grid for the coordinate of element `i`, and for the index (or coordinate, or existence) of the neighbour to the 'East', 'West', 'North' or 'South' of `i`.
The coordinates for each index `i` are pre-computed and cached in the member attribute `sm::grid::v_c`. If I recall correctly, this was found to provide better performance than computing a coordinate for each lookup call.

You can specify, and change at runtime (at the cost of re-computing `sm::grid::v_c`), the width and height of the grid (in numbers of elements), the inter-element spacing and an offset for element 0.
You can also choose whether the grid wraps at its edges and which order its indices run in.
Because the parameters can be changed after construction, `sm::grid` can be used to model an expanding or shifting domain — for example, changing `dx` over time to model an expanding grid.

Defined as:
```c++
export namespace sm
{
    template<typename I = std::uint32_t, typename C = float>
    struct grid
    {
        // ...
```
`I` is the *index* type, used for indexing (and counting) elements in the grid. `C` is the *coordinate* type, used for the metric coordinates of the grid elements. `I` must be an integer type and `C` must be a signed type (normally a floating point type, but a signed integer type would also work).

## Design

### Grid order

There are four ways in which the elements of a grid can be numbered, given by the enum class `sm::gridorder`:

```c++
enum class gridorder
{
    bottomleft_to_topright,
    topleft_to_bottomright,
    bottomleft_to_topright_colmaj,
    topleft_to_bottomright_colmaj
};
```

For a grid of width 4 and height 2, the four orderings number the elements like this:

```
bottomleft_to_topright:          topleft_to_bottomright:

4 5 6 7                           0 1 2 3
0 1 2 3                           4 5 6 7

bottomleft_to_topright_colmaj:   topleft_to_bottomright_colmaj:

1 3 5 7                           0 2 4 6
0 2 4 6                           1 3 5 7
```

`bottomleft_to_topright` is the default. You can query the order of a grid at any time with `get_order()`, and you can find out whether the current order is row-major (as opposed to column-major) with `rowmaj()`.

### Wrapping

The `griddomainwrap` enum class determines whether the grid wraps at its edges, and if so, in which direction:

```c++
enum class griddomainwrap
{
    none,        // No wrapping
    horizontal,  // The eastern neighbour of the most eastern element is the most western element on that row
    vertical,    // The northern neighbour of the most northern element is the most southern element on that col
    both
};
```

Wrapping only affects neighbour relationships (see [Neighbours](#neighbours), below); it has no effect on the coordinates that are computed for each element.

There is a third enum, `griddomainshape`, distinguishing a `rectangle` from an arbitrary `boundary`. `sm::grid` is always rectangular, so this enum exists purely for use by `sm::cartgrid`, which derives its domain-shape handling from the same header.

## Create a grid

The default constructor makes a grid of a single element; call `init()` (or one of the setters, each of which calls `init()` for you) before using it:
```c++
sm::grid<int, float> g;
g.set_w (10);
g.set_h (10);
```

More usually, you construct the grid with its dimensions and, optionally, spacing, offset, wrapping and order:
```c++
sm::vec<float, 2> dx = { 1.0f, 1.0f };       // inter-element spacing (horizontal, vertical)
sm::vec<float, 2> offset = { 0.0f, 0.0f };   // coordinate of element 0

sm::grid<int, float> g (10, 12, dx, offset, sm::griddomainwrap::none, sm::gridorder::bottomleft_to_topright);
```
`dx`, `offset`, `wrap` and `order` all have sensible defaults ({1,1}, {0,0}, `griddomainwrap::none` and `gridorder::bottomleft_to_topright` respectively), so you can omit any of the trailing arguments:
```c++
sm::grid<std::uint32_t, double> g2 (10, 12, sm::vec<double, 2>{ 0.2, 0.2 });
```

You can also set most of the parameters together, after construction, with `set_grid_params`:
```c++
sm::vec<int, 2> dims = { 20, 15 };
sm::vec<float, 2> spacing = { 0.5f, 0.5f };
sm::vec<float, 2> grid_offset = { -5.0f, -3.75f };
g.set_grid_params (dims, spacing, grid_offset);
```
There are also individual setters `set_w`, `set_h`, `set_dx` and `set_offset`, each of which re-runs `init()` for you so that the memorized coordinates stay in sync. There are no setters for `wrap` or `order`; these are not expected to change at runtime. If you need them to change, you can construct a new `sm::grid`.

`init()` checks (with `static_assert`) that `I` is an integer type and `C` is a signed type, and throws a `std::runtime_error` at runtime if `w` or `h` is negative, or if `w * h` would overflow `I`. Choose `I` with enough range for the number of elements you need.

## Coordinates and indices

Once a grid is constructed, `operator[]` (or, equivalently, `coord_lookup`) gives you the metric coordinate of an element by index:
```c++
sm::grid<int, float> g (10, 10);
sm::vec<float, 2> c = g[42];             // same as g.coord_lookup (42)
```
If `index` is out of range, both return `{C_max, C_max}` (`std::numeric_limits<C>::max()` in each component) rather than throwing.

`coord` computes a coordinate directly from an index (this is what populates the grid's coordinate cache, `v_c`, during `init()`), and `polar_lookup` returns the same location in polar form, `{radius, angle}`:
```c++
sm::vec<float, 2> polar = g.polar_lookup (42); // {length, angle} of g[42]
```

You can also look up an index from a coordinate. `index_lookup` finds the index of the element whose coordinate matches a given location, throwing `std::runtime_error` if the location doesn't lie on the grid:
```c++
try {
    int idx = g.index_lookup ({ 3.0f, 4.0f });
} catch (const std::exception& e) {
    std::cout << "Off grid: " << e.what() << std::endl;
}
```

`row (index)` and `col (index)` return the row and column of a given index, respecting the grid's `order`:
```c++
sm::grid<int, float> g (4, 2);
int r = g.row (5); // 1
int c = g.col (5); // 1
```

## Neighbours

For any index, you can ask for the index or coordinate of its East, West, North or South neighbour, and whether that neighbour exists:
```c++
sm::grid<int, float> g (4, 2);

if (g.has_ne (5)) {
    int east_idx = g.index_ne (5);
    sm::vec<float, 2> east_coord = g.coord_ne (5);
}
```
If a neighbour doesn't exist (because the element is on an edge of the grid and `wrap` doesn't cover that edge), `index_ne`/`index_nw`/`index_nn`/`index_ns` return `std::numeric_limits<I>::max()` and the corresponding `coord_*` function returns `{C_max, C_max}`.

The diagonal neighbours North-East, North-West, South-East and South-West are also available, built from pairs of the four cardinal directions:
```c++
bool ok = g.has_nne (5);
int idx = g.index_nne (5);
sm::vec<float, 2> coord = g.coord_nne (5);
// and similarly has_nnw/index_nnw/coord_nnw, has_nse/index_nse/coord_nse, has_nsw/index_nsw/coord_nsw
```

To collect the North, East, South and West neighbours of a whole set of indices in one go, use `find_nearest_neighbours`:
```c++
sm::vvec<int> sources = { 0, 1, 2 };
sm::vvec<int> neighbours;
g.find_nearest_neighbours (sources, neighbours);
```

## Shifting by an offset in elements

As well as neighbour lookups (a shift of one element), you can compute the result of shifting an index by an arbitrary number of elements in x, y or both. `col_after_x_shift` and `row_after_y_shift` return the new column or row (or `std::numeric_limits<I>::max()` if the shift takes you off the grid and `wrap` doesn't cover that direction):
```c++
sm::grid<int, float> g (5, 4);
int new_col = g.col_after_x_shift (7, 2);  // column of element 7, shifted 2 elements east
int new_row = g.row_after_y_shift (7, -1); // row of element 7, shifted 1 element south
```
`shift_index` combines both into the resulting index directly:
```c++
sm::vec<int, 2> delta = { 2, 2 };
int new_index = g.shift_index (7, delta);
```

## Extents and geometry

`sm::grid` distinguishes between the distance spanned by element *centres* and the distance spanned if you draw the grid as a set of solid pixels (which extends half an inter-element spacing beyond the outermost centres on each side):
```c++
sm::grid<int, float> g (10, 8, { 0.1f, 0.1f });

float w = g.width();             // centre-to-centre width  = dx[0] * (w - 1)
float w_px = g.width_of_pixels(); // pixel width            = dx[0] * w
float h = g.height();
float h_px = g.height_of_pixels();
float a = g.area();
float a_px = g.area_of_pixels();
```
`xmin`, `xmax`, `ymin` and `ymax` give the coordinate extents of the grid, `extents()` bundles all four into a `sm::vec<C, 4>` as `{xmin, xmax, ymin, ymax}`, and `centre()` returns the coordinate of the middle of the grid.

`get_abscissae()` and `get_ordinates()` return the set of x coordinates for a row and y coordinates for a column, respectively, as an `sm::vvec<C>`:
```c++
sm::vvec<float> abscissae = g.get_abscissae(); // size == g.get_w()
sm::vvec<float> ordinates = g.get_ordinates(); // size == g.get_h()
```

## Choosing dimensions for N elements

If you need a grid with (approximately) a given number of elements, the static method `suggest_dims` finds a suitable width and height:
```c++
sm::vec<int, 2> dims = sm::grid<int, float>::suggest_dims (24);
// dims == {6, 4} (or another pair of factors of 24, whichever is closest to square)
```
By default, `suggest_dims` will only return dimensions whose product exactly equals `num_elements`; if `num_elements` has no useful factors, it returns `{I_max, I_max}` to signal failure. Pass `true` as the second argument to allow the function to search upwards for a nearby number of elements that *does* factorise nicely:
```c++
sm::vec<int, 2> dims2 = sm::grid<int, float>::suggest_dims (23, true); // 23 is prime; dims2.product() > 23
```

## Other utilities

`indices_in_radius` fills a `sm::vvec<I>` with the indices of every element within a given radius of a metric location, expanding outwards ring-by-ring from the nearest element:
```c++
sm::vvec<int> inds;
g.indices_in_radius ({ 0.0f, 0.0f }, 2.5f, inds);
```

`resample_image` resamples a monochrome image (supplied as a flat `sm::vvec<float>` of pixel values, assumed to run bottom-left to top-right and to be one unit wide) onto the grid's own elements, using a Gaussian kernel. It requires the grid's `order` to be `gridorder::bottomleft_to_topright`:
```c++
sm::vvec<float> image_data = /* ... */;
std::uint32_t image_pixelwidth = 64;
sm::vec<float, 2> image_scale = { 1.0f, 1.0f };
sm::vec<float, 2> image_offset = { 0.0f, 0.0f };
sm::vvec<float> resampled = g.resample_image (image_data, image_pixelwidth, image_scale, image_offset);
```

Finally, `str()` renders the grid's indices and coordinates as a human-readable string, useful for debugging:
```c++
std::cout << g.str() << std::endl;
```

*This page was authored with AI, based on human written code in grid.cppm, and reviewed by Seb James*