// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * Principle component analysis
 */
module;

#include <iostream>
#include <cstdint>
#include <type_traits>
#include <complex>

export module sm.pca;

import sm.vvec;
import sm.mat;

export namespace sm::pca
{
    template<typename T, std::uint32_t N> requires std::is_arithmetic_v<T>
    struct result
    {
        sm::vec<T, N> pc_mags = {};
        sm::vec<sm::vec<T, N>, N> pc_ev_real;
        sm::vec<sm::vvec<T>, N> z; // standardized data
        sm::vec<sm::vvec<T>, N> x_proj; // x projected onto principle components
    };

    // Return covariance matrix for the data z, using the matrix multiplication x.T * X / (dsz-1)
    template<typename T, std::uint32_t N> requires std::is_arithmetic_v<T>
    sm::mat<T, N, N> covariance (const sm::vec<sm::vvec<T>, N>& z)
    {
        std::uint32_t dsz = z[0].size();
        sm::mat<T, N, N> cm = {{}}; // Our covariance matrix
        for (std::uint32_t r = 0; r < N; ++r) {
            for (std::uint32_t c = 0; c < N; ++c) {
                cm.arr[r * N + c] = z[c].dot (z[r]) / (dsz - 1);
            }
        }
        return cm;
    }

    template <typename T, std::uint32_t N> requires std::is_arithmetic_v<T> && (N > 0)
    pca::result<T, N> compute (const sm::vec<sm::vvec<T>, N>& x)
    {
        pca::result<T, N> rtn;

        // 0. Sanity check
        std::uint32_t dsz = x[0].size();
        if (dsz == 0) { return rtn; }
        for (std::uint32_t i = 1; i < N; ++i) {
            if (dsz != x[i].size()) { return rtn; }
        } // at end, we know all x[i] have same size, as required

        // 1. Compute mean and std, normalize/standardize
        sm::vec<sm::vvec<T>, N> mu_sig_x; // maybe rtn.mu_sig_x
        sm::vec<sm::vvec<T>, N> z; // maybe rtn.z
        for (std::uint32_t i = 0; i < N; ++i) {
            mu_sig_x[i] = x[i].mean_std();
            z[i] = (x[i] - mu_sig_x[i][0]) / mu_sig_x[i][1];
        }

        // 2. Calculate covariance matrix of z.
        sm::mat<T, N, N> cm_z = pca::covariance<T, N> (z);

        // 3. Compute Eigenvalues and vectors of the covariance matrix. Last element is largest magnitude
        sm::vec<typename sm::mat<T, N>::eigenpair, N> pairs = cm_z.eigenpairs();


        // 4. Store principle component magnitudes and vectors into rtn, in descending order of magnitude (PC1 has biggest eigenvalue)
        T ev_sum = T{0};
        for (std::uint32_t i = 0; i < N; ++i) { ev_sum += std::norm(pairs[i].eigenvalue); }
        for (std::uint32_t ii = 0; ii < N; ++ii) {
            std::uint32_t i = N - ii - 1;
            rtn.pc_mags[i] = std::norm(pairs[ii].eigenvalue) / ev_sum;
            sm::vvec<T> ev_real (N, T{0});
            for (std::uint32_t j = 0; j < N; ++j) {
                ev_real[j] = std::real(pairs[ii].eigenvector[j]);
                rtn.pc_ev_real[i][j] = ev_real[j];
            }
        }

        // 5. Project each datum, and store the projected data in rtn
        for (std::uint32_t i = 0; i < N; ++i) { rtn.x_proj[i].resize (dsz); }
        sm::vec<T, N> _z = {};
        for (std::uint32_t j = 0; j < dsz; ++j) { // for each datum...
            // Make the _z vec (for computing a dot product)
            for (std::uint32_t i = 0; i < N; ++i) { _z[i] = z[i][j]; }
            // Construct the projected datum by computing dot product for each dim
            for (std::uint32_t i = 0; i < N; ++i) {
                rtn.x_proj[i][j] = _z.dot (rtn.pc_ev_real[i]);
            }
        }

        return rtn;
    }

    template <typename T, std::uint32_t N> requires std::is_arithmetic_v<T> && (N > 0)
    pca::result<T, N> compute (const sm::vvec<sm::vec<T, N>>& data)
    {
        // Each dimension of the vec is a feature. Repackage...
        sm::vec<sm::vvec<T>, N> x;
        for (std::uint32_t i = 0; i < N; ++i) { x[i].resize (data.size()); }
        for (std::uint32_t j = 0; j < data.size(); ++j) {
            for (std::uint32_t i = 0; i < N; ++i) { x[i][j] = data[j][i]; }
        }
        // ...and call the other overload
        return pca::compute<T, N> (x);
    }

} // namespace
