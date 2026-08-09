---
layout: page
title: sm::nm_simplex
parent: Reference
nav_order: 18
permalink: /ref/nm_simplex/
---
# sm::nm_simplex
{: .no_toc}
## Nelder-Mead simplex optimization
{: .no_toc}
```c++
import sm.nm_simplex;
```

Module file: [sm/nm_simplex.cppm](https://github.com/sebsjames/maths/blob/main/sm/nm_simplex.cppm), implemented following [the Nelder-Mead Wikipedia page](https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method). Test and example code: [tests/nmsimplex1](https://github.com/sebsjames/maths/blob/main/tests/nmsimplex1.cpp), [examples/nelder_mead](https://github.com/sebsjames/maths/blob/main/examples/nelder_mead.cpp).

**Table of Contents**

- TOC
{:toc}

## Summary

The Nelder-Mead method is a derivative-free optimization algorithm. It maintains a *simplex* - `n + 1` vertices in an `n`-dimensional search space (a triangle for `n = 2`, a tetrahedron for `n = 3`, and so on) - and repeatedly reflects, expands, contracts or shrinks it, always moving the worst vertex towards better territory, until the vertices' objective-function values converge.

`sm::nm_simplex<T>` implements this as an explicit **state machine** rather than calling your objective function itself: it never calls your code back, and it doesn't hold a function pointer as its primary interface. Instead, `nm_simplex::state` tells you what point(s) it needs evaluated next; you compute your objective function's value at that point yourself, feed the value back in, and ask the simplex to continue. This makes it straightforward to drive the optimization from a context where evaluating the objective is expensive, asynchronous, or happens on a GPU - you're never forced to make it a synchronous callback. If your objective function evaluation genuinely *is* just a synchronous C++ function or lambda, you can instead assign it to `nm_simplex::objective` and call `run()`, which drives the whole state machine for you.

## Creating a simplex

The dimensionality `n` is inferred from the number of vertices you supply (`n = (number of vertices) - 1`). The general constructor takes an `sm::vvec` of `n + 1` vertices, each element being an `sm::vvec` of `n` coordinates:
```c++
sm::vvec<sm::vvec<float>> i_vertices = {
    { 0.7, 0.0 },
    { 0.0, 0.6 },
    { -0.6, -1.0 }
};
sm::nm_simplex<float> simp (i_vertices); // n == 2 (a triangle)
```
There are convenience constructors for the common 1D (two scalar vertices) and 2D (three `sm::vec<T, 2>` vertices) cases:
```c++
sm::nm_simplex<float> simp1d (0.0f, 1.0f);                                  // n == 1
sm::nm_simplex<float> simp2d ({0.7f, 0.0f}, {0.0f, 0.6f}, {-0.6f, -1.0f});  // n == 2, equivalent to the vvec example above
```
You can also construct with just a dimensionality (`sm::nm_simplex<float> simp (2)`), leaving all vertex coordinates at zero, or default-construct (`n` defaults to `2`) and set things up yourself. Either way, you'll need to fill in `vertices` (or call `reset`, below) before running.

To reuse an existing `nm_simplex` object for a fresh optimization (a new set of initial vertices, dimensionality possibly different from before), call `reset`:
```c++
simp.reset (new_i_vertices);
```

## Setting up the search

Assign your objective function to `objective`, a `std::function<T(const sm::vvec<T>&)>`:
```c++
simp.objective = [](const sm::vvec<float>& point) {
    float x = point[0], y = point[1];
    return (1.0f - x) * (1.0f - x) + 100.0f * (y - x * x) * (y - x * x); // Rosenbrock's banana function
};
```
By default the simplex descends towards the *minimum* of the objective function; set `downhill = false` before running to instead ascend towards the maximum.

`termination_threshold` (default `0.0001`) is the key stopping parameter: once the standard deviation of the objective-function values across all vertices drops below this, the simplex is considered to have converged. Tune it to whatever precision your problem needs - the Rosenbrock example below sets it to `std::numeric_limits<T>::epsilon()`. `too_many_operations`, if set greater than zero, is a safety net that stops the algorithm (with `stopreason == too_many_operations`) if it performs more than that many shape-changing operations without converging - usually a sign that `termination_threshold` was set too tight for the problem.

`alpha`, `gamma`, `rho` and `sigma` are the reflection, expansion, contraction and shrink coefficients, initialized to the standard values from the Nelder-Mead Wikipedia page (`1`, `2`, `0.5`, `0.5`); you shouldn't normally need to change these.

## Running the optimization

If your objective function is a plain synchronous callable, the simplest approach is `run()`, which loops internally until the simplex is done:
```c++
simp.termination_threshold = std::numeric_limits<float>::epsilon();
if (!simp.run()) { std::cerr << "Objective was not set\n"; }
```
`run()` returns `false` (without doing anything) if you didn't set `objective`.

Otherwise, drive it yourself by checking `state` and calling `step()` in a loop - useful if you want to interleave evaluation with other work, or plot the simplex's progress:
```c++
while (simp.state != sm::nm_simplex_state::ready_to_stop) { simp.step(); }
```
This is exactly what `run()` does internally; `step()` itself examines `state` to decide whether it needs to evaluate the objective at all `n + 1` vertices, at the reflected point `xr`, the expanded point `xe`, or the contracted point `xc` - and, because `objective` is set, it calls it directly rather than asking you to.

## The state machine

`nm_simplex_state` is:

| Value | Meaning |
|---|---|
| `unknown` | Not yet initialized with vertices |
| `need_to_compute_then_order` | Every vertex needs a fresh objective-function evaluation, then the vertices need ordering |
| `need_to_order` | Vertices just need re-ordering (best to worst) |
| `need_to_compute_reflection` | Evaluate the objective at the reflected point `xr` |
| `need_to_compute_expansion` | Evaluate the objective at the expanded point `xe` |
| `need_to_compute_contraction` | Evaluate the objective at the contracted point `xc` |
| `ready_to_stop` | Finished - read `best_vertex()`/`best_value()` |

Once the simplex reaches `ready_to_stop`, `stopreason` (an `nm_simplex_stop_reason`: `none`, `termination_threshold` or `too_many_operations`) tells you why.

## Reading the result

```c++
sm::vvec<float> best_point = simp.best_vertex();
float best_val = simp.best_value();
```
`operation_count` tracks how many shape-changing operations (reflections, expansions, contractions, shrinks) were performed - useful for judging how hard the problem was, or for tuning `too_many_operations`.

## Worked example

From [tests/nmsimplex1.cpp](https://github.com/sebsjames/maths/blob/main/tests/nmsimplex1.cpp), minimizing the Rosenbrock banana function:
```c++
sm::vvec<sm::vvec<float>> i_vertices = { {0.7, 0.0}, {0.0, 0.6}, {-0.6, -1.0} };
sm::nm_simplex<float> simp (i_vertices);
simp.objective = [](const sm::vvec<float>& point) {
    float x = point[0], y = point[1];
    constexpr float a = 1.0f, b = 100.0f;
    return ((a - x) * (a - x)) + (b * (y - (x * x)) * (y - (x * x)));
};
simp.termination_threshold = std::numeric_limits<float>::epsilon();
simp.run();

sm::vvec<float> best = simp.best_vertex();
std::cout << "Best approximation: (" << best << ") has value " << simp.best_value() << std::endl;
```
Running this gives:
```
FINISHED! Best approximation: ((1.00009227,1.00019765)) has value 2.57085e-08
```
- converging, as expected, close to the Rosenbrock function's known minimum at `(1, 1)`.

*This page was authored with AI, based on human written code in nm_simplex.cppm. Reviewed by Seb James*
