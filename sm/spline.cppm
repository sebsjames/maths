// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * Cubic spline interpolation
 *
 * \author Seb James
 */
module;

#include <cstdint>
#include <iostream>

export module sm.spline;

import sm.constexpr_math; // constexpr math functions from Keith O'Hara
export import sm.mathconst;
export import sm.vec;
export import sm.mat;

export namespace sm
{
    // (Natural) cubic spline for N 2D coordinates in (floating point) type F. Computes the
    // coefficients for N - 1 cubic curves, so the result is 4 * (N - 1) coefficients.
    template<typename F, std::uint32_t N>
    struct spline
    {
        // Points
        sm::vec<sm::vec<F, 2>, N> p; // imagine N = 4
        // Coefficients. c[0][3] to c[0][0] are the coefficients for cubic function 0. c[0][3] is
        // the coefficient of x^3
        sm::vec<sm::vec<F, 4>, (N - 1)> c;

        // Compute coefficients from points
        void compute()
        {
            // A x = B
            sm::mat<F, 4 * (N - 1), 4 * (N - 1)> A = {{}};
            sm::vec<F, 4 * (N - 1)> B = {};

            // Each row of A is coefficients of:
            // sm::vec<F, 4 * (N - 1)> x = { c[0][3] c[0][2] c[0][1] c[0][0] c[1][3] c[1][2] c[1][1] c[1][0] c[2][3] c[2][2] c[2][1] c[2][0] };

            // Fill A. First the function equations (zeroth derivative) at known points
            for (std::uint32_t i = 0; i < N - 1; ++i) { // for each of N - 1 pairs of eqns
                // B
                B[2 * i]     = p[i][1];
                B[2 * i + 1] = p[i + 1][1];
                // A(r, c)
                std::uint32_t r = i * 4;
                A(2 * i, r + 0) = p[i][0] * p[i][0] * p[i][0];
                A(2 * i, r + 1) = p[i][0] * p[i][0];
                A(2 * i, r + 2) = p[i][0];
                A(2 * i, r + 3) = 1;

                A(2 * i + 1, r + 0) = p[i + 1][0] * p[i + 1][0] * p[i + 1][0];
                A(2 * i + 1, r + 1) = p[i + 1][0] * p[i + 1][0];
                A(2 * i + 1, r + 2) = p[i + 1][0];
                A(2 * i + 1, r + 3) = 1;
            }

            // Fill the first derivative equations in
            for (std::uint32_t i = 0; i < N - 1; ++i) { // for each of N - 1 pairs of eqns
                // B stays 0 for these rows
            }

            std::cout << "A = \n" << A << " * X = " << B << std::endl;
        }
    };
}
