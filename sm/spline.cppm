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
#include <stdexcept>

export module sm.spline;

import sm.constexpr_math; // constexpr math functions from Keith O'Hara
export import sm.mathconst;
export import sm.vec;
export import sm.vvec;
export import sm.vmat;

export namespace sm
{
    // (Natural) cubic spline for N 2D coordinates in (floating point) type F. Computes the
    // coefficients for N - 1 cubic curves, so the result is 4 * (N - 1) coefficients.
    template<typename F>
    struct spline
    {
        spline() {}
        spline (const sm::vvec<sm::vec<F, 2>>& _p) { this->init (_p); }

        void init (const sm::vvec<sm::vec<F, 2>>& _p)
        {
            N = _p.size();
            p = _p;
            this->compute_coefficients();
        }

        // Initially there are N coordinates to interpolate between
        std::uint32_t N = 0;
        // Points
        sm::vvec<sm::vec<F, 2>> p = {};
        // Coefficients. c[0][3] to c[0][0] are the coefficients for cubic function 0. c[0][3] is
        // the coefficient of x^3
        sm::vvec<sm::vec<F, 4>> c; // will have size (N - 1)

        // Compute coefficients from points
        void compute_coefficients()
        {
            // A x = B
            sm::vmat<F> A(4 * (N - 1), 4 * (N - 1));
            sm::vvec<F> B(4 * (N - 1), F{0});

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
            sm::vmat<F> Aug (4 * (N - 1), 4 * (N - 1) + 1);
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
            sm::vvec<F> X = Aug.back_substitution();
            if (X.size() != 4 * (N - 1)) {
                throw std::runtime_error ("Unexpected size for X");
            }
            // Copy X into c.
            this->c.resize (N - 1);
            for (std::uint32_t i = 0; i < N - 1; ++i) {
                for (std::uint32_t j = 0; j < 4; ++j) { this->c[i][3 - j] = X[4 * i + j]; }
            }
        }

        F compute (const F x)
        {
            // Could/should check that c size is N - 1
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

    /*
     * Place n additional elements between each element in v, computing a cubic spline
     * interpolation. v must initially contain some elements to interpolate between.
     */
    template <typename T>
    void cubic_spline_expansion (sm::vvec<T>& v, std::uint32_t n)
    {
        std::uint32_t N = v.size();
        if (v.size() < 2) { throw std::runtime_error ("v.size < 2!"); }
        sm::vvec<sm::vec<T, 2>> p(N);
        for (std::uint32_t i = 0; i < N; ++i) { p[i] = { static_cast<T>(n) * i, v[i] }; }
        sm::spline<T> spl (p);
        const std::uint64_t newsz = v.size() + (v.size() - 1) * n;
        v.resize (newsz, T{0});
        // The amount ti has to increment depends on the output size (n * [N-1]):
        const T end = static_cast<T>(n) * (N - 1);
        const T tinc = end / (newsz - 1);
        T ti = T{0};
        for (std::uint32_t i = 0; i < v.size(); ++i, ti += tinc) { v[i] = spl.compute (ti); }
    }
}
