---
layout: page
title: sm::winder
parent: Reference
nav_order: 11
permalink: /ref/winder/
---
# sm::winder
{: .no_toc}
## Compute winding numbers
{: .no_toc}
```c++
import sm.winder;
```
Module file: [sm/winder.cppm](https://github.com/sebsjames/maths/blob/main/sm/winder.cppm).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::winder` computes the winding number of a 2D coordinate with respect to a
closed boundary described by a path of coordinates. The winding number tells
you how many times the boundary winds around the point and so `sm::winder`
can be used as a point-in-polygon test: a winding number of `0` means the
point is outside the boundary; any non-zero value means it's inside.

It implements the axis-crossing algorithm described in [Dan
Sunday's point-in-polygon
paper](https://www.engr.colostate.edu/~dga/documents/papers/point_in_polygon.pdf).

`sm::winder` is a class template, templated on the type of the container that
holds your boundary path:

```c++
template<typename C>
class winder
```

`C` can be more or less any copyable STL-like container of 2D
coordinates &mdash; `std::vector`, `std::list` or `std::array` (but not
`std::map`, which isn't ordered in the right way). The element type
(`C::value_type`) can be:

* `sm::vec<T, 2>`
* `sm::vvec<T>`
* `std::array<T, 2>` or `std::vector<T>`
* A type which provides `.x`/`.y` member attributes, or an
  `operator-` that can be used to vector-subtract one coordinate from another

Coordinate types that provide only `.x()`/`.y()` accessor methods, such as
`cv::Point`, or `first`/`second` members, such as `std::pair` are not supported.

## Example usage

Construct a `winder` with a reference to your boundary container, then call
`wind` with the coordinate you want to test:

```c++
#include <iostream>
#include <list>
import sm.winder;
import sm.vec;

int main()
{
    std::list<sm::vec<float, 2>> path = {
        {0.0f, 0.0f}, {1000.0f, 0.0f}, {1000.0f, 1000.0f}, {0.0f, 1000.0f}
    };
    sm::winder w (path);

    sm::vec<float, 2> pixel = { 500.0f, 500.0f };
    int winding_number = w.wind (pixel);
    std::cout << "Winding number = " << winding_number << std::endl; // 1: pixel is inside
}
```

Note that the `winder` constructor stores a *reference* to the boundary
container rather than a copy, so the container you pass in must outlive the
`winder` object. Once constructed, you can call `wind()` repeatedly with as
many different test coordinates as you like.

## Interpreting the result

`winder::wind` returns an `int`:

* `0` &mdash; the point lies outside the boundary
* non-zero &mdash; the point lies inside the boundary. For a simple boundary
  (one that doesn't self-intersect) traced once around, this will be `1` if
  the boundary points wind anticlockwise around the test point, or `-1` if
  they wind clockwise.

You don't need to explicitly close the boundary path by repeating its first
coordinate at the end; `wind()` always treats the segment from the last
coordinate back to the first as part of the boundary.
