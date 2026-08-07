---
layout: page
title: sm::anneal
parent: Reference
nav_order: 19
permalink: /ref/anneal/
---
# sm::anneal
{: .no_toc}
## Adaptive Simulated Annealing
{: .no_toc}
```c++
import sm.anneal;
```

Module file: [sm/anneal.cppm](https://github.com/sebsjames/maths/blob/main/sm/anneal.cppm), implementing Lester Ingber's Adaptive Simulated Annealing algorithm, described in Ingber, L. (1989), "Very fast simulated re-annealing", *Mathematical and Computer Modelling* **12**, 967-973.

**Table of Contents**

- TOC
{:toc}

## Summary

Like [`sm::nm_simplex`](/maths/ref/nm_simplex/), `sm::anneal` drives its optimization via an explicit state machine (`anneal::state`) rather than calling your objective function directly - it tells you what parameter set it needs evaluated next, you compute the objective yourself and feed the value back in.

Where Nelder-Mead makes a purely local, gradient-free descent from a fixed simplex, simulated annealing is a *global* optimization method: it can escape local minima by occasionally accepting a candidate that's worse than its current point, with the probability of accepting a worse candidate controlled by a 'temperature' that cools as the search progresses. Ingber's *adaptive* variant additionally periodically 'reanneals' - it estimates, from the local tangents of the objective function, how sensitive the result is to each parameter, and rescales its per-parameter step sizes (and effectively restarts the cooling schedule) accordingly. This lets it take large steps in insensitive dimensions and small, careful steps in sensitive ones.

## Creating an annealer

The constructor takes your initial parameter guess and the `[min, max]` range to search in each dimension:
```c++
sm::vvec<double> initial_params = { 0.0, 0.0 };
sm::vvec<sm::vec<double, 2>> ranges = { {-10.0, 10.0}, {-10.0, 10.0} }; // search box for each of the 2 parameters
sm::anneal<double> annl (initial_params, ranges);
```
The number of dimensions, `D`, is set from `initial_params.size()`. The second template parameter, `debug` (default `false`), enables extra diagnostic output on every step when `true`.

After constructing, but **before** calling `init()`, you can adjust any of the algorithm's tuning parameters - `temperature_ratio_scale`, `temperature_anneal_scale`, `cost_parameter_scale_ratio`, `acc_gen_reanneal_ratio`, `delta_param`, `f_x_best_repeat_max`, `enable_reanneal`, `reanneal_after_steps`, `exit_at_T_f`, and `downhill` (as with `nm_simplex`, `true` by default, meaning descend to a minimum; set `false` to ascend to a maximum). The defaults follow Ingber's own ASA reference implementation. Once you're happy with the parameters, call `init()`:
```c++
annl.downhill = true;
annl.init(); // sets state to need_to_compute
```

## Driving the algorithm

After `init()`, loop on `state` until it reaches `ready_to_stop`:
```c++
while (annl.state != sm::anneal_state::ready_to_stop) {
    if (annl.state == sm::anneal_state::need_to_compute) {
        // Evaluate your objective function at the candidate parameters, x_cand
        annl.f_x_cand = my_objective (annl.x_cand);
    } else if (annl.state == sm::anneal_state::need_to_compute_set) {
        // Reannealing: evaluate the objective at the perturbed parameters, x_plusdelta
        annl.f_x_plusdelta = my_objective (annl.x_plusdelta);
    }
    annl.step();
}
```
`need_to_compute` is by far the most common state you'll see; `need_to_compute_set` only appears while [reannealing](#reannealing) is in progress. (The enum also defines `need_to_init` and `need_to_step`, but these are purely internal/transient - `need_to_init` is only ever the state immediately after construction, before you call `init()`, and `need_to_step` is set and then immediately overwritten within a single call to `step()`, so you'll never observe it from client code.)

Once `state == ready_to_stop`, read off the result:
```c++
sm::vvec<double> best_params = annl.x_best;
double best_value = annl.f_x_best;
```
`reason_for_exit` (an `anneal_stopcondition`: `unknown`, `T_k_less_than_T_f`, `T_k_less_than_epsilon`, `T_cost_less_than_epsilon` or `f_x_best_repeated`) tells you why the algorithm stopped.

**Note:** whenever `acceptance_check()` accepts a candidate that's worse than the current point, it unconditionally prints `"Accepted worse candidate"` to `std::cout` - this isn't currently gated by `display_temperatures`, `display_reanneal`, or any other flag.

## Reannealing

Periodically (governed by `acc_gen_reanneal_ratio` and `reanneal_after_steps`), the algorithm needs to re-estimate how sensitive the objective is to each parameter. When this happens, `step()` sets `state` to `need_to_compute_set` and populates `x_plusdelta` - a perturbed version of the current best parameters. Your job is just to evaluate the objective there and store it in `f_x_plusdelta`, exactly as shown in the loop above; `complete_reanneal()` (called internally on the next `step()`) uses that value, together with the already-known `f_x`, to estimate tangents and rescale the per-dimension temperatures accordingly.

## Saving optimization history

`save (path)` writes the full accepted/rejected parameter history, the temperature histories, the best parameters found and all of the algorithm's tuning parameters to an HDF5 file, using [`sm::hdfdata`](/maths/ref/hdf/):
```c++
annl.save ("anneal_run.h5");
```

## Worked example

Minimizing `(x - 2)^2 + (y + 3)^2` over `[-10, 10] x [-10, 10]`:
```c++
sm::vvec<double> initial_params = { 0.0, 0.0 };
sm::vvec<sm::vec<double, 2>> ranges = { {-10.0, 10.0}, {-10.0, 10.0} };
sm::anneal<double> annl (initial_params, ranges);
annl.init();

while (annl.state != sm::anneal_state::ready_to_stop) {
    if (annl.state == sm::anneal_state::need_to_compute) {
        double x = annl.x_cand[0], y = annl.x_cand[1];
        annl.f_x_cand = (x - 2.0) * (x - 2.0) + (y + 3.0) * (y + 3.0);
    } else if (annl.state == sm::anneal_state::need_to_compute_set) {
        double x = annl.x_plusdelta[0], y = annl.x_plusdelta[1];
        annl.f_x_plusdelta = (x - 2.0) * (x - 2.0) + (y + 3.0) * (y + 3.0);
    }
    annl.step();
}
// annl.x_best == (2, -3); annl.f_x_best == 4.4e-15; annl.steps == 741
// annl.reason_for_exit == sm::anneal_stopcondition::T_cost_less_than_epsilon
```

*This page was authored with AI, based on human written code in anneal.cppm. Reviewed by Seb James*
