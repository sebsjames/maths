---
layout: page
title: sm::bezcoord
parent: Reference
nav_order: 25
permalink: /ref/bezcoord/
---
# sm::bezcoord
{: .no_toc}
## A point on a Bezier curve, with its curve parameter
{: .no_toc}
```c++
import sm.bezcoord;
```

Module file: [sm/bezcoord.cppm](https://github.com/sebsjames/maths/blob/main/sm/bezcoord.cppm).

**Table of Contents**

- TOC
{:toc}

## Summary

An `sm::bezcoord<F>` bundles a 2D point (`bezcoord::coord`) on a Bezier curve together with the curve parameter `t` (in `[0, 1]`) that produced it (`bezcoord::param`). The distance remaining to the end of the `sm::bezcurve` to which the bezcoord may belong can be recorded in `bezcoord::remaining`.

It also has the flag `bezcoord::null_coordinate`. A 'null' bezcoord can hold a value in `bezcoord::remaining`,
but any value in `bezcoord::coord` or `bezcoord::param` should be ignored. A bezcoord with
`null_coordinate` set `true` is used to indicate the remaining distance to the
end of an `sm::bezcurve`, and is important when joining curves with
`sm::bezcurvepath` (see the bezcurve section [Evaluating the curve](/maths/ref/bezcurve/#evaluating-the-curve)).

`bezcoord` is mostly encountered as a *return type* from [`sm::bezcurve`](/maths/ref/bezcurve/) and [`sm::bezcurvepath`](/maths/ref/bezcurvepath/) methods rather than something you construct yourself, but it's a small, self-contained struct you can use on its own too.

Following the SVG convention, `coord[0]` (x) is positive rightwards and `coord[1]` (y) is positive *downwards*; use `invert_y()` if you need to flip into a right-handed plotting convention.

## Creating a bezcoord

```c++
sm::bezcoord<float> a;                                // coord = {0,0}, param = -1 ('unset'), not null
sm::bezcoord<float> nullcoord (true);                 // an explicitly null coordinate
sm::bezcoord<float> b (sm::vec<float,2>{3.0f, 4.0f}); // just a position
sm::bezcoord<float> c (1.0f, sm::vec<float,2>{3.0f, 4.0f}); // position with its curve parameter t=1.0
sm::bezcoord<float> d (1.0f, sm::vec<float,2>{3.0f, 4.0f}, 2.5f); // ...and remaining dist = 2.5
```
`param` (`t`) defaults to `-1`, and `remaining` (accessed via `get_remaining()`/`set_remaining()`) also defaults to `-1` - by convention, both use `-1` to mean 'unset' rather than an actual value.

## Accessors

```c++
float x = c.x(), y = c.y(), t = c.t(); // short forms for coord[0], coord[1], param
bool null1 = c.get_null_coordinate();  // Query if this is a 'null coordinate'
bool null2 = c.is_null();              // is_null() is the same as get_null_coordinate()
c.set_null_coordinate (true);
```

## Distances

Find distances from `bezcoord` a to `bezcoord` b:

```c++
float d  = a.distance_to (b);       // Euclidean distance
float dx = a.horz_distance_to (b);  // |a.x() - b.x()|
float dy = a.vert_distance_to (b);  // |a.y() - b.y()|
```

## Arithmetic

`add`/`subtract` mutate `coord` in place, taking either a raw `sm::vec<F,2>` or another `bezcoord` (only its `coord` is used):
```c++
a.add (sm::vec<float,2>{1.0f, 1.0f});
a.subtract (b);
```
`operator-` instead returns a new `bezcoord` holding the difference of the two coordinates - but **not** a full, meaningful subtraction of every field: the result's `param` and `remaining` are left at their default 'unset' value of `-1`, regardless of what `param`/`remaining` were on either operand:
```c++
sm::bezcoord<float> diff = b - a; // diff.coord == b.coord - a.coord; diff.t() == -1
```

## Normalizing and flipping

`normalize()` divides `coord` by its distance from the origin, turning the coordinate into a unit vector pointing in the same direction:
```c++
a.normalize(); // a.coord is now a unit vector
```
`invert_y()` negates the y coordinate in place - useful when converting from SVG's left-handed coordinate system to a right-handed one.

## Printing

`operator<<` writes `t,x,y`:
```c++
sm::bezcoord<float> pt (1.0f, sm::vec<float,2>{3.0f, 4.0f});
std::cout << pt << std::endl; // 1,3,4
```

*This page was authored with AI, based on human written code in bezcoord.cppm. Reviewed by Seb James*
