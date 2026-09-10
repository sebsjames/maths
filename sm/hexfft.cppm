// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * An implementation of the hexagonal fast Fourier transform of:
 *
 *   J. B. Birdsong and N. I. Rummelt, "The hexagonal fast Fourier transform,"
 *   2016 IEEE International Conference on Image Processing (ICIP), 2016.
 *
 * The algorithm computes the DFT of data sampled on a hexagonal lattice by
 * splitting the lattice into two interleaved rectangular arrays (one for each
 * of the two possible row-offsets that occur in a hex-packed row of samples)
 * and re-uses an ordinary 1D FFT to do the heavy lifting.
 *
 * sm::hexgrid supplies the physical hex lattice, including its axial hex::ri, hex::gi
 * addressing. hex::gi's parity picks out which of the two interleaved rectangular arrays a
 * given hex belongs to; hex::ri and hex::gi/2 give that array's column and row. Since the
 * hexgrid's boundary may be any shape at all, and need not form a rectangle in (ri, gi)
 * space, the transform is actually computed over the smallest enclosing rectangle, with any
 * (ri, gi) not present in the hexgrid treated as zero (an ordinary zero-padded/windowed FFT).
 *
 * See sm::hexfft::spectrum for how that rectangle relates back to the hexgrid.
 *
 * claude --resume c364f967-91ac-4ab9-9422-42b410ef007c
 *
 * \author: AI, based in part on work by Josua Grawitter in https://github.com/gwater/HexFFT.jl and
 * on the Wikipedia page. Data visualization and bug identification by Seb James.
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

// Regular, non-hexagonal fft
export namespace sm::fft
{
    // What alignment for the frequency space fft?
    constexpr sm::hexalign hgf_align = sm::hexalign::flat_up;

    /*!
     * A minimal row-major complex matrix. This is an intermediate data type that is used during the
     * hexagonal FFT computation. it is exported so that the data can be inspected.
     */
    template<typename F>
    struct cmat
    {
        std::uint32_t size() const { return this->rows * this->cols; }
        std::uint32_t rows = 0;
        std::uint32_t cols = 0;
        sm::vvec<std::complex<F>> data;

        cmat() = default;
        cmat (std::uint32_t r, std::uint32_t c) : rows(r), cols(c), data (r * c, std::complex<F>{F{0}, F{0}}) {}

        std::complex<F>& operator() (std::uint32_t r, std::uint32_t c) { return this->data[r * this->cols + c]; }
        const std::complex<F>& operator() (std::uint32_t r, std::uint32_t c) const { return this->data[r * this->cols + c]; }
    };

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
                std::uint32_t half = len / 2;
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

        fft_pow2 (av, false);
        fft_pow2 (bv, false);
        for (std::uint32_t i = 0; i < m; ++i) { av[i] *= bv[i]; }
        fft_pow2 (av, true);

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
            fft_pow2 (a, invert);
        } else {
            fft_bluestein (a, invert);
        }
    }

    //! Transform each of mat's rows (a 1D signal of length mat.cols) independently.
    template<typename F>
    void dft_rows (sm::fft::cmat<F>& mat, bool invert)
    {
        std::vector<std::complex<F>> buf (mat.cols);
        for (std::uint32_t r = 0; r < mat.rows; ++r) {
            for (std::uint32_t c = 0; c < mat.cols; ++c) { buf[c] = mat(r, c); }
            dft1d (buf, invert);
            for (std::uint32_t c = 0; c < mat.cols; ++c) { mat(r, c) = buf[c]; }
        }
    }

    //! Transform each of mat's columns (a 1D signal of length mat.rows) independently.
    template<typename F>
    void dft_cols (sm::fft::cmat<F>& mat, bool invert)
    {
        std::vector<std::complex<F>> buf (mat.rows);
        for (std::uint32_t c = 0; c < mat.cols; ++c) {
            for (std::uint32_t r = 0; r < mat.rows; ++r) { buf[r] = mat(r, c); }
            dft1d (buf, invert);
            for (std::uint32_t r = 0; r < mat.rows; ++r) { mat(r, c) = buf[r]; }
        }
    }

    //! Horizontally concatenate in with a same-sized block of zeros: (rows,cols) -> (rows,2*cols)
    template<typename F>
    sm::fft::cmat<F> pad_zeros (const sm::fft::cmat<F>& in)
    {
        sm::fft::cmat<F> out (in.rows, in.cols * 2); // constructor ensures out is filled with 0
        for (std::uint32_t r = 0; r < in.rows; ++r) {
            for (std::uint32_t c = 0; c < in.cols; ++c) { out(r, c) = in(r, c); }
        }
        return out;
    }

    //! Take every other column of in, starting at column offset (0 or 1): (rows,2*cols) -> (rows,cols)
    template<typename F>
    sm::fft::cmat<F> decimate_cols (const sm::fft::cmat<F>& in, std::uint32_t offset)
    {
        std::uint32_t outcols = in.cols / 2;
        sm::fft::cmat<F> out (in.rows, outcols);
        for (std::uint32_t r = 0; r < in.rows; ++r) {
            for (std::uint32_t c = 0; c < outcols; ++c) { out(r, c) = in(r, offset + 2 * c); }
        }
        return out;
    }

    //! Add or subtract the bottom half of in's rows from the top half.
    template<typename F>
    sm::fft::cmat<F> fold_half (const sm::fft::cmat<F>& in, bool add)
    {
        std::uint32_t half = in.rows / 2;
        sm::fft::cmat<F> out (half, in.cols);
        for (std::uint32_t r = 0; r < half; ++r) {
            for (std::uint32_t c = 0; c < in.cols; ++c) {
                out(r, c) = add ? (in(r, c) + in(r + half, c)) : (in(r, c) - in(r + half, c));
            }
        }
        return out;
    }

    //! Vertically stack two copies of in, on top of each other.
    template<typename F>
    sm::fft::cmat<F> repmat2 (const sm::fft::cmat<F>& in)
    {
        sm::fft::cmat<F> out (in.rows * 2, in.cols);
        for (std::uint32_t r = 0; r < in.rows; ++r) {
            for (std::uint32_t c = 0; c < in.cols; ++c) {
                out(r, c) = in(r, c);
                out(r + in.rows, c) = in(r, c);
            }
        }
        return out;
    }

    //! Circularly shift in's rows by shift_r and columns by shift_c: the value at (r, c) in in
    //! moves to ((r+shift_r) % in.rows, (c+shift_c) % in.cols) in the result.
    template<typename F>
    sm::fft::cmat<F> roll (const sm::fft::cmat<F>& in, std::uint32_t shift_r, std::uint32_t shift_c)
    {
        sm::fft::cmat<F> out (in.rows, in.cols);
        for (std::uint32_t r = 0; r < in.rows; ++r) {
            std::uint32_t sr = (r + shift_r) % in.rows;
            for (std::uint32_t c = 0; c < in.cols; ++c) {
                std::uint32_t sc = (c + shift_c) % in.cols;
                out (sr, sc) = in (r, c);
            }
        }
        return out;
    }

#if 0 // Not right
    //! Move the zero-frequency (DC) bin, at (0,0), to the middle of in, matching the
    //! numpy/MATLAB fftshift convention. Because in.rows is always even here, this is its own
    //! exact inverse in the row direction; in.cols need not be even, so use ifftshift, not
    //! fftshift again, to invert this exactly when in.cols is odd.
    template<typename F>
    sm::fft::cmat<F> fftshift (const sm::fft::cmat<F>& in)
    {
        return roll (in, in.rows / 2u, in.cols / 2u);
    }

    //! The exact inverse of fftshift: moves the bin at the middle of in (where fftshift put
    //! the DC bin) back to (0,0).
    template<typename F>
    sm::fft::cmat<F> ifftshift (const sm::fft::cmat<F>& in)
    {
        return roll (in, in.rows - in.rows / 2u, in.cols - in.cols / 2u);
    }
#endif
}

namespace sm::hexfft::internal
{
    // The remaining functions follow the naming used by Birdsong & Rummelt (and by
    // gwater/HexFFT.jl, a reference implementation of the same algorithm). data has shape
    // (R, COLS), where R (== grid::n) MUST be even.

    // Fourier transform row by row, then decimate_cols.
    template<typename F>
    std::pair<sm::fft::cmat<F>, sm::fft::cmat<F>> nst1 (const sm::fft::cmat<F>& data)
    {
        sm::fft::cmat<F> padded = sm::fft::pad_zeros (data);
        sm::fft::dft_rows (padded, false);
        return { sm::fft::decimate_cols (padded, 0), sm::fft::decimate_cols (padded, 1) };
    }

    template<typename F>
    sm::fft::cmat<F> nst2 (const sm::fft::cmat<F>& data)
    {
        sm::fft::cmat<F> folded = sm::fft::fold_half (data, true);
        sm::fft::dft_cols (folded, false);
        return sm::fft::repmat2 (folded);
    }

    template<typename F>
    sm::fft::cmat<F> nst3 (const sm::fft::cmat<F>& data)
    {
        std::uint32_t R = data.rows;
        sm::fft::cmat<F> folded = sm::fft::fold_half (data, false);
        for (std::uint32_t r = 0; r < folded.rows; ++r) {
            F ang = F{-2} * sm::mathconst<F>::pi * static_cast<F>(r) / static_cast<F>(R);
            std::complex<F> coeff (std::cos (ang), std::sin (ang));
            for (std::uint32_t c = 0; c < folded.cols; ++c) { folded(r, c) *= coeff; }
        }
        sm::fft::dft_cols (folded, false);
        return sm::fft::repmat2 (folded);
    }

    template<typename F>
    sm::fft::cmat<F> w_matrix (int b, std::uint32_t R, std::uint32_t C)
    {
        sm::fft::cmat<F> out (R, C);
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
    std::pair<sm::fft::cmat<F>, sm::fft::cmat<F>> hfft2 (const sm::fft::cmat<F>& data0, const sm::fft::cmat<F>& data1)
    {
        std::uint32_t R = data0.rows;
        std::uint32_t C = data0.cols;

        // std::pair<sm::fft::cmat<F>, sm::fft::cmat<F>>, so g00, g01, g10, g11 are all cmat<F>
        auto [g00, g01] = nst1 (data0);
        auto [g10, g11] = nst1 (data1);

        sm::fft::cmat<F> X0 = nst2 (g00);
        sm::fft::cmat<F> t0 = nst2 (g10);
        sm::fft::cmat<F> W0 = w_matrix<F> (0, R, C);
        for (std::uint32_t i = 0; i < X0.data.size(); ++i) {
            X0.data[i] += W0.data[i] * t0.data[i];
        }

        sm::fft::cmat<F> X1 = nst3 (g01);
        sm::fft::cmat<F> t1 = nst3 (g11);
        sm::fft::cmat<F> W1 = w_matrix<F> (1, R, C);
        for (std::uint32_t i = 0; i < X1.data.size(); ++i) {
            X1.data[i] += W1.data[i] * t1.data[i];
        }

        return { X0, X1 };
    }

    template<typename F>
    std::pair<sm::fft::cmat<F>, sm::fft::cmat<F>> idft_inst1 (const sm::fft::cmat<F>& data)
    {
        sm::fft::cmat<F> padded = sm::fft::pad_zeros (data);
        sm::fft::dft_rows (padded, true);
        for (auto& v : padded.data) { v *= F{2}; }
        return { sm::fft::decimate_cols (padded, 0), sm::fft::decimate_cols (padded, 1) };
    }

    template<typename F>
    sm::fft::cmat<F> inst2 (const sm::fft::cmat<F>& data)
    {
        sm::fft::cmat<F> folded = sm::fft::fold_half (data, true);
        sm::fft::dft_cols (folded, true);
        for (auto& v : folded.data) { v *= F{0.5}; }
        return sm::fft::repmat2 (folded);
    }

    template<typename F>
    sm::fft::cmat<F> inst3 (const sm::fft::cmat<F>& data)
    {
        std::uint32_t R = data.rows;
        sm::fft::cmat<F> folded = sm::fft::fold_half (data, false);
        for (std::uint32_t r = 0; r < folded.rows; ++r) {
            F ang = F{2} * sm::mathconst<F>::pi * static_cast<F>(r) / static_cast<F>(R);
            std::complex<F> coeff (std::cos (ang), std::sin (ang));
            for (std::uint32_t c = 0; c < folded.cols; ++c) { folded(r, c) *= coeff; }
        }
        sm::fft::dft_cols (folded, true);
        for (auto& v : folded.data) { v *= F{0.5}; }
        return sm::fft::repmat2 (folded);
    }

    template<typename F>
    sm::fft::cmat<F> iw_matrix (int a, std::uint32_t R, std::uint32_t C)
    {
        sm::fft::cmat<F> out (R, C);
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
    std::pair<sm::fft::cmat<F>, sm::fft::cmat<F>> ihfft2 (const sm::fft::cmat<F>& X0, const sm::fft::cmat<F>& X1)
    {
        std::uint32_t R = X0.rows;
        std::uint32_t C = X0.cols;

        auto [g00, g01] = idft_inst1 (X0);
        auto [g10, g11] = idft_inst1 (X1);

        sm::fft::cmat<F> a0 = inst2 (g00);
        sm::fft::cmat<F> t0 = inst2 (g10);
        sm::fft::cmat<F> IW0 = iw_matrix<F> (0, R, C);
        sm::fft::cmat<F> out0 (R, C);
        for (std::uint32_t i = 0; i < out0.data.size(); ++i) { out0.data[i] = F{0.5} * (a0.data[i] + IW0.data[i] * t0.data[i]); }

        sm::fft::cmat<F> a1 = inst3 (g01);
        sm::fft::cmat<F> t1 = inst3 (g11);
        sm::fft::cmat<F> IW1 = iw_matrix<F> (1, R, C);
        sm::fft::cmat<F> out1 (R, C);
        for (std::uint32_t i = 0; i < out1.data.size(); ++i) { out1.data[i] = F{0.5} * (a1.data[i] + IW1.data[i] * t1.data[i]); }

        return { out0, out1 };
    }

    sm::vec<std::int32_t, 2> asa_to_ri_gi (const std::uint32_t a, const std::uint32_t r, const std::uint32_t c,
                                           const std::int32_t ri_min, const std::int32_t gi_min)
    {
        const std::int32_t gi = r * 2 + a; // r (row) gives gi
        const std::int32_t ri = c - r;
        return sm::vec<std::int32_t, 2> { ri + ri_min, gi + gi_min };
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

    //! Flatten a pair of (a=0, a=1) matrices into a single vvec, indexed as [a*n*m + r*m + c].
    template<typename F>
    sm::vvec<std::complex<F>> flatten (const sm::fft::cmat<F>& d0, const sm::fft::cmat<F>& d1)
    {
        std::uint32_t plane = d0.rows * d0.cols;
        sm::vvec<std::complex<F>> out (2 * plane);
        for (std::uint32_t i = 0; i < plane; ++i) {
            out[i] = d0.data[i];
            out[plane + i] = d1.data[i];
        }
        return out;
    }

    //! The inverse of flatten.
    template<typename F>
    std::pair<sm::fft::cmat<F>, sm::fft::cmat<F>> unflatten (const sm::vvec<std::complex<F>>& data, std::uint32_t n, std::uint32_t m)
    {
        std::uint32_t plane = static_cast<std::uint32_t> (n) * static_cast<std::uint32_t> (m);
        if (data.size() != 2 * plane) {
            throw std::runtime_error ("sm::hexfft: spectrum data size does not match its n, m");
        }
        sm::fft::cmat<F> d0 (n, m);
        sm::fft::cmat<F> d1 (n, m);
        for (std::uint32_t i = 0; i < plane; ++i) {
            d0.data[i] = data[i];
            d1.data[i] = data[plane + i];
        }
        return { d0, d1 };
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

    template<typename F>
    std::pair<sm::fft::cmat<F>, sm::fft::cmat<F>> image_hexgrid_to_asa (const sm::hexgrid<F>& hg, const sm::vvec<std::complex<F>>& data,
                                                                        std::int32_t ri_min, std::int32_t gi_min,
                                                                        std::uint32_t n, std::uint32_t m)
    {
        if (data.size() != hg.num()) {
            std::stringstream ee;
            ee << "sm::hexfft: data.size() (" << data.size() << ") does not match hg.num() (" << hg.num() << ")";
            throw std::runtime_error (ee.str());
        }
        sm::fft::cmat<F> d0 (n, m);
        sm::fft::cmat<F> d1 (n, m);
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

    // Take the k1, k2 tile coordinates from freq space and make it into rgb coordinates for the hexgrid
    sm::vec<std::int32_t, 3> ks_to_rgb (const std::uint32_t k1, const std::uint32_t k2,
                                        const std::int32_t rows, const std::int32_t cols)
    {
        std::int32_t ri_offs = -rows / 2;
        std::int32_t bi_offs = rows / 2;
        std::int32_t gi_offs = -cols / 2;
        sm::vec<std::int32_t, 3> rgb = { ri_offs, gi_offs + static_cast<std::int32_t>(k1), bi_offs - static_cast<std::int32_t>(k2) };
        return rgb;
    }

    // X0 is the array (0, s, d) and X1 is (1, s, d). Transfer these to a hexgrid in the freq. space.
    template<typename F>
    sm::vvec<std::complex<F>> X_asa_to_frequency_hexgrid (const sm::hexgrid<F, sm::fft::hgf_align>* hgf,
                                                          const sm::fft::cmat<F>& X0, const sm::fft::cmat<F>& X1,
                                                          std::int32_t ri_min, std::int32_t gi_min)
    {
        sm::vvec<std::complex<F>> out (hgf->num(), std::complex<F>{0,0});

        std::cout << ri_min << ", " << gi_min << std::endl;
        // X0 is (0, s, d)
        for (std::uint32_t i = 0; i < X0.size(); ++i) {
            const std::uint32_t r = i / X0.cols;
            const std::uint32_t c = i % X0.cols;

            auto k1 = 0 + r + c;
            auto k2 = 0 + 2 * r;
            sm::vec<std::int32_t, 3> rgb = ks_to_rgb (k1, k2, X0.rows, X1.cols);
            // Find the vi index for k1, k2
            auto hi = hgf->find_hex_at (rgb);
            if (hi->vi < out.size()) { out[hi->vi] = X0.data[i]; }
        }
        // X1 is (1, s, d)
        for (std::uint32_t i = 0; i < X1.size(); ++i) {
            const std::uint32_t r = i / X1.cols;
            const std::uint32_t c = i % X1.cols;

            auto k1 = 1 + r + c;
            auto k2 = 1 + 2 * r;
            sm::vec<std::int32_t, 3> rgb = ks_to_rgb (k1, k2, X1.rows, X1.cols);

            // Find the vi index for k1, k2
            auto hi = hgf->find_hex_at (rgb);
            if (hi->vi < out.size()) { out[hi->vi] = X1.data[i]; }
        }

        return out;
    }

} // sm::hexfft::internal

export namespace sm::hexfft
{
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
        std::uint32_t n = 0;
        //! Columns in the ASA grids.
        std::uint32_t m = 0;
        //! The hex::ri, hex::gi of the padded rectangle's (a=0, r=0, c=0) corner.
        std::int32_t ri_min = 0;
        std::int32_t gi_min = 0;

        //! This holds the data in the twin rectangular grids (ASA: array set addressing grids)
        std::pair<sm::fft::cmat<F>, sm::fft::cmat<F>> d_asa; // first: even, second: odd
        //! FFT in twin rectangular ASA grids. Saved to enable plotting/debugging
        std::pair<sm::fft::cmat<F>, sm::fft::cmat<F>> X_asa;

        void re_quadrant()
        {
            // cmat has rows, cols and vvec<> data
            if (X_asa.second.cols % 2) {
                std::cout << "cols not divisible by 2\n";
                return;
            }
            if (X_asa.second.rows % 2) {
                std::cout << "rows not divisible by 2\n";
                return;
            }

            if ((X_asa.first.rows != X_asa.second.rows) || (X_asa.first.cols != X_asa.second.cols)) {
                std::cerr << "re_quadrant: Size mismatch, returning without changing anything\n";
                return;
            }

            sm::fft::cmat<F> X0 (X_asa.first.rows, X_asa.first.cols);
            sm::fft::cmat<F> X1 (X_asa.second.rows, X_asa.second.cols);

            std::uint32_t hcols = X_asa.second.cols / 2;
            std::uint32_t hrows = X_asa.second.rows / 2;

            sm::vvec<std::complex<F>> quad (hcols * hrows);
            std::uint32_t sidx = 0;
            std::uint32_t didx = 0;
            for (std::uint32_t i = 0; i < hcols; i++) {
                for (std::uint32_t j = 0; j < hrows; j++) {
                    sidx = i + (j * X_asa.second.cols);
                    didx = (i + hcols) + ((j + hrows) * X_asa.second.cols);
                    if (didx < X1.size() && sidx < X1.size()) {
                        X0.data[didx] = X_asa.first.data[sidx];
                        X1.data[didx] = X_asa.second.data[sidx];
                    }
                    didx = sidx;
                    sidx = (i + hcols) + ((j) * X_asa.second.cols);
                    if (didx < X1.size() && sidx < X1.size()) {
                        X0.data[didx] = X_asa.first.data[sidx];
                        X1.data[didx] = X_asa.second.data[sidx];
                    }
                    sidx += hrows * X_asa.second.cols;
                    didx += hrows * X_asa.second.cols;
                    if (didx < X1.size() && sidx < X1.size()) {
                        X0.data[didx] = X_asa.first.data[sidx];
                        X1.data[didx] = X_asa.second.data[sidx];
                    }
                    sidx = i + ((j + hrows) * X_asa.second.cols);
                    didx = (i + hcols) + ((j) * X_asa.second.cols);
                    if (didx < X1.size() && sidx < X1.size()) {
                        X0.data[didx] = X_asa.first.data[sidx];
                        X1.data[didx] = X_asa.second.data[sidx];
                    }
                }
            }
            X_asa.first = X0;
            X_asa.second = X1;
        }

        // Reverse of re_quadrant
        void de_quadrant()
        {
            if (X_asa.second.cols % 2) {
                std::cout << "cols not divisible by 2\n";
                return;
            }
            if (X_asa.second.rows % 2) {
                std::cout << "rows not divisible by 2\n";
                return;
            }

            if ((X_asa.first.rows != X_asa.second.rows) || (X_asa.first.cols != X_asa.second.cols)) {
                std::cerr << "re_quadrant: Size mismatch, returning without changing anything\n";
                return;
            }

            sm::fft::cmat<F> X0 (X_asa.first.rows, X_asa.first.cols);
            sm::fft::cmat<F> X1 (X_asa.second.rows, X_asa.second.cols);

            std::uint32_t hcols = X_asa.second.cols / 2;
            std::uint32_t hrows = X_asa.second.rows / 2;

            sm::vvec<std::complex<F>> quad (hcols * hrows);
            std::uint32_t sidx = 0;
            std::uint32_t didx = 0;
            // Comparing with re_quadrant, I just swapped didx and sidx. Easy.
            for (std::uint32_t i = 0; i < hcols; i++) {
                for (std::uint32_t j = 0; j < hrows; j++) {
                    didx = i + (j * X_asa.second.cols);
                    sidx = (i + hcols) + ((j + hrows) * X_asa.second.cols);
                    if (didx < X1.size() && sidx < X1.size()) {
                        X0.data[didx] = X_asa.first.data[sidx];
                        X1.data[didx] = X_asa.second.data[sidx];
                    }
                    sidx = didx;
                    didx = (i + hcols) + ((j) * X_asa.second.cols);
                    if (didx < X1.size() && sidx < X1.size()) {
                        X0.data[didx] = X_asa.first.data[sidx];
                        X1.data[didx] = X_asa.second.data[sidx];
                    }
                    didx += hrows * X_asa.second.cols;
                    sidx += hrows * X_asa.second.cols;
                    if (didx < X1.size() && sidx < X1.size()) {
                        X0.data[didx] = X_asa.first.data[sidx];
                        X1.data[didx] = X_asa.second.data[sidx];
                    }
                    didx = i + ((j + hrows) * X_asa.second.cols);
                    sidx = (i + hcols) + ((j) * X_asa.second.cols);
                    if (didx < X1.size() && sidx < X1.size()) {
                        X0.data[didx] = X_asa.first.data[sidx];
                        X1.data[didx] = X_asa.second.data[sidx];
                    }
                }
            }
            X_asa.first = X0;
            X_asa.second = X1;
        }

        //! Result. flat data. A copy of X_asa.first and X_asa.second in a single 1D vvec
        sm::vvec<std::complex<F>> data;

        //! We construct a frequency hexgrid from the image hexgrid.
        std::unique_ptr<sm::hexgrid<F, sm::fft::hgf_align>> hgf;
        //! Scaling factor (obtained from image data hexgrid spacing)
        F Uscale = F{1};
        //! Result, suitable for visualization on the frequency space hexgrid
        sm::vvec<std::complex<F>> hex_data;

        //! The total number of samples in the padded rectangle (2 * n * m).
        std::uint32_t size() const { return 2u * this->n * this->m; }
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
        internal::bounding_box (hg, result.ri_min, result.gi_min, result.n, result.m);
        std::cout << "ri_min: " << result.ri_min << ", gi_min: " << result.gi_min
                  << " n x m: " << result.n << " x " <<  result.m << std::endl;

        // Save the input data after it has been extracted into ASA format
        result.d_asa = internal::image_hexgrid_to_asa (hg, data, result.ri_min, result.gi_min, result.n, result.m);
        // Fourier transform the ASA formatted data into an ASA formatted result (result.X_asa)
        result.X_asa = internal::hfft2 (result.d_asa.first, result.d_asa.second);
        // Make concatenated version of X_asa.
        result.data = internal::flatten (result.X_asa.first, result.X_asa.second);
        // Re-quadrant X_asa before putting it on hexgrid
        result.re_quadrant();
        result.de_quadrant();

        // Construct a hexgrid
        auto V = sm::fft::make_V<float>();
        V *= hg.d;
        const sm::mat<F, 2, 2> U = sm::fft::make_U<F>(V);
        result.Uscale = V.col(0).length() * V.col(0).length(); // a suitable scaling (zoom factor) for the frequency hexgrid
        result.hgf = std::make_unique<sm::hexgrid<F, sm::fft::hgf_align>>(U.col(0).length(), result.m * 2 * U.col(0).length(), 0.0f);
        result.hgf->set_rectangular_boundary (result.m * U.col(0).length(), result.m * U.col(0).length());

        // Populated a frequency space hexgrid with result.X_asa
        result.hex_data = internal::X_asa_to_frequency_hexgrid (result.hgf.get(), result.X_asa.first, result.X_asa.second, result.ri_min, result.gi_min);

        return result;
    }

    //! As above, but for real-valued input data.
    template<typename F>
    spectrum<F> fft (const sm::hexgrid<F>& hg, const sm::vvec<F>& data)
    {
        sm::vvec<std::complex<F>> cdata (data.size());
        for (std::uint32_t i = 0; i < data.size(); ++i) { cdata[i] = std::complex<F> (data[i], F{0}); }
        return sm::hexfft::fft (hg, cdata);
    }

#if 0
    /*!
     * Compute the inverse hexagonal FFT, undoing sm::hexfft::fft. hg must be the same hexgrid
     * (or one with the same hexes) that X was computed from. The result is indexed by
     * hex::vi, as usual (data.size() == hg.num()); the part of X's padded rectangle that lies
     * outside hg's boundary is simply discarded.
     */
    template<typename F>
    sm::vvec<std::complex<F>> ifft (const sm::hexgrid<F>& hg, const spectrum<F>& X)
    {
        auto [d0, d1] = internal::unflatten (X.data, X.n, X.m);
        auto [x0, x1] = internal::ihfft2 (d0, d1);
        return internal::extract_by_vi (hg, x0, x1, X.ri_min, X.gi_min);
    }
#endif

#if 0
    // All wrong!

    /*!
     * As above, but taking X indexed by hex::vi (X.size() == hg.num()), such as
     * spectrum::hex_data, rather than a full sm::hexfft::spectrum. Since X only carries one
     * value per hex, hg's bounding rectangle (see sm::hexfft::fft) is recomputed from hg, and
     * any entries of that rectangle that don't correspond to a hex in hg (i.e. the zero-padded
     * region added by sm::hexfft::fft, if hg's boundary doesn't already fill its own bounding
     * rectangle) are treated as zero. X is then ifftshifted (see internal::ifftshift) to undo
     * the fftshift that sm::hexfft::fft applies when it builds spectrum::hex_data, restoring
     * the DC-at-(0,0) bin ordering that the inverse transform expects.
     *
     * Note that this makes this overload a true inverse of sm::hexfft::fft only when hg's
     * boundary exactly fills its bounding rectangle in (ri, gi) space (so that
     * spectrum::hex_data and spectrum::data carry the same information, just laid out
     * differently). For any other boundary shape, spectrum::hex_data has already discarded the
     * frequency content in the padded region, so the result here is a filtered approximation
     * to the original spatial data, not an exact reconstruction: use the
     * sm::hexfft::ifft (hg, spectrum) overload, which keeps that padded region, when an exact
     * inverse is required.
     */
    template<typename F>
    sm::vvec<std::complex<F>> ifft (const sm::hexgrid<F>& hg, const sm::vvec<std::complex<F>>& X)
    {
        std::int32_t ri_min = 0;
        std::int32_t gi_min = 0;
        std::uint32_t n = 0;
        std::uint32_t m = 0;
        internal::bounding_box (hg, ri_min, gi_min, n, m);

        auto [d0, d1] = internal::image_hexgrid_to_asa (hg, X, ri_min, gi_min, n, m);
        d0 = sm::fft::ifftshift (d0);
        d1 = sm::fft::ifftshift (d1);
        auto [x0, x1] = internal::ihfft2 (d0, d1);
        return internal::extract_by_vi (hg, x0, x1, ri_min, gi_min);
    }
#endif

} // sm::hexfft
