// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * Principle component analysis (up to 4D)
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
        // fill me in
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
        sm::vec<sm::vvec<T>, N> mu_sig_x;
        sm::vec<sm::vvec<T>, N> z;
        for (std::uint32_t i = 0; i < N; ++i) {
            std::cout << "x[" << i << "].std() = " << x[i].std() << std::endl;
            mu_sig_x[i] = x[i].mean_std();
            std::cout << "mu_sig_x["<< i << ": " << mu_sig_x[i] << std::endl;
            z[i] = (x[i] - mu_sig_x[i][0]) / mu_sig_x[i][1];
        }

        // 2. Calculate covariance matrix of both x AND z.
        [[maybe_unused]] sm::mat<T, N, N> cm_x = pca::covariance<T, N> (x);
        sm::mat<T, N, N> cm_z = pca::covariance<T, N> (z);
        std::cout << "Cov. matrix x:\n" << cm_x << std::endl;
        std::cout << "Cov. matrix z:\n" << cm_z << std::endl;

        // 3. Compute Eigenvalues and vectors of the covariance matrix
        sm::vec<typename sm::mat<T, N>::eigenpair, N> pairs = cm_z.eigenpairs();

        for (std::uint32_t i = 0; i < N; ++i) {
            std::cout << "pairs["<<i<<"].eigenvalue = " << pairs[i].eigenvalue.real() <<" + " << pairs[i].eigenvalue.imag() << "i" << std::endl;
            std::cout << "pairs["<<i<<"].eigenvector = " << pairs[i].eigenvector << std::endl;
        }

#if 0
        sm::vec<sm::vvec<T>, N> x_proj;
        for (std::uint32_t r = 0; r < N; ++r) {
            x_proj[r] = z[r].dot (eps[i].eigenvector);
        }
#endif

#if 0
        sm::vec<T, 2> pc1vec = m.row(0);
        T angle1 = pc1vec.angle();
        std::cout << "Angle of first component " << pc1vec << " is " << angle1 * sm::mathconst<T>::rad2deg << std::endl;
        sm::vec<T, 2> pc2vec = m.row(1);
        T angle2 = pc2vec.angle();
        std::cout << "Angle of 2nd component " << pc2vec << " is " << angle2 * sm::mathconst<T>::rad2deg << std::endl;
#endif

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
