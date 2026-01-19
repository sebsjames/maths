---
title: sm::polysolve
parent: Reference
layout: page
permalink: /ref/polysolve
nav_order: 16
---
# sm::polysolve
## Polynomial solver namespace
{: .no_toc}

```c++
#include <sm/polysolve>
```
Header file: [<sm/polysolve>](https://github.com/sebsjames/maths/blob/main/sm/polysolve).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::polysolve` is a namespace that contains solvers for real-valued polynomials of arbitrary order.
You provide the polynomial coefficients and standalone functions in the namespace can compute the
roots.

The namespace also includes solvers for linear and quadratic complex polynomials.

## Finding roots of real polynomials

The general purpose polynomial solver is `sm::polysolve::solve`.
There are two overloads of `solve`.
Both are templated with the numeric type to use.
One version allows you to provide the coefficients in a fixed-length `sm::vec` and the other allows you to provide them in an `sm::vvec`.

```c++
namespace sm::polysolve
{
    template <typename T, typename Ty=T> // not showing 'requires' conditions here
    sm::vvec<std::complex<T>> solve (const sm::vvec<Ty>& coeffs);

    template <typename T, std::size_t N, typename Ty = T>
    sm::vvec<std::complex<T>> solve (const sm::vec<Ty, N+1>& coeffs);
}
```

From these definitions, you can see that both accept an array of real-valued coefficients and both return a dynamically sized `sm::vvec` containing the roots as complex numbers. T
he coefficients can be provided in a different type from the 'main' type `T`.
This allows you to pass coefficients as `float`s or `int`s but compute in `double` precision.
Roots are returned using the main type `T`.

```c++
// Find the solutions of the quadratic equation x^2 - 5 x + 6 = 0
sm::vvec<double> c_quadratic = { 6, -5, 1 };
sm::vvec<std::complex<double>> roots = sm::polysolve::solve<double> (c_quadratic);
```
The order of the polynomial is defined by the number of elements in the coefficients array.
You can run the test program **polysolve_1** and see that the roots are 2 and 3.
The return object may contain 0 or more roots.

Here's a cubic example for x^3 + 8 = 0

```c++
sm::vvec<double> c_cubic = { 8, 0, 0, 1 };
sm::vvec<std::complex<double>> roots = sm::polysolve::solve<double> (c_cubic);
```
Note that you *do* need to provide the first template argument to `solve`.
The compiler cannot deduce the type T from the argument.

### Solving using `sm::vec` coefficient array

The cubic example again:

```c++
sm::vec<double, 4> c_cubic = { 8, 0, 0, 1 };
sm::vvec<std::complex<double>> roots = sm::polysolve::solve<double, 3> (c_cubic);
```
Again, unfortunately, the compiler cannot deduce the dimension template value (3) from the input (a vec of length 4), so you have to write it into the function call.

## Evaluating real valued polynomials

`sm::polysolve` contains functions to evaluate polynomials.

```c++
sm::vvec<double> c_quadratic = { 6, -5, 1 };
// Evaluate at the root, 2:
std::cout << "f(2) = " << sm::polysolve::evaluate (c_quadratic, 2.0) << std::endl;
// Outputs "f(2) = 0"
```

## Underlying functions

`sm::polysolve::solve` uses a number of sub-calls to solve the polynomial, depending on the order.
First to fourth order polynomials are solved with `polysolve::linear`, `polysolve::quadratic`, `polysolve::cubic` and `polysolve::quartic`, which are analytical solvers. A direct solution is computed for `polysolve::linear`; `polysolve::quadratic` uses the common quadratic formula; `polysolve::cubic` implements Cardano's method and `quartic` implements Ferrari's method.
Higher order polynomials are solved using the numerical Durand-Kerner (Weierstrass) algorithm.

## Finding roots of complex polynomials

While `polysolve::solve` cannot solve complex-valued polynomials, two functions are provided to solve complex-valued linear and quadratic polynomials:

```c++
// Solves f = a1 z + a0
template <typename T, typename Ty=T> // requires not shown
sm::vvec<std::complex<T>> linear (const std::complex<Ty>& a1, const std::complex<Ty>& a0);

// Solves f = a2 z^2 + a1 z + a0
template <typename T, typename Ty=T>
sm::vvec<std::complex<T>> quadratic (const std::complex<Ty>& a2, const std::complex<Ty>& a1, const std::complex<Ty>& a0);
```