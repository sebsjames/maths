---
layout: page
title: sm::polysolve (namespace)
parent: Reference
nav_order: 14
permalink: /ref/polysolve/
---
# sm::polysolve
{: .no_toc}
## Polynomial root solving functions
{: .no_toc}
```c++
import sm.polysolve;
```

Module file: [sm/polysolve.cppm](https://github.com/sebsjames/maths/blob/main/sm/polysolve.cppm). Test code: [tests/polysolve_1](https://github.com/sebsjames/maths/blob/main/tests/polysolve_1.cpp).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::polysolve` is not a class, but a namespace of free functions for finding the roots of a polynomial. Polynomials of degrees 1-4 are solved analytically (the linear formula, the quadratic formula, Cardano's method for cubics and Ferrari's method for quartics); those of degree 5 and above are solved numerically with the Durand-Kerner (Weierstrass) iteration. Roots are always returned as `std::complex<T>`, even when they turn out to be purely real.

## Solving a polynomial

The top-level function to call is `polysolve::solve`, which picks the right analytical solver for the polynomial's degree, or falls back to the numerical method for degree 5 and above:
```c++
std::vector<double> coeffs = { 6.0, -5.0, 1.0 };  // a0=6, a1=-5, a2=1, i.e. x^2 - 5x + 6 = 0
std::vector<std::complex<double>> roots = sm::polysolve::solve<double> (coeffs);
// roots == { (2,0), (3,0) }
```

Coefficients are always given lowest-degree first: `[a0, a1, ..., an]` for `an*x^n + ... + a1*x + a0 = 0`. This is the opposite order to the way you'd normally write the polynomial down, but it means `coeffs[i]` is always the coefficient of `x^i`, and the degree of the polynomial is `coeffs.size() - 1`.

Every solver returns a `std::vector<std::complex<T>>` of roots, sorted lexicographically by `(real, imag)`, so real roots (whose imaginary part rounds to zero) come first, in ascending order, followed by any complex-conjugate pairs.


There's also a fixed-size overload taking a `std::array`, for when you know the degree at compile time:
```c++
std::array<double, 3> coeffs2 = { 6.0, -5.0, 1.0 }; // N = 2 (degree), so the array has N+1 = 3 elements
std::vector<std::complex<double>> roots2 = sm::polysolve::solve<double, 2> (coeffs2);
```
Both overloads take a second (defaulted) template parameter, `Ty`, for the coefficient type, distinct from `T`, the type used internally and for the returned roots; useful if your coefficients are `float` but you want the numerical computation carried out (and your roots returned) in `double`:
```c++
std::vector<float> coeffs_f = { 6.0f, -5.0f, 1.0f };
std::vector<std::complex<double>> roots3 = sm::polysolve::solve<double, float> (coeffs_f); // T=double, Ty=float
```

Both overloads throw `std::invalid_argument` if every coefficient is zero (or, for the vector overload, if `coeffs` is empty), and return an empty vector (rather than throwing) for a non-zero constant polynomial, which has no roots.

Before solving, trailing (highest-degree) coefficients that are within `std::numeric_limits<T>::epsilon()` of zero are stripped off, so passing e.g. `{6, -5, 1, 0}` (a cubic with a zero leading term) is solved as the quadratic it really is.

## Real roots only

If you only care about the real roots, `polysolve::real` calls `polysolve::solve` and then filters out any root whose imaginary part exceeds a tolerance (which defaults to `100 * std::numeric_limits<T>::epsilon()`), returning them in ascending order:
```c++
std::vector<double> real_roots = sm::polysolve::real<double> (coeffs);
```
It has a matching `std::array` overload.

## Solving a specific degree directly

If you already know the degree, you can call the individual solvers directly rather than going through `solve`. Each takes its coefficients highest-degree first, as separate arguments (note the reversal from `solve`'s lowest-first vector/array convention):
```c++
auto lin  = sm::polysolve::linear<double> (2.0, -6.0);                  // a1, a0    for  2x - 6 = 0
auto quad = sm::polysolve::quadratic<double> (1.0, -5.0, 6.0);          // a2, a1, a0 for x^2 - 5x + 6 = 0
auto cub  = sm::polysolve::cubic<double> (1.0, -6.0, 11.0, -6.0);       // a3..a0
auto qrt  = sm::polysolve::quartic<double> (1.0, -10.0, 0.0, 9.0, 0.0); // a4..a0
```
`linear` and `quadratic` also have overloads that accept `std::complex<Ty>` coefficients, for solving a polynomial whose coefficients are themselves complex (this is exactly what `quartic`'s Ferrari's-method implementation uses internally to solve its complex-coefficient resolvent equations).

For degree 5 and above, `high_order` runs the Durand-Kerner method directly (this is also what `solve` falls back to). It's useful if you specifically want the numerical method, e.g. to compare against an analytical result:
```c++
std::vector<double> quintic = { -120, 274, -225, 85, -15, 1 }; // (x-1)(x-2)(x-3)(x-4)(x-5)
std::vector<std::complex<double>> roots = sm::polysolve::high_order<double> (quintic);
```
`high_order` seeds its `degree` initial guesses evenly around a circle, then iterates up to 100 times, stopping early once every root's update falls below `std::numeric_limits<T>::epsilon()`.

## Helper functions

A handful of smaller utilities used internally by the solvers above are also exported, in case you find them useful on their own:
```c++
sm::polysolve::remove_trailing_zeros (coeffs); // strip coeffs of near-zero highest-degree coefficients
T y = sm::polysolve::evaluate (coeffs, x);     // Horner's method; x and coeffs may be real or complex
sm::polysolve::sort_roots (roots);             // sort in place, lexicographically by (real, imag)
```

*This page was authored with AI, based on human written code in polysolve.cppm and reviewed by Seb James.*
