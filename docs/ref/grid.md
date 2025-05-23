---
layout: page
title: sm::grid
parent: Reference
permalink: /ref/grid/
---
```c++
#include <sm/grid>
```

A Cartesian grid class. `sm::grid` is a simpler version of [`sm::cartgrid`](https://github.com/sebsjames/maths/blob/main/sm/cartgrid). The difference is that any `sm::grid` is rectangular, whereas a `sm::cartgrid` may be constructed with an arbitrary domain boundary (`sm::hexgrid` objects can also have an arbitrary boundary). Unless you need non-rectangular boundaries for your Cartesian grids, prefer `sm::grid` over `sm::cartgrid`.
