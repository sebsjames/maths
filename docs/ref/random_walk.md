---
layout: page
title: sm::random_walk
parent: Reference
nav_order: 34
permalink: /ref/random_walk/
---
# sm::random_walk
{: .no_toc}
## A correlated random walk, following a bee flight model
{: .no_toc}
```c++
import sm.random_walk;
```

Module file: [sm/random_walk.cppm](https://github.com/sebsjames/maths/blob/main/sm/random_walk.cppm), implementing the random-walk generation model described in Stone, T. et al. (2017), "An Anatomically Constrained Model for Path Integration in the Bee Brain", *Current Biology* **27**, 3069-3085 ([DOI: 10.1016/j.cub.2017.08.052](http://dx.doi.org/10.1016/j.cub.2017.08.052)).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::random_walk<T>` generates a 2D path with the kind of statistics seen in real foraging-insect flight: a slowly-varying forward acceleration combined with a correlated (rather than fully random) turning angle, so the path curves smoothly rather than jittering back and forth. At each step, a random angular *acceleration* is drawn from a [von Mises distribution](/maths/ref/randn/) (its concentration parameter `kappa` controlling how tightly the walk tends to keep going in roughly the same direction), which accumulates into an angular velocity and then into the heading itself - and a separately-generated, smoothly-varying acceleration profile drives the walker's forward speed.

## Creating a walk

```c++
sm::random_walk<float> rw (500u, 50u);              // n_steps, a_tau
sm::random_walk<float> rw2 (500u, 50u, T{50});       // ...and kappa (von Mises concentration)
sm::random_walk<float> rw3 (500u, 50u, T{50}, T{0.01}); // ...and acc_max (peak acceleration)
```
`n_steps` is how many steps `generate` (below) will produce. `a_tau` controls how often the underlying acceleration profile changes: a fresh random acceleration value is drawn roughly every `a_tau` steps, then smoothed across those steps with a cubic spline (see [Acceleration profile](#acceleration-profile), below) so the forward acceleration varies gradually rather than jumping. `kappa` is the von Mises concentration parameter for the angular-acceleration draws (default `100`; higher values give straighter, less-wandering paths). `acc_max` sets the upper end of the (uniformly-random, then spline-smoothed) range the acceleration profile is drawn from - the range defaults to `[0, 0.005]`.

## Generating a path

```c++
sm::vvec<sm::vec<float, 2>> path = rw.generate();                       // starting at theta=0, position=(0,0)
sm::vvec<sm::vec<float, 2>> path2 = rw.generate (theta0, start_position); // or from a given heading/position
```
`generate` returns `n_steps` coordinates, calling `step()` internally once per coordinate. You can also call `step()` yourself if you want to inspect the walker's state (`theta`, `omega`, `velocity`, `speed`) between steps, and `about_turn()` to add a half-turn (`pi` radians) onto the current heading.

`reset()` zeroes `t`, `theta`, `omega`, `velocity` and `speed`, but does **not** regenerate the acceleration profile `a` or the von Mises random number generator - call `init()` again (which does both, plus reapplying `n_steps`/`a_tau`) if you want a genuinely fresh walk rather than just restarting the clock on the existing one.

**Careful:** `about_turn()` always adds `sm::mathconst<float>::pi`, regardless of the class's own template parameter `T` - so for `sm::random_walk<double>`, the half-turn added is only `float` precision, not `double`.

## Acceleration profile

At construction, `init()` draws `n_steps / a_tau` random values (uniformly, then scaled into `[0, acc_max]`) and expands them with `cubic_spline_expansion` (see below) into the member `a`, which `step()` reads one element at a time as it advances.

**Careful:** the expanded `a` ends up with `(n_steps / a_tau) * (a_tau + 1)` elements, which generally isn't exactly `n_steps` - verified while writing this page: for `n_steps=500, a_tau=50`, `a.size()` comes out as `510`, not `500`. This is harmless for `random_walk` itself (`step()` guards its lookup with `if (t < this->a.size())`), but is worth knowing if you inspect `a` directly.

## `cubic_spline_expansion`

Two free functions, also useful independently of `random_walk`, upsample an `sm::vvec` by fitting a [natural cubic spline](/maths/ref/spline/) through its existing elements (treating their index as x) and inserting `n` interpolated values between each pair:
```c++
sm::vvec<float> v = { 1.0f, 2.0f, 3.0f, 4.0f };
sm::cubic_spline_expansion<float> (v, 3u); // insert 3 points between each of the original 4
```
The runtime-sized overload (shown above) dispatches, via a `switch` on `v.size()`, to a fixed-`N` overload templated on the actual size - because [`sm::spline`](/maths/ref/spline/)'s point count is a compile-time template parameter, only a fixed list of sizes is supported (`2`-`20`, `25`, then `30`-`100` in steps of `10`); anything else throws `std::runtime_error`.

**Careful:** because the underlying spline only produces valid values up to its last knot (see [`sm::spline`](/maths/ref/spline/#evaluating-the-spline)), and the expanded vector's new size (`v.size() * (n + 1)`) is generally *larger* than that valid domain (which only spans `(v.size() - 1) * n` steps), the tail of the expanded vector can come out as zeros rather than genuinely interpolated values. Verified while writing this page: expanding `{1, 2, 3, 4}` with `n = 3` gives a 16-element result, but only the first 10 elements are real spline values (`1, 1.33, 1.67, 2, 2.33, 2.67, 3, 3.33, 3.67, 4`) - the last 6 are `0`.

*This page was authored with AI, based on human written code in random_walk.cppm.*
