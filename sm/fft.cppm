// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * Fast Fourier transform code.
 *
 * \date: 2026
 */

module;

#include <cstdint>
#include <complex>
#include <vector>
#include <utility>
#include <cmath>

export module sm.fft;

import sm.mathconst;
export import sm.vmat;

export namespace sm::fft
{
    /*!
     * In-place iterative radix-2 Cooley-Tukey FFT. invert selects the inverse transform
     * (which includes the 1/N normalisation). a.size() MUST be a power of two (or 0/1).
     */
    template<typename F>
    void fft_pow2 (std::vector<std::complex<F>>& a, bool invert)
    {
        const std::uint32_t n = a.size();
        if (n <= 1) { return; }

        for (std::uint32_t i = 1, j = 0; i < n; ++i) {
            std::uint32_t bit = n >> 1;
            for (; j & bit; bit >>= 1) { j ^= bit; }
            j ^= bit;
            if (i < j) { std::swap (a[i], a[j]); }
        }

        for (std::uint32_t len = 2; len <= n; len <<= 1) {
            F ang = (invert ? F{2} : F{-2}) * sm::mathconst<F>::pi / static_cast<F>(len);
            std::complex<F> wlen (std::cos (ang), std::sin (ang));
            for (std::uint32_t i = 0; i < n; i += len) {
                std::complex<F> w (F{1}, F{0});
                std::uint32_t half = len / 2u;
                for (std::uint32_t j = 0; j < half; ++j) {
                    std::complex<F> u = a[i + j];
                    std::complex<F> v = a[i + j + half] * w;
                    a[i + j] = u + v;
                    a[i + j + half] = u - v;
                    w *= wlen;
                }
            }
        }

        if (invert) { for (auto& x : a) { x /= static_cast<F>(n); } }
    }

    /*!
     * Bluestein's algorithm: a DFT/IDFT of arbitrary length, implemented via a power-of-two
     * FFT-based convolution. Used for any a.size() that is not itself a power of two.
     */
    template<typename F>
    void fft_bluestein (std::vector<std::complex<F>>& a, bool invert)
    {
        const std::uint32_t n = a.size();
        if (n <= 1) { return; }

        std::uint32_t m = 1;
        while (m < 2 * n + 1) { m <<= 1; }

        std::vector<std::complex<F>> exptab (n);
        for (std::uint32_t i = 0; i < n; ++i) {
            std::uint64_t j = (static_cast<std::uint64_t>(i) * static_cast<std::uint64_t>(i)) % (2ull * static_cast<std::uint64_t>(n));
            F ang = (invert ? F{1} : F{-1}) * sm::mathconst<F>::pi * static_cast<F>(j) / static_cast<F>(n);
            exptab[i] = std::complex<F> (std::cos (ang), std::sin (ang));
        }

        std::vector<std::complex<F>> av (m, std::complex<F>{F{0}, F{0}});
        for (std::uint32_t i = 0; i < n; ++i) { av[i] = a[i] * exptab[i]; }

        std::vector<std::complex<F>> bv (m, std::complex<F>{F{0}, F{0}});
        bv[0] = exptab[0];
        for (std::uint32_t i = 1; i < n; ++i) { bv[i] = bv[m - i] = std::conj (exptab[i]); }

        sm::fft::fft_pow2 (av, false);
        sm::fft::fft_pow2 (bv, false);
        for (std::uint32_t i = 0; i < m; ++i) { av[i] *= bv[i]; }
        sm::fft::fft_pow2 (av, true);

        for (std::uint32_t i = 0; i < n; ++i) { a[i] = av[i] * exptab[i]; }
        if (invert) {
            for (std::uint32_t i = 0; i < n; ++i) { a[i] /= static_cast<F>(n); }
        }
    }

    //! A DFT/IDFT of an arbitrary, non-zero length.
    template<typename F>
    void dft1d (std::vector<std::complex<F>>& a, bool invert)
    {
        std::uint32_t n = a.size();
        if (n <= 1) { return; }
        if ((n & (n - 1)) == 0) {
            sm::fft::fft_pow2 (a, invert);
        } else {
            sm::fft::fft_bluestein (a, invert);
        }
    }

    //! Transform each of mat's rows (a 1D signal of length mat.cols) independently.
    template<typename F>
    void dft_rows (sm::vmat<std::complex<F>>& mat, bool invert)
    {
        std::vector<std::complex<F>> buf (mat.cols());
        for (std::uint32_t r = 0; r < mat.rows(); ++r) {
            for (std::uint32_t c = 0; c < mat.cols(); ++c) { buf[c] = mat(r, c); }
            sm::fft::dft1d (buf, invert);
            for (std::uint32_t c = 0; c < mat.cols(); ++c) { mat(r, c) = buf[c]; }
        }
    }

    //! Transform each of mat's columns (a 1D signal of length mat.rows()) independently.
    template<typename F>
    void dft_cols (sm::vmat<std::complex<F>>& mat, bool invert)
    {
        std::vector<std::complex<F>> buf (mat.rows());
        for (std::uint32_t c = 0; c < mat.cols(); ++c) {
            for (std::uint32_t r = 0; r < mat.rows(); ++r) { buf[r] = mat(r, c); }
            sm::fft::dft1d (buf, invert);
            for (std::uint32_t r = 0; r < mat.rows(); ++r) { mat(r, c) = buf[r]; }
        }
    }

    //! Horizontally concatenate in with a same-sized block of zeros: (rows,cols) -> (rows,2*cols)
    template<typename F>
    sm::vmat<std::complex<F>> pad_zeros (const sm::vmat<std::complex<F>>& in)
    {
        sm::vmat<std::complex<F>> out (in.rows(), in.cols() * 2); // constructor ensures out is filled with 0
        for (std::uint32_t r = 0; r < in.rows(); ++r) {
            for (std::uint32_t c = 0; c < in.cols(); ++c) { out(r, c) = in(r, c); }
        }
        return out;
    }

    //! Take every other column of in, starting at column offset (0 or 1): (rows,2*cols) -> (rows,cols)
    template<typename F>
    sm::vmat<std::complex<F>> decimate_cols (const sm::vmat<std::complex<F>>& in, std::uint32_t offset)
    {
        std::uint32_t outcols = in.cols() / 2;
        sm::vmat<std::complex<F>> out (in.rows(), outcols);
        for (std::uint32_t r = 0; r < in.rows(); ++r) {
            for (std::uint32_t c = 0; c < outcols; ++c) { out(r, c) = in(r, offset + 2 * c); }
        }
        return out;
    }

    //! Add or subtract the bottom half of in's rows from the top half.
    template<typename F>
    sm::vmat<std::complex<F>> fold_half (const sm::vmat<std::complex<F>>& in, bool add)
    {
        std::uint32_t half = in.rows() / 2;
        sm::vmat<std::complex<F>> out (half, in.cols());
        for (std::uint32_t r = 0; r < half; ++r) {
            for (std::uint32_t c = 0; c < in.cols(); ++c) {
                out(r, c) = add ? (in(r, c) + in(r + half, c)) : (in(r, c) - in(r + half, c));
            }
        }
        return out;
    }

    //! Vertically stack two copies of in, on top of each other.
    template<typename F>
    sm::vmat<std::complex<F>> repmat2 (const sm::vmat<std::complex<F>>& in)
    {
        sm::vmat<std::complex<F>> out (in.rows() * 2, in.cols());
        for (std::uint32_t r = 0; r < in.rows(); ++r) {
            for (std::uint32_t c = 0; c < in.cols(); ++c) {
                out(r, c) = in(r, c);
                out(r + in.rows(), c) = in(r, c);
            }
        }
        return out;
    }
}
