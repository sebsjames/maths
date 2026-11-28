// -*- C++ -*-
/*!
 * See https://github.com/sebsjames/maths
 *
 * General polynomial solver functions
 *
 * This header provides:
 * - Functions in the sm::polysolve namespace, with the top-level function being sm::polysolve::solve()
 *
 * Features:
 * - Analytical solvers for degrees 1-4:
 *   * Linear: Direct solution
 *   * Quadratic: Quadratic formula
 *   * Cubic: Cardano's method
 *   * Quartic: Ferrari's method
 * - Numerical Durand-Kerner (Weierstrass) algorithm for degree > 4
 * - Handles real and complex roots, multiple roots, degenerate cases
 *
 * Usage:
 *   import sm.polysolve;
 *   std::vector<double> coeffs = {6.0, -5.0, 1.0};         // [a0, a1, a2] for x^2 - 5x + 6 = 0
 *   std::vector<std::complex<double>> roots = polysolve::solve<double> (coeffs);
 *   std::vector<double> real_roots = polysolve::real<double> (coeffs);
 *
 * Notes:
 * - Coefficient order when presented in an array is [a0, a1, ..., an] for a_n*x^n + ... + a_1*x + a_0 = 0
 * - Functions return complex roots (may be real with zero imaginary part)
 * - All solvers return roots in sorted order: lexicographic by (real, imag)
 *
 * See polysolve_1.cpp in mathplot/maths/tests/ for a set of tests
 *
 * Authors
 * Alex Blenkinsop, using Claude Code 4.2
 * Seb James, un-fscking the AI code.
 *
 * Date Jan - Feb 2026
 */
module;

#include <sm/polysolve.hpp>

export module sm.polysolve;

export namespace sm::polysolve
{
    using sm::polysolve::remove_trailing_zeros;
    using sm::polysolve::evaluate;
    using sm::polysolve::sort_roots;
    using sm::polysolve::linear;
    using sm::polysolve::quadratic;
    using sm::polysolve::cubic;
    using sm::polysolve::quartic;
    using sm::polysolve::high_order;
    using sm::polysolve::solve;
    using sm::polysolve::real;
}
