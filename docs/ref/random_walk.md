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

Module file: [sm/random_walk.cppm](https://github.com/sebsjames/maths/blob/main/sm/random_walk.cppm)

**Table of Contents**

- TOC
{:toc}

## Summary
`sm::random_walk<T>` implements the random-walk generation model described in Stone, T. et al. (2017), "An Anatomically Constrained Model for Path Integration in the Bee Brain", *Current Biology* **27**, 3069-3085 ([DOI: 10.1016/j.cub.2017.08.052](http://dx.doi.org/10.1016/j.cub.2017.08.052)).

`random_walk` generates a 2D path with the kind of statistics seen in real foraging-insect flight: a slowly-varying forward acceleration combined with a correlated (rather than fully random) turning angle, so the path curves smoothly rather than jittering back and forth. At each step, a random angular acceleration is drawn from a [von Mises distribution](/maths/ref/randn/) (its concentration parameter `kappa` controls the 'wiggliness' of the path). A separately generated, smoothly-varying linear acceleration profile drives the walker's forward speed.

## Creating a walk

```c++
sm::random_walk<float> rw (500u, 50u);              // n_steps, a_tau
sm::random_walk<float> rw2 (500u, 50u, T{50});      // ...and kappa (von Mises concentration)
sm::random_walk<float> rw3 (500u, 50u, T{50}, T{0.01}); // ...and acc_max (peak acceleration)
```
`n_steps` is how many steps `generate` (below) will produce. `a_tau` controls how often the underlying acceleration profile changes: a fresh random acceleration value is drawn every `a_tau` steps, then smoothed across those steps with a cubic spline (see [Acceleration profile](#acceleration-profile), below) so the forward acceleration varies gradually rather than jumping. `kappa` is the von Mises concentration parameter for the angular-acceleration draws (default `100`; higher values give straighter, less-wandering paths). `acc_max` sets the upper end of the uniform distribution the acceleration profile is drawn from - the default interval is `[0, 0.005]`.

## Generating a path

```c++
sm::vvec<sm::vec<float, 2>> path = rw.generate(); // starting at theta=0, position=(0,0)
// or from a given heading/position:
sm::vvec<sm::vec<float, 2>> path2 = rw.generate (theta0, start_position);
```
`generate` returns `n_steps` coordinates, calling `step()` internally once per coordinate. You can also call `step()` yourself if you want to inspect the walker's state (`theta`, `omega`, `velocity`, `speed`) between steps, and `about_turn()` to add a half-turn (`pi` radians) onto the current heading.

`reset()` zeroes `t`, `theta`, `omega`, `velocity` and `speed`, but does **not** regenerate the acceleration profile `a` or the von Mises random number generator - call `init()` again (which does both, plus reapplying `n_steps`/`a_tau`) if you want a genuinely fresh walk rather than just restarting the clock on the existing one.

## Acceleration profile

At construction, `init()` draws `n_steps / a_tau` random values (uniformly, then scaled into `[0, acc_max]`) and expands them with `cubic_spline_expansion` (see below) into the member `a`, which `step()` reads one element at a time as it advances.

**Note:** the expanded `a` ends up with `(n_steps / a_tau) * (a_tau + 1)` elements, which generally isn't exactly `n_steps`: for `n_steps=500, a_tau=50`, `a.size()` comes out as `510`, not `500`. This is harmless for `random_walk` itself (`step()` guards its lookup with `if (t < this->a.size())`), but is worth knowing if you inspect `a` directly.

*This page was authored with AI, based on human written code in random_walk.cppm and reviewed by Seb James.*
