---
layout: page
title: sm::rungekutta4
parent: Reference
nav_order:
permalink: /ref/rungekutta4/
---
# sm::rungekutta4
{: .no_toc }
## A fourth order Runge Kutta solver class
{: .no_toc }
```c++
import sm.rungekutta4;
```

Module file: [sm/rungekutta4.cppm](https://github.com/sebsjames/maths/blob/main/sm/rungekutta4.cppm). Test code:
[tests/arungekutta4_1](https://github.com/sebsjames/maths/blob/main/tests/rungekutta4_1.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary

A fourth order Runge Kutta solver class with a fixed (i.e. non-adaptive) step size.

## Usage

You first instantiate your `sm::rungekutta4`, giving it the function
dx/dt, the initial state (x0), time (usually 0) and a step size. You
can then advance the state by a single step with
`sm::rungekutta4::step()` or find a time series of solutions with
`sm::rungekutta4::integrate()`.

### Single ODE setup

`x` and `t` have scalar type, such as `double`.

```c++
auto dxdt = [](const double& t, const double& x) { return -x; };
double x0 = 1.0; // Initial state x(t0)
double t0 = 0.0; // Initial time
double h = 0.01; // step size
sm::rungekutta4<double> rks (dxdt, x0, t0, h);
// Can now rks.step(); or rk.integrate(...);
```
Alternatively, use a default constructor, and the `sm::rungekutta4::init()` method:
```c++
sm::rungekutta4<double> rks;
rks.init (dxdt, x0, t0, h);
```

### Multiple ODE setup
`t` and `h` still have scalar type, such as `float`, but the state `x` may be `sm::vec<>` or `sm::vvec<>`.
```c++
auto dxdt = [](const float& t, const sm::vec<float, 2>& x) {
                return sm::vec<float, 2>{ x[1], -x[0] };
            };
sm::vec<float, 2> x0 = {1, 0};
float t0 = 0.0f;
float h = 0.01f;
sm::rungekutta4<float, sm::vec<float, 2>> rkm (dxdt, x0, t0, h);
```

### Finding solutions

Either step forwards for each timestep with `rungekutta4::step()` or call `integrate`:

```c++
sm::vvec<double> traj_s = rks.integrate (100);
sm::vvec<sm::vec<float, 2>> traj_m = rkm.integrate (1000);
```
