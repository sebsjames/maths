// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * An implementation of the hexagonal fast Fourier transform decribed in Nick Rummelt's PhD thesis.
 *
 *
 * \author: AI took some code from Josua Grawitter in https://github.com/gwater/HexFFT.jl and made a
 * conversion to C++. This code generated an FFT in the twin rectangular 'ASA' arrays. Seb made
 * sense of this and wrote it onto a correctly arranged hex grid in the frequency space.
 *
 * \date: 2026
 */

module;

#include <memory>
#include <cstdint>
#include <complex>
#include <vector>
#include <utility>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <list>

#include <iostream>

export module sm.hexfft;

export import sm.hexgrid;
export import sm.vvec;
import sm.vec;
import sm.mathconst;
import sm.mat;
export import sm.vmat;

// Non-hexagonal Fast Fourier Transform code
export namespace sm::fft
{
    //! In-place iterative radix-2 Cooley-Tukey FFT. invert selects the inverse transform
    //! (which includes the 1/N normalisation). a.size() MUST be a power of two (or 0/1).
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

        if (invert) {
            for (auto& x : a) { x /= static_cast<F>(n); }
        }
    }

    //! Bluestein's algorithm: a DFT/IDFT of arbitrary length, implemented via a power-of-two
    //! FFT-based convolution. Used for any a.size() that is not itself a power of two.
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

namespace sm::hexfft::internal
{
    // 'Non standard function/transform 1' Fourier transform row by row, then decimate_cols.
    template<typename F>
    std::pair<sm::vmat<std::complex<F>>, sm::vmat<std::complex<F>>> nst1 (const sm::vmat<std::complex<F>>& data)
    {
        sm::vmat<std::complex<F>> padded = sm::fft::pad_zeros (data);
        sm::fft::dft_rows (padded, false);
        return { sm::fft::decimate_cols (padded, 0), sm::fft::decimate_cols (padded, 1) };
    }

    template<typename F>
    sm::vmat<std::complex<F>> nst2 (const sm::vmat<std::complex<F>>& data)
    {
        sm::vmat<std::complex<F>> folded = sm::fft::fold_half (data, true);
        sm::fft::dft_cols (folded, false);
        return sm::fft::repmat2 (folded);
    }

    template<typename F>
    sm::vmat<std::complex<F>> nst3 (const sm::vmat<std::complex<F>>& data)
    {
        std::uint32_t R = data.rows();
        sm::vmat<std::complex<F>> folded = sm::fft::fold_half (data, false);
        for (std::uint32_t r = 0; r < folded.rows(); ++r) {
            F ang = F{-2} * sm::mathconst<F>::pi * static_cast<F>(r) / static_cast<F>(R);
            std::complex<F> coeff (std::cos (ang), std::sin (ang));
            for (std::uint32_t c = 0; c < folded.cols(); ++c) { folded(r, c) *= coeff; }
        }
        sm::fft::dft_cols (folded, false);
        return sm::fft::repmat2 (folded);
    }

    template<typename F>
    sm::vmat<std::complex<F>> w_matrix (int b, std::uint32_t R, std::uint32_t C)
    {
        sm::vmat<std::complex<F>> out (R, C);
        const F _b = static_cast<F>(b);
        const F _R = static_cast<F>(R);
        const F _C = static_cast<F>(C);
        for (std::uint32_t s = 0; s < R; ++s) {
            const F _s = static_cast<F>(s);
            for (std::uint32_t d = 0; d < C; ++d) {
                const F _d = static_cast<F>(d);
                F ang = -sm::mathconst<F>::pi * ( (_b + F{2} * _d) / (F{2} * _C) + (_b + F{2} * _s) / _R );
                out(s, d) = std::complex<F> (std::cos (ang), std::sin (ang));
            }
        }
        return out;
    }

    //! The forward transform. data0 and data1 hold the two interleaved rectangular arrays (a=0 and a=1).
    template<typename F>
    std::pair<sm::vmat<std::complex<F>>, sm::vmat<std::complex<F>>> hfft2 (const sm::vmat<std::complex<F>>& data0, const sm::vmat<std::complex<F>>& data1)
    {
        std::uint32_t R = data0.rows();
        std::uint32_t C = data0.cols();

        // std::pair<sm::vmat<std::complex<F>>, sm::vmat<std::complex<F>>>, so g00, g01, g10, g11 are all cmat<F>
        auto [g00, g01] = nst1 (data0);
        auto [g10, g11] = nst1 (data1);

        sm::vmat<std::complex<F>> X0 = nst2 (g00);
        sm::vmat<std::complex<F>> t0 = nst2 (g10);
        sm::vmat<std::complex<F>> W0 = w_matrix<F> (0, R, C);
        for (std::uint32_t i = 0; i < X0.size(); ++i) {
            X0.arr[i] += W0.arr[i] * t0.arr[i];
        }

        sm::vmat<std::complex<F>> X1 = nst3 (g01);
        sm::vmat<std::complex<F>> t1 = nst3 (g11);
        sm::vmat<std::complex<F>> W1 = w_matrix<F> (1, R, C);
        for (std::uint32_t i = 0; i < X1.size(); ++i) {
            X1.arr[i] += W1.arr[i] * t1.arr[i];
        }

        return { X0, X1 };
    }

    template<typename F>
    std::pair<sm::vmat<std::complex<F>>, sm::vmat<std::complex<F>>> idft_inst1 (const sm::vmat<std::complex<F>>& data)
    {
        sm::vmat<std::complex<F>> padded = sm::fft::pad_zeros (data);
        sm::fft::dft_rows (padded, true);
        for (auto& v : padded.arr) { v *= F{2}; }
        return { sm::fft::decimate_cols (padded, 0), sm::fft::decimate_cols (padded, 1) };
    }

    template<typename F>
    sm::vmat<std::complex<F>> inst2 (const sm::vmat<std::complex<F>>& data)
    {
        sm::vmat<std::complex<F>> folded = sm::fft::fold_half (data, true);
        sm::fft::dft_cols (folded, true);
        for (auto& v : folded.arr) { v *= F{0.5}; }
        return sm::fft::repmat2 (folded);
    }

    template<typename F>
    sm::vmat<std::complex<F>> inst3 (const sm::vmat<std::complex<F>>& data)
    {
        std::uint32_t R = data.rows();
        sm::vmat<std::complex<F>> folded = sm::fft::fold_half (data, false);
        for (std::uint32_t r = 0; r < folded.rows(); ++r) {
            F ang = F{2} * sm::mathconst<F>::pi * static_cast<F>(r) / static_cast<F>(R);
            std::complex<F> coeff (std::cos (ang), std::sin (ang));
            for (std::uint32_t c = 0; c < folded.cols(); ++c) { folded(r, c) *= coeff; }
        }
        sm::fft::dft_cols (folded, true);
        for (auto& v : folded.arr) { v *= F{0.5}; }
        return sm::fft::repmat2 (folded);
    }

    template<typename F>
    sm::vmat<std::complex<F>> iw_matrix (int a, std::uint32_t R, std::uint32_t C)
    {
        sm::vmat<std::complex<F>> out (R, C);
        for (std::uint32_t r = 0; r < R; ++r) {
            for (std::uint32_t c = 0; c < C; ++c) {
                F ang = sm::mathconst<F>::pi * ( (static_cast<F>(a) + F{2} * static_cast<F>(c)) / (F{2} * static_cast<F>(C))
                                                + (static_cast<F>(a) + F{2} * static_cast<F>(r)) / static_cast<F>(R) );
                out(r, c) = std::complex<F> (std::cos (ang), std::sin (ang));
            }
        }
        return out;
    }

    //! The inverse transform, undoing hfft2.
    template<typename F>
    std::pair<sm::vmat<std::complex<F>>, sm::vmat<std::complex<F>>> ihfft2 (const sm::vmat<std::complex<F>>& X0, const sm::vmat<std::complex<F>>& X1)
    {
        std::uint32_t R = X0.rows();
        std::uint32_t C = X0.cols();

        auto [g00, g01] = idft_inst1 (X0);
        auto [g10, g11] = idft_inst1 (X1);

        sm::vmat<std::complex<F>> a0 = inst2 (g00);
        sm::vmat<std::complex<F>> t0 = inst2 (g10);
        sm::vmat<std::complex<F>> IW0 = iw_matrix<F> (0, R, C);
        sm::vmat<std::complex<F>> out0 (R, C);
        for (std::uint32_t i = 0; i < out0.size(); ++i) { out0.arr[i] = F{0.5} * (a0.arr[i] + IW0.arr[i] * t0.arr[i]); }

        sm::vmat<std::complex<F>> a1 = inst3 (g01);
        sm::vmat<std::complex<F>> t1 = inst3 (g11);
        sm::vmat<std::complex<F>> IW1 = iw_matrix<F> (1, R, C);
        sm::vmat<std::complex<F>> out1 (R, C);
        for (std::uint32_t i = 0; i < out1.size(); ++i) { out1.arr[i] = F{0.5} * (a1.arr[i] + IW1.arr[i] * t1.arr[i]); }

        return { out0, out1 };
    }

    sm::vec<std::int32_t, 2> asa_to_ri_gi (const std::uint32_t a, const std::uint32_t r, const std::uint32_t c,
                                           const std::int32_t ri_min, const std::int32_t gi_min)
    {
        const std::int32_t gi = r * 2 + a; // r (row) gives gi
        const std::int32_t ri = c - r;
        return sm::vec<std::int32_t, 2> { ri + ri_min, gi + gi_min };
    }

    /*!
     * Convert an sm::hex's ri/gi position *in the image/input* space into its index on one of the ASA rectangles.
     */
    sm::vec<std::uint32_t, 3> ri_gi_to_asa (const std::int32_t ri, const std::int32_t gi,
                                            const std::int32_t ri_min, const std::int32_t gi_min)
    {
        const std::int32_t _gi = (gi - gi_min) / 2;

        sm::vec<std::uint32_t, 3> arc = {};

        if ((_gi < 0) || ((ri - ri_min + _gi) < 0)) {
            arc.set_from (std::numeric_limits<std::uint32_t>::max());
        } else {
            arc = {
                static_cast<std::uint32_t> ((2 + (gi % 2)) % 2), // a from oddness/evenness of gi
                static_cast<std::uint32_t> (_gi),                // r is green axis only
                static_cast<std::uint32_t> (ri - ri_min + _gi )  // c is a combination of green axis and red axis
            };
        }

        return arc;
    }

    //! Find the smallest rectangle, in the (a, r, c) addressing of ri_gi_to_asa, that encloses
    //! every hex in hg, and round its row count up so that n (the number of rows in each of
    //! the two row-parity arrays) is even and at least 2, as required by fold_half.
    template<typename F>
    void bounding_box (const sm::hexgrid<F, sm::hexalign::point_up>& hg, std::int32_t& ri_min, std::int32_t& gi_min,
                       std::uint32_t& n, std::uint32_t& m)
    {
        if (hg.hexen.empty()) {
            throw std::runtime_error ("sm::hexfft: hexgrid has no hexes");
        }

        // hex::gi's parity/2 addressing (a, r) is unaffected by the column shear discussed in
        // ri_gi_to_asa, so gi_min, gi_max can be found directly.
        std::int32_t gi_max = 0;
        bool first = true;
        for (const auto& h : hg.hexen) {
            if (first) {
                gi_min = gi_max = h.gi;
                first = false;
            } else {
                gi_min = std::min (gi_min, h.gi);
                gi_max = std::max (gi_max, h.gi);
            }
        }

        // The column, c = hex::ri + r (see ri_gi_to_asa), does depend on r, so it needs a
        // second pass, now that gi_min (and hence every hex's r) is known.
        std::int32_t c_min = 0;
        std::int32_t c_max = 0;
        first = true;
        for (const auto& h : hg.hexen) {
            std::int32_t r = (h.gi - gi_min) / 2;
            std::int32_t c = h.ri + r;
            if (first) {
                c_min = c_max = c;
                first = false;
            } else {
                c_min = std::min (c_min, c);
                c_max = std::max (c_max, c);
            }
        }
        ri_min = c_min;

        m = static_cast<std::uint32_t> (c_max - c_min + 1);

        std::uint32_t rows = static_cast<std::uint32_t> (gi_max - gi_min + 1);
        n = (rows + 1u) / 2u;
        if (n < 2u) {
            n = 2u;
        } else if ((n % 2u) != 0u) {
            ++n;
        }
    }

    template<typename F>
    std::pair<sm::vmat<std::complex<F>>, sm::vmat<std::complex<F>>> image_hexgrid_to_asa (const sm::hexgrid<F, sm::hexalign::point_up>& hg, const sm::vvec<std::complex<F>>& data,
                                                                                          std::int32_t ri_min, std::int32_t gi_min,
                                                                                          std::uint32_t rows, std::uint32_t cols)
    {
        if (data.size() != hg.num()) {
            std::stringstream ee;
            ee << "sm::hexfft: data.size() (" << data.size() << ") does not match hg.num() (" << hg.num() << ")";
            throw std::runtime_error (ee.str());
        }
        sm::vmat<std::complex<F>> d0 (rows, cols);
        sm::vmat<std::complex<F>> d1 (rows, cols);
        sm::vec<std::uint32_t> arc = {};
        for (const auto& h : hg.hexen) {
            arc = ri_gi_to_asa (h.ri, h.gi, ri_min, gi_min);
            if (arc[0] == 0u) {
                d0 (arc[1], arc[2]) = data[h.vi];
            } else {
                d1 (arc[1], arc[2]) = data[h.vi];
            }
        }
        return { d0, d1 };
    }

    sm::vec<std::int32_t, 3> ns_to_rgb (const std::uint32_t n1, const std::uint32_t n2,
                                        const std::int32_t rows, const std::int32_t cols)
    {
        // FIXME based on actual transform!
        std::int32_t ri_offs = -cols / 2;
        std::int32_t gi_offs = -rows / 2;
        sm::vec<std::int32_t, 3> rgb = { ri_offs + static_cast<std::int32_t>(n1), gi_offs + static_cast<std::int32_t>(n2), 0 };
        return rgb;
    }

    template<typename F>
    sm::vvec<std::complex<F>> d_asa_to_image_hexgrid (const sm::hexgrid<F, sm::hexalign::point_up>& hg,
                                                      const sm::vmat<std::complex<F>>& d0, const sm::vmat<std::complex<F>>& d1)
    {
        sm::vvec<std::complex<F>> out (hg.num(), std::complex<F>{0,0});

        // d0 is (0, r, c)
        for (std::uint32_t i = 0; i < d0.size(); ++i) {
            const std::uint32_t r = i % d0.rows();
            const std::uint32_t c = i / d0.rows();

            auto n1 = 0 + r + c;
            auto n2 = 0 + 2 * r;
            sm::vec<std::int32_t, 3> rgb = ns_to_rgb (n1, n2, d0.rows(), d0.cols());
            // Find the vi index for n1, n2
            auto hi = hg.find_hex_at (rgb);
            if (hi->vi < out.size()) { out[hi->vi] = d0.arr[i]; }
        }
        // d1 is (1, r, c)
        for (std::uint32_t i = 0; i < d1.size(); ++i) {
            const std::uint32_t r = i % d1.rows();
            const std::uint32_t c = i / d1.rows();

            auto n1 = 1 + r + c;
            auto n2 = 1 + 2 * r;
            sm::vec<std::int32_t, 3> rgb = ns_to_rgb (n1, n2, d1.rows(), d1.cols());

            // Find the vi index for k1, k2
            auto hi = hg.find_hex_at (rgb);
            if (hi->vi < out.size()) { out[hi->vi] = d1.arr[i]; }
        }

        return out;
    }

    // Take the k1, k2 tile coordinates from freq space and make it into rgb coordinates for the hexgrid
    sm::vec<std::int32_t, 3> ks_to_rgb (const std::uint32_t k1, const std::uint32_t k2,
                                        const std::int32_t rows, const std::int32_t cols)
    {
        //std::cout << __func__ << " called for ks: " << k1 << ", " << k2;
        std::int32_t ri_offs = -rows / 2;
        std::int32_t gi_offs = -cols / 2;
        std::int32_t bi_offs = rows / 2;
        sm::vec<std::int32_t, 3> rgb = { ri_offs, gi_offs + static_cast<std::int32_t>(k1), bi_offs - static_cast<std::int32_t>(k2) };
        //std::cout << " generates rgb = " << rgb << std::endl;
        return rgb;
    }

    // Convert from rgb coords on the frequency hexgrid to k1, k2 tile coordinates
    sm::vec<std::uint32_t, 3> rgb_to_ks ([[maybe_unused]] const std::int32_t r, const std::int32_t g, const std::int32_t b,
                                         const std::int32_t rows, const std::int32_t cols)
    {
        //std::cout << __func__ << " called for rgb: " << r << ", " << g << ", " << b;
        const std::int32_t gi_offs = -cols / 2;
        const std::int32_t bi_offs = rows / 2;
        sm::vec<std::uint32_t, 3> ks = { std::numeric_limits<std::int32_t>::max() };
        std::int32_t _g = g - gi_offs;
        std::int32_t _b = bi_offs - b;
        if (_g < 0) { _g = 0; }
        if (_b < 0) { _b = 0; }

        std::uint32_t arr = 0u;
        if ((2 + (g % 2)) % 2 == 0) {
            arr = 1u;
        } // else arr remains 1

        ks = { static_cast<std::uint32_t>(_g) , static_cast<std::uint32_t>(_b), arr };
        //std::cout << " generating ks = " << ks << std::endl;

        return ks;
    }

    // X0 is the array (0, s, d) and X1 is (1, s, d). Transfer these to a hexgrid in the freq. space.
    template<typename F>
    sm::vvec<std::complex<F>> X_asa_to_frequency_hexgrid (const sm::hexgrid<F, sm::hexalign::flat_up>* hgf,
                                                          const sm::vmat<std::complex<F>>& X0, const sm::vmat<std::complex<F>>& X1)
    {
        sm::vvec<std::complex<F>> out (hgf->num(), std::complex<F>{0,0});

        // X0 is (0, s, d)
        for (std::uint32_t i = 0; i < X0.size(); ++i) {
            const std::uint32_t r = i % X0.rows();
            const std::uint32_t c = i / X0.rows();

            auto k1 = 0 + r + c;
            auto k2 = 0 + 2 * r;
            sm::vec<std::int32_t, 3> rgb = ks_to_rgb (k1, k2, X0.rows(), X0.cols());
            // Find the vi index for k1, k2
            auto hi = hgf->find_hex_at (rgb);
            if (hi->vi < out.size()) { out[hi->vi] = X0.arr[i]; }
        }
        // X1 is (1, s, d)
        for (std::uint32_t i = 0; i < X1.size(); ++i) {
            const std::uint32_t r = i % X0.rows();
            const std::uint32_t c = i / X0.rows();

            auto k1 = 1 + r + c;
            auto k2 = 1 + 2 * r;
            sm::vec<std::int32_t, 3> rgb = ks_to_rgb (k1, k2, X1.rows(), X1.cols());

            // Find the vi index for k1, k2
            auto hi = hgf->find_hex_at (rgb);
            if (hi->vi < out.size()) { out[hi->vi] = X1.arr[i]; }
        }

        return out;
    }

    template<typename F>
    std::pair<sm::vmat<std::complex<F>>, sm::vmat<std::complex<F>>> frequency_hexgrid_to_X_asa (const sm::hexgrid<F, sm::hexalign::flat_up>* hgf,
                                                                                                const sm::vvec<std::complex<F>>& hex_data,
                                                                                                std::uint32_t rows, std::uint32_t cols)
    {
        if (hex_data.size() != hgf->num()) {
            std::stringstream ee;
            ee << "sm::hexfft: hex_data.size() (" << hex_data.size() << ") does not match hgf->num() (" << hgf->num() << ")";
            throw std::runtime_error (ee.str());
        }
        sm::vmat<std::complex<F>> X0 (rows, cols);
        sm::vmat<std::complex<F>> X1 (rows, cols);
        sm::vec<std::uint32_t> arc = {};
        for (const auto& h : hgf->hexen) {
            sm::vec<std::uint32_t, 3> ks = rgb_to_ks (h.ri, h.gi, h.bi, rows, cols);

            std::uint32_t a = ks[2]; // how to determine?
            arc[0] = a;
            arc[1] = (ks[1] - a) / 2;
            arc[2] = ks[0] - (ks[1] - a) / 2;
            std::cout << "(arc[1], arc[2]) = " << arc[1] << ", " << arc[2] << std::endl;
            if (arc[0] == 0u) {
                X0 (arc[1], arc[2]) = hex_data[h.vi];
            } else {
                X1 (arc[1], arc[2]) = hex_data[h.vi];
            }
        }
        return { X0, X1 };
    }

} // sm::hexfft::internal

export namespace sm::hexfft
{
    template<typename F>
    constexpr sm::mat<F, 2, 2> make_V()
    {
        sm::mat<F, 2, 2> V;
        V.set_col(0, { F{1}, F{0} });
        V.set_col(1, { F{0.5}, sm::mathconst<F>::root_3_over_2 });
        return V;
    }

    template<typename F>
    constexpr sm::mat<F, 2, 2> make_U (const sm::mat<F, 2, 2>& V)
    {
        sm::mat<F, 2, 2> U = V.transpose().inverse();

        // Swap cols of U (but why? Because freq space would end up being left-handed?)
        auto tmp0 = U.col(0);
        U.set_col(0, U.col(1));
        U.set_col(1, tmp0);

        return U;
    }

    template<typename F>
    constexpr sm::mat<F, 2, 2> make_U()
    {
        // Unit vectors in image space combined into a matrix
        constexpr sm::mat<F, 2, 2> V = make_V<F>();
        return make_U (V);
    }

    /*!
     * The result of a forward hexagonal FFT (sm::hexfft::fft).
     *
     * A hexgrid's boundary can be any shape, and so need not form a rectangle in (hex::ri,
     * hex::gi) index space, but Birdsong & Rummelt's algorithm requires one. sm::hexfft::fft
     * therefore computes the transform over the smallest rectangle enclosing all of the
     * hexgrid's hexes, treating any (ri, gi) that falls inside that rectangle but outside the
     * hexgrid's boundary as a zero input sample (an ordinary zero-padded/windowed FFT).
     */
    template<typename F = double>
    struct spectrum
    {
        //! Rows in each of the two ASA grids.
        std::uint32_t rows = 0;
        //! Columns in the ASA grids.
        std::uint32_t cols = 0;
        //! The hex::ri, hex::gi of the padded rectangle's (a=0, r=0, c=0) corner.
        std::int32_t ri_min = 0;
        std::int32_t gi_min = 0;

        //! This holds the data in the twin rectangular grids (ASA: array set addressing grids)
        std::pair<sm::vmat<std::complex<F>>, sm::vmat<std::complex<F>>> d_asa; // first: even, second: odd
        //! FFT in twin rectangular ASA grids. Saved to enable plotting/debugging
        std::pair<sm::vmat<std::complex<F>>, sm::vmat<std::complex<F>>> X_asa;

        //! We construct a frequency hexgrid from the image hexgrid.
        std::unique_ptr<sm::hexgrid<F, sm::hexalign::flat_up>> hgf;
        //! Scaling factor (obtained from image data hexgrid spacing)
        F Uscale = F{1};
        //! Result, suitable for visualization on the frequency space hexgrid
        sm::vvec<std::complex<F>> hex_data;

        //! The total number of samples in the padded rectangle (2 * r * c).
        std::uint32_t size() const { return 2u * this->rows * this->cols; }

        // Re-arrange X_asa so that it is in a human-readable arrangement
        void re_quadrant()
        {
            // cmat has rows, cols and vvec<> data
            if (X_asa.second.cols() % 2) {
                std::cout << "cols not divisible by 2\n";
                return;
            }
            if (X_asa.second.rows() % 2) {
                std::cout << "rows not divisible by 2\n";
                return;
            }

            if ((X_asa.first.rows() != X_asa.second.rows()) || (X_asa.first.cols() != X_asa.second.cols())) {
                std::cerr << "re_quadrant: Size mismatch, returning without changing anything\n";
                return;
            }

            sm::vmat<std::complex<F>> X0 (X_asa.first.rows(), X_asa.first.cols());
            sm::vmat<std::complex<F>> X1 (X_asa.second.rows(), X_asa.second.cols());

            std::uint32_t hcols = X_asa.second.cols() / 2;
            std::uint32_t hrows = X_asa.second.rows() / 2;

            for (std::uint32_t c = 0; c < hcols; c++) {
                for (std::uint32_t r = 0; r < hrows; r++) {
                    X0(r + hrows, c + hcols) = X_asa.first(r, c);
                    X1(r + hrows, c + hcols) = X_asa.second(r, c);
                    X0(r, c) = X_asa.first(r, c + hcols);
                    X1(r, c) = X_asa.second(r, c + hcols);
                    X0(r + hrows, c) = X_asa.first(r + hrows, c + hcols);
                    X1(r + hrows, c) = X_asa.second(r + hrows, c + hcols);
                    X0(r, c + hcols) = X_asa.first(r + hrows, c);
                    X1(r, c + hcols) = X_asa.second(r + hrows, c);
                }
            }
            X_asa.first = X0;
            X_asa.second = X1;
        }

        // Reverse of re_quadrant
        // Comparing with re_quadrant, I just swapped didx and sidx. Easy.
        void de_quadrant()
        {
            if (X_asa.second.cols() % 2) {
                std::cout << "cols not divisible by 2\n";
                return;
            }
            if (X_asa.second.rows() % 2) {
                std::cout << "rows not divisible by 2\n";
                return;
            }

            if ((X_asa.first.rows() != X_asa.second.rows()) || (X_asa.first.cols() != X_asa.second.cols())) {
                std::cerr << "de_quadrant: Size mismatch, returning without changing anything\n";
                return;
            }

            sm::vmat<std::complex<F>> X0 (X_asa.first.rows(), X_asa.first.cols());
            sm::vmat<std::complex<F>> X1 (X_asa.second.rows(), X_asa.second.cols());

            std::uint32_t hcols = X_asa.second.cols() / 2;
            std::uint32_t hrows = X_asa.second.rows() / 2;

            for (std::uint32_t c = 0; c < hcols; c++) {
                for (std::uint32_t r = 0; r < hrows; r++) {
                    X0(r, c) = X_asa.first(r + hrows, c + hcols);
                    X1(r, c) = X_asa.second(r + hrows, c + hcols);
                    X0(r, c + hcols) = X_asa.first(r, c);
                    X1(r, c + hcols) = X_asa.second(r, c);
                    X0(r + hrows, c + hcols) = X_asa.first(r + hrows, c);
                    X1(r + hrows, c + hcols) = X_asa.second(r + hrows, c);
                    X0(r + hrows, c) = X_asa.first(r, c + hcols);
                    X1(r + hrows, c) = X_asa.second(r, c + hcols);
                }
            }
            X_asa.first = X0;
            X_asa.second = X1;
        }
    };

    /*!
     * Compute the hexagonal FFT of data, sampled on the (possibly arbitrarily bounded)
     * hexgrid hg. data must be indexed by hex::vi, as usual for hexgrid client data (so
     * data.size() == hg.num()).
     */
    template<typename F>
    spectrum<F> fft (const sm::hexgrid<F>& hg, const sm::vvec<std::complex<F>>& data)
    {
        spectrum<F> result;
        internal::bounding_box (hg, result.ri_min, result.gi_min, result.rows, result.cols);
        std::cout << "ri_min: " << result.ri_min << ", gi_min: " << result.gi_min
                  << " rows x cols: " << result.rows << " x " <<  result.cols << std::endl;

        // Save the input data after it has been extracted into ASA format
        result.d_asa = internal::image_hexgrid_to_asa (hg, data, result.ri_min, result.gi_min, result.rows, result.cols);
        // Fourier transform the ASA formatted data into an ASA formatted result (result.X_asa)
        result.X_asa = internal::hfft2 (result.d_asa.first, result.d_asa.second);
        // Re-quadrant X_asa before putting it on hexgrid
        result.re_quadrant();
        // Can also reverse: result.de_quadrant();

        // Construct a hexgrid
        auto V = sm::hexfft::make_V<float>();
        V *= hg.d;
        const sm::mat<F, 2, 2> U = sm::hexfft::make_U<F>(V);
        result.Uscale = V.col(0).length() * V.col(0).length(); // a suitable scaling (zoom factor) for the frequency hexgrid
        result.hgf = std::make_unique<sm::hexgrid<F, sm::hexalign::flat_up>>(U.col(0).length(), result.cols * 2 * U.col(0).length(), 0.0f);
        result.hgf->set_rectangular_boundary (result.cols * U.col(0).length(), result.cols * U.col(0).length());

        // Populated a frequency space hexgrid with result.X_asa
        result.hex_data = internal::X_asa_to_frequency_hexgrid (result.hgf.get(), result.X_asa.first, result.X_asa.second);

        return result; // std::move?
    }

    //! As above, but for real-valued input data.
    template<typename F>
    spectrum<F> fft (const sm::hexgrid<F>& hg, const sm::vvec<F>& data)
    {
        sm::vvec<std::complex<F>> cdata (data.size());
        for (std::uint32_t i = 0; i < data.size(); ++i) { cdata[i] = std::complex<F> (data[i], F{0}); }
        return sm::hexfft::fft (hg, cdata);
    }

    // Inverse FFT, starting from the hex_data in spectrum X
    template<typename F>
    sm::vvec<std::complex<F>> ifft (const sm::hexgrid<F>& hg, spectrum<F>& X)
    {
        std::cout << "Setting zero...\n";
        X.X_asa.first.set_zero();
        X.X_asa.second.set_zero();
        X.d_asa.first.set_zero();
        X.d_asa.second.set_zero();

        // 1. From X.hex_data, construct X.data or x0 and x1.
        X.X_asa = internal::frequency_hexgrid_to_X_asa (X.hgf.get(), X.hex_data, X.rows, X.cols);
        std::cout << "ifft: X.X_asa.first.size: " << X.X_asa.first.size() << std::endl;

        // Switch X into the quadranted data that the hfft2/ihfft2 functions work in
        //X.de_quadrant();

        // 2.
        X.d_asa = internal::ihfft2 (X.X_asa.first, X.X_asa.second);

        return internal::d_asa_to_image_hexgrid<F> (hg, X.d_asa.first, X.d_asa.second);
    }

} // sm::hexfft
