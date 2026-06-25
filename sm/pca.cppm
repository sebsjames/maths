// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * Principal component analysis
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
    enum class error_code : std::uint32_t
    {
        uncomputed,
        x_cols_unequal,
        x_empty,
        no_error
    };

    // Return struct for a principal component analysis with N dimensions of data
    template<typename T, std::uint32_t N> requires std::is_floating_point_v<T>
    struct result
    {
        // The length of each column of z
        std::uint32_t dsz = 0u;
        // The mean and standard deviation of each dimension of the input data
        sm::vec<sm::vvec<T>, N> mu_sig_x;
        // The input data, after it has been zero-shifted
        sm::vec<sm::vvec<T>, N> z;
        // The covariance matrix of the zero-shifted data
        sm::mat<T, N, N> covariance;
        // The principal component unit vectors
        sm::vec<sm::vec<T, N>, N> pc_vectors;
        // Principal component magnitudes (like the SD of the data in each dimension)
        sm::vec<T, N> pc_magnitudes = {};
        // Principal component proportions (of the variability each component accounts for)
        sm::vec<T, N> pc_proportions = {};
        // This holds the centred input data, projected onto the principal components
        sm::vec<sm::vvec<T>, N> x_proj;
        // If you need the projected data as a vvec of vecs, then call this
        sm::vvec<sm::vec<T, N>> get_x_proj()
        {
            sm::vvec<sm::vec<T, N>> xp (x_proj[0].size());
            for (std::uint32_t i = 0; i < x_proj[0].size(); ++i) {
                for (std::uint32_t j = 0; j < N; ++j) { xp[i][j] = x_proj[j][i]; }
            }
            return xp;
        }
        // A status or error code
        pca::error_code error = pca::error_code::uncomputed;
    };

    // Return covariance matrix for the data z, using the matrix multiplication x.T * X / (dsz-1)
    // If the arrays in z are not of the same length, return matrix containing the max possible value for type T
    template<typename T, std::uint32_t N> requires std::is_floating_point_v<T>
    sm::mat<T, N, N> covariance (const sm::vec<sm::vvec<T>, N>& z)
    {
        sm::mat<T, N, N> cm = {{}}; // Our covariance matrix
        std::uint32_t dsz = z[0].size();
        // Sanity check the column lengths
        if (dsz == 0u) {
            cm.set_from (std::numeric_limits<T>::max());
            return cm;
        }
        for (std::uint32_t i = 1; i < N; ++i) {
            if (dsz != z[i].size()) {
                cm.set_from (std::numeric_limits<T>::max());
                return cm;
            }
        }
        for (std::uint32_t r = 0; r < N; ++r) {
            for (std::uint32_t c = 0; c < N; ++c) {
                cm(r, c) = z[c].dot (z[r]) / (dsz - 1);
            }
        }
        return cm;
    }

    // Project each datum in res.z, and store the projected data back into the pca::result struct.
    template <typename T, std::uint32_t N> requires std::is_floating_point_v<T> && (N > 0)
    void transform (pca::result<T, N>& res)
    {
        if (res.error != pca::error_code::no_error) { return; }

        for (std::uint32_t i = 0; i < N; ++i) { res.x_proj[i].resize (res.dsz); }
        sm::vec<T, N> _z = {};
        for (std::uint32_t j = 0; j < res.dsz; ++j) { // for each datum...
            // Make the _z vec (for computing a dot product)
            for (std::uint32_t i = 0; i < N; ++i) { _z[i] = res.z[i][j]; }
            // Construct the projected datum by computing dot product for each dim
            for (std::uint32_t i = 0; i < N; ++i) {
                res.x_proj[i][j] = _z.dot (res.pc_vectors[i]);
            }
        }
    }

    /*
     * Perform Principal Component Analysis on N dimensions of data, storing results into the
     * return object.
     *
     * \tparam T the data type. In principle you could pass in an x with values in an integer type,
     * but the computation has to be performed in floating point, so the requires statement asks
     * that you do any conversion from int or similar into your preferred precision of floating
     * point.
     *
     * \tparam N The number of columns in the input data, x
     *
     * \param x The input data, provided as a fixed array of columns. All columns should have the
     * same length (this will be tested)
     */
    template <typename T, std::uint32_t N> requires std::is_floating_point_v<T> && (N > 0)
    pca::result<T, N> compute (const sm::vec<sm::vvec<T>, N>& x)
    {
        pca::result<T, N> rtn;

        // 0. Sanity check
        std::uint32_t dsz = x[0].size();
        if (dsz == 0) {
            rtn.error = pca::error_code::x_empty;
            return rtn;
        }
        for (std::uint32_t i = 1; i < N; ++i) {
            if (dsz != x[i].size()) {
                rtn.error = pca::error_code::x_cols_unequal;
                return rtn;
            }
        } // at end, we know all x[i] have same size, as required
        rtn.dsz = dsz;

        // 1. Compute mean and std then standardize (by shifting all data to be zero-centered)
        for (std::uint32_t i = 0; i < N; ++i) {
            rtn.mu_sig_x[i] = x[i].mean_std();    // We don't really need the standard deviation
            rtn.z[i] = x[i] - rtn.mu_sig_x[i][0]; // mean-shift
        }

        // 2. Calculate covariance matrix of the zero-centered data
        rtn.covariance = pca::covariance<T, N> (rtn.z);

        // 3. Compute Eigenvalues and vectors of the covariance matrix. Last element is largest magnitude
        sm::vec<typename sm::mat<T, N>::eigenpair, N> pairs = rtn.covariance.eigenpairs();

        // 4. Store principal component magnitudes and vectors into rtn, in descending order of
        // magnitude (PC1 has biggest eigenvalue). Also store the magnitudes as proportions.
        for (std::uint32_t ii = 0; ii < N; ++ii) {
            std::uint32_t i = N - ii - 1;
            rtn.pc_magnitudes[i] = std::sqrt (std::norm (pairs[ii].eigenvalue));
            for (std::uint32_t j = 0; j < N; ++j) {
                rtn.pc_vectors[i][j] = std::real (pairs[ii].eigenvector[j]);
            }
        }
        rtn.pc_proportions = rtn.pc_magnitudes / rtn.pc_magnitudes.sum();

        rtn.error = pca::error_code::no_error;
        return rtn;
    }

    // Perform Principal Component Analysis for a vvec of vec data
    template <typename T, std::uint32_t N> requires std::is_floating_point_v<T> && (N > 0)
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
