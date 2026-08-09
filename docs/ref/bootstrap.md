---
layout: page
title: sm::bootstrap::functions
parent: Reference
nav_order: 29
permalink: /ref/bootstrap/
---
# sm::bootstrap
{: .no_toc}
## Bootstrap resampling statistics
{: .no_toc}
```c++
import sm.bootstrap;
```

Module file: [sm/bootstrap.cppm](https://github.com/sebsjames/maths/blob/main/sm/bootstrap.cppm). Test code: [tests/bootstrap1](https://github.com/sebsjames/maths/blob/main/tests/bootstrap1.cpp).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::bootstrap` implements the nonparametric bootstrap (Efron & Tibshirani): rather than relying on a distributional assumption, it estimates the sampling variability of a statistic by resampling your data (with replacement) many times and looking at how the statistic varies across those resamples. It provides bootstrapped standard errors for the mean and standard deviation, and a bootstrapped two-sample significance test for equality of means. Every function has both an `sm::vvec<T>` and a plain `std::vector<T>` overload.

## Resampling

```c++
std::vector<sm::vvec<double>> resamples;
sm::bootstrap::resample_with_replacement<double> (data, resamples, 500u); // 500 resamples, each the same size as data
```
Each of the `B` resamples is built by drawing `data.size()` indices uniformly at random, with replacement, from `data`. This is the building block every other function in this namespace uses.

## Standard errors

```c++
double se_mean = sm::bootstrap::error_of_mean<double> (data, 512u);
double se_std  = sm::bootstrap::error_of_std<double>  (data, 512u);
```
Both compute `B` resamples, evaluate the statistic (`mean()` or `std()`) on each one, and return the standard deviation of *those* `B` values - the bootstrapped estimate of the statistic's standard error. Verified while writing this page: for 1000 samples drawn from a normal distribution, `error_of_mean` (with 512 resamples) tracks the analytically-expected standard error (`std::sqrt(n)` scaling) closely across 500 repeated trials.

## Two-sample t-test for equality of means

```c++
sm::vec<double, 2> asl = sm::bootstrap::ttest_equalityofmeans<double> (zdata, ydata, 500u);
double achieved_significance = asl[0];
double min_possible_asl = asl[1];
```
This implements algorithm 16.2 from Efron & Tibshirani, testing the null hypothesis that `zdata` and `ydata` have the same mean, without assuming their variances are equal. It works by shifting both samples to share a common (combined) mean, resampling from those shifted distributions, and counting what fraction of the resampled, studentized differences-of-means are as extreme as the one actually observed between `zdata` and `ydata`.

The return value's first element is the achieved significance level (ASL): the smaller it is, the less plausible the 'equal means' null hypothesis. Its second element, `1 / B`, is the smallest ASL that `B` resamples could possibly report - if the first element comes out equal to (or, due to how the counting works, occasionally computed as less than) the second, all you can really say is 'the true ASL is below this floor', not what it actually is. The function automatically swaps its two arguments if needed, so that whichever of `zdata`/`ydata` has the larger mean is always treated as `zdata` internally.

Verified while writing this page (`tests/bootstrap1.cpp`, 1000-sample normal distributions, 500 resamples each): comparing against a distribution with a distinctly different mean consistently gives `asl[0] == 0`, while comparing against one with the same mean gives `asl[0]` scattered across a wide range (roughly `0.004` to `0.49` over 100 repeated trials) - as expected for a null hypothesis that's actually true.

*This page was authored with AI, based on human written code in bootstrap.cppm. Reviewed by Seb James*
