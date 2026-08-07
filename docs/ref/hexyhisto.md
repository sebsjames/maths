---
layout: page
title: sm::hexyhisto
parent: Reference
nav_order: 35
permalink: /ref/hexyhisto/
---
# sm::hexyhisto
{: .no_toc}
## A 2D histogram of point data on a hexgrid
{: .no_toc}
```c++
import sm.hexyhisto;
```

Module file: [sm/hexyhisto.cppm](https://github.com/sebsjames/maths/blob/main/sm/hexyhisto.cppm).

**Note:** at the time of writing, `sm.hexyhisto` has no dedicated `SM_HEXYHISTO_MODULES` entry in [cmake/module_definitions.cmake](https://github.com/sebsjames/maths/blob/main/cmake/module_definitions.cmake), and no test or example in this repository. It does compile and run (verified independently while writing this page, using a standalone build against [`sm::hexgrid`](/maths/ref/hexgrid/)) - but see the important caveat below before using it.

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::hexyhisto<T>` bins a cloud of 2D points onto an existing [`sm::hexgrid`](/maths/ref/hexgrid/), counting how many points land in (or near) each hex - a 2D histogram for hex-tiled data, useful for plotting a density map of e.g. crossing points or events over a spatial domain.

## Careful: the third coordinate must be exactly zero

```c++
sm::hexgrid hg (0.1f, 2.0f, 0.0f);
hg.set_circular_boundary (0.5f);

sm::vvec<sm::vec<float>> data; // sm::vec<float> defaults to 3 elements: {x, y, flag}
data.push_back ({ 0.0f, 0.0f, 0.0f }); // a valid point at the origin - flag MUST be 0.0f, not e.g. 1.0f
data.push_back ({ 0.2f, 0.2f, -1.0f }); // flag < 0 means "skip this point"

sm::hexyhisto<float> hh (data, &hg);
```
Each `data` entry is a 3-element `sm::vec<T>`: `{x, y, flag}`. A negative `flag` marks a point to be skipped entirely. **However**, for points that *are* included, the constructor computes the distance from the point to its nearest hex as `(hipos - datum).length()`, where `hipos` is built as `{hex.x, hex.y, T{0}}` - meaning `datum`'s third component (your `flag`) is subtracted against a literal `0`, and so ends up contributing to the computed 3D Euclidean distance as if it were a z-coordinate. Verified while writing this page: 50 coincident points at the origin were binned correctly (`datacount == 50`) when their flag was `0.0f`, but **all 50 were silently dropped** (`datacount == 0`) when the exact same points instead used a flag of `1.0f` - because the resulting 'distance' of `1.0` exceeded the hex-spacing threshold `hg.get_v()` used to decide whether a point is close enough to its nearest hex to count. **Only use a flag value of exactly `0.0` for points you want counted**, and a negative value to skip them.

## Reading the result

```c++
T total = hh.datacount;          // how many input points were actually counted
sm::vvec<T> counts = hh.counts;       // raw count per hex, indexed by each hex's vi
sm::vvec<T> proportions = hh.proportions; // counts, normalized to sum to 1 (counts / datacount)
```
`proportions` is exactly what you'd plot on the `sm::hexgrid` to visualize the histogram as a density map.

*This page was authored with AI, based on human written code in hexyhisto.cppm.*
