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
export import sm.vvec;
export import sm.mat;

export namespace sm
{
    // (Natural) cubic spline for N 2D coordinates in (floating point) type F. Computes the
    // coefficients for N - 1 cubic curves, so the result is 4 * (N - 1) coefficients.
    template<typename F, std::uint32_t N>
    struct spline
    {
        spline() {}
        spline (const sm::vec<sm::vec<F, 2>, N>& _p)
        {
            p = _p;
            this->compute_coefficients();
        }

        // Points
        sm::vec<sm::vec<F, 2>, N> p;
        // Coefficients. c[0][3] to c[0][0] are the coefficients for cubic function 0. c[0][3] is
        // the coefficient of x^3
        sm::vec<sm::vec<F, 4>, (N - 1)> c;

        // Compute coefficients from points
        void compute_coefficients()
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
                std::uint32_t sc = i * 4;
                A(2 * i, sc + 0) = p[i][0] * p[i][0] * p[i][0];
                A(2 * i, sc + 1) = p[i][0] * p[i][0];
                A(2 * i, sc + 2) = p[i][0];
                A(2 * i, sc + 3) = F{1};

                A(2 * i + 1, sc + 0) = p[i + 1][0] * p[i + 1][0] * p[i + 1][0];
                A(2 * i + 1, sc + 1) = p[i + 1][0] * p[i + 1][0];
                A(2 * i + 1, sc + 2) = p[i + 1][0];
                A(2 * i + 1, sc + 3) = F{1};
            }

            // Fill the N - 2 first derivative equations in
            for (std::uint32_t i = 0; i < N - 2; ++i) {
                // B stays 0 for these rows
                std::uint32_t sc = i * 4;
                A((2 * (N - 1)) + i, sc + 0) = F{-3} * p[i + 1][0] * p[i + 1][0];
                A((2 * (N - 1)) + i, sc + 1) = F{-2} * p[i + 1][0];
                A((2 * (N - 1)) + i, sc + 2) = F{-1};
                A((2 * (N - 1)) + i, sc + 4) = F{3} * p[i + 1][0] * p[i + 1][0];
                A((2 * (N - 1)) + i, sc + 5) = F{2} * p[i + 1][0];
                A((2 * (N - 1)) + i, sc + 6) = F{1};
            }

            // Fill N - 2 second derivative equations
            for (std::uint32_t i = 0; i < N - 2; ++i) {
                // B stays 0 for these rows
                std::uint32_t sc = i * 4;
                A(2 * (N - 1) + N - 2 + i, sc + 0) = F{-6} * p[i + 1][0];
                A(2 * (N - 1) + N - 2 + i, sc + 1) = F{-2};
                A(2 * (N - 1) + N - 2 + i, sc + 4) = F{6} * p[i + 1][0];
                A(2 * (N - 1) + N - 2 + i, sc + 5) = F{2};

            }

            // The final 2 equations from the boundary condition that F"(end) = 0
            A(4 * (N - 1) - 2, 0) = F{6} * p[0][0];
            A(4 * (N - 1) - 2, 1) = F{2};
            A(4 * (N - 1) - 1, (N - 1) * 4 - N) = F{6} * p[N - 1][0];
            A(4 * (N - 1) - 1, (N - 1) * 4 - N + 1) = F{2};

            // Solve the above using the back substitution as in the mat::eigenvector function?
            sm::mat<F, 4 * (N - 1), 4 * (N - 1) + 1> Aug = {{}};
            for (std::uint32_t i = 0; i < 4 * (N - 1); ++i) {
                for (std::uint32_t j = 0; j < 4 * (N - 1); ++j) {
                    Aug(i, j) = A(i, j);
                }
            }
            for (std::uint32_t i = 0; i < 4 * (N - 1); ++i) {
                Aug(i, 4 * (N-1)) = B[i];
            }
            // Convert augmented matrix to row echelon form...
            Aug.row_echelon_form_inplace();
            // and solve by back-substitution
            sm::vec<F, 4 * (N - 1)> X = Aug.back_substitution();

            // Copy X into c.
            for (std::uint32_t i = 0; i < N - 1; ++i) {
                for (std::uint32_t j = 0; j < 4; ++j) { this->c[i][3 - j] = X[4 * i + j]; }
            }
        }

        F compute (const F x)
        {
            F y = F{0};
            // Find which curve to use
            for (std::uint32_t i = 1; i < p.size(); ++i) {
                if (x <= p[i][0]) { // Do the cubic computation for c[i-1]
                    y = c[i - 1][0] + c[i - 1][1] * x + c[i - 1][2] * x * x + c[i - 1][3] * x * x * x;
                    break;
                }
            }
            return y;
        }

        sm::vvec<F> compute (const sm::vvec<F>& x)
        {
            sm::vvec<F> y (x.size());
            for (std::uint32_t i = 0; i < x.size(); ++i) { y[i] = this->compute (x[i]); }
            return y;
        }
    };
}
