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

#include <cstdint>
#include <complex>
#include <vector>
#include <utility>
#include <stdexcept>
#include <sstream>
#include <cmath>

export module sm.hexfft;

export import sm.hexgrid;
export import sm.vvec;
import sm.mathconst;

export namespace sm::hexfft
{
    /*!
     * A minimal row-major complex matrix. This is an intermediate data type that is used during the
     * hexagonal FFT computation. it is exported so that the data can be inspected.
     */
    template<typename F>
    struct cmat
    {
        std::uint32_t rows = 0;
        std::uint32_t cols = 0;
        sm::vvec<std::complex<F>> data;

        cmat() = default;
        cmat (std::uint32_t r, std::uint32_t c) : rows(r), cols(c), data (r * c, std::complex<F>{F{0}, F{0}}) {}

        std::complex<F>& operator() (std::uint32_t r, std::uint32_t c) { return this->data[r * this->cols + c]; }
        const std::complex<F>& operator() (std::uint32_t r, std::uint32_t c) const { return this->data[r * this->cols + c]; }
    };
}

namespace sm::hexfft::internal
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
    void dft_rows (sm::hexfft::cmat<F>& mat, bool invert)
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
    void dft_cols (sm::hexfft::cmat<F>& mat, bool invert)
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
    sm::hexfft::cmat<F> pad_zeros (const sm::hexfft::cmat<F>& in)
    {
        sm::hexfft::cmat<F> out (in.rows, in.cols * 2);
        for (std::uint32_t r = 0; r < in.rows; ++r) {
            for (std::uint32_t c = 0; c < in.cols; ++c) { out(r, c) = in(r, c); }
        }
        return out;
    }

    //! Take every other column of in, starting at column offset (0 or 1): (rows,2*cols) -> (rows,cols)
    template<typename F>
    sm::hexfft::cmat<F> decimate_cols (const sm::hexfft::cmat<F>& in, std::uint32_t offset)
    {
        std::uint32_t outcols = in.cols / 2;
        sm::hexfft::cmat<F> out (in.rows, outcols);
        for (std::uint32_t r = 0; r < in.rows; ++r) {
            for (std::uint32_t c = 0; c < outcols; ++c) { out(r, c) = in(r, offset + 2 * c); }
        }
        return out;
    }

    //! Add (add==true) or subtract the bottom half of in's rows from the top half.
    template<typename F>
    sm::hexfft::cmat<F> fold_half (const sm::hexfft::cmat<F>& in, bool add)
    {
        std::uint32_t half = in.rows / 2;
        sm::hexfft::cmat<F> out (half, in.cols);
        for (std::uint32_t r = 0; r < half; ++r) {
            for (std::uint32_t c = 0; c < in.cols; ++c) {
                out(r, c) = add ? (in(r, c) + in(r + half, c)) : (in(r, c) - in(r + half, c));
            }
        }
        return out;
    }

    //! Vertically stack two copies of in, on top of each other.
    template<typename F>
    sm::hexfft::cmat<F> repmat2 (const sm::hexfft::cmat<F>& in)
    {
        sm::hexfft::cmat<F> out (in.rows * 2, in.cols);
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
    sm::hexfft::cmat<F> roll (const sm::hexfft::cmat<F>& in, std::uint32_t shift_r, std::uint32_t shift_c)
    {
        sm::hexfft::cmat<F> out (in.rows, in.cols);
        for (std::uint32_t r = 0; r < in.rows; ++r) {
            std::uint32_t sr = (r + shift_r) % in.rows;
            for (std::uint32_t c = 0; c < in.cols; ++c) {
                std::uint32_t sc = (c + shift_c) % in.cols;
                out (sr, sc) = in (r, c);
            }
        }
        return out;
    }

    //! Move the zero-frequency (DC) bin, at (0,0), to the middle of in, matching the
    //! numpy/MATLAB fftshift convention. Because in.rows is always even here, this is its own
    //! exact inverse in the row direction; in.cols need not be even, so use ifftshift, not
    //! fftshift again, to invert this exactly when in.cols is odd.
    template<typename F>
    sm::hexfft::cmat<F> fftshift (const sm::hexfft::cmat<F>& in)
    {
        return roll (in, in.rows / 2u, in.cols / 2u);
    }

    //! The exact inverse of fftshift: moves the bin at the middle of in (where fftshift put
    //! the DC bin) back to (0,0).
    template<typename F>
    sm::hexfft::cmat<F> ifftshift (const sm::hexfft::cmat<F>& in)
    {
        return roll (in, in.rows - in.rows / 2u, in.cols - in.cols / 2u);
    }

    // The remaining functions follow the naming used by Birdsong & Rummelt (and by
    // gwater/HexFFT.jl, a reference implementation of the same algorithm). data has shape
    // (R, COLS), where R (== grid::n) MUST be even.

    template<typename F>
    std::pair<sm::hexfft::cmat<F>, sm::hexfft::cmat<F>> dft_nst1 (const sm::hexfft::cmat<F>& data)
    {
        sm::hexfft::cmat<F> padded = pad_zeros (data);
        dft_rows (padded, false);
        return { decimate_cols (padded, 0), decimate_cols (padded, 1) };
    }

    template<typename F>
    sm::hexfft::cmat<F> nst2 (const sm::hexfft::cmat<F>& data)
    {
        sm::hexfft::cmat<F> folded = fold_half (data, true);
        dft_cols (folded, false);
        return repmat2 (folded);
    }

    template<typename F>
    sm::hexfft::cmat<F> nst3 (const sm::hexfft::cmat<F>& data)
    {
        std::uint32_t R = data.rows;
        sm::hexfft::cmat<F> folded = fold_half (data, false);
        for (std::uint32_t r = 0; r < folded.rows; ++r) {
            F ang = F{-2} * sm::mathconst<F>::pi * static_cast<F>(r) / static_cast<F>(R);
            std::complex<F> coeff (std::cos (ang), std::sin (ang));
            for (std::uint32_t c = 0; c < folded.cols; ++c) { folded(r, c) *= coeff; }
        }
        dft_cols (folded, false);
        return repmat2 (folded);
    }

    template<typename F>
    sm::hexfft::cmat<F> w_matrix (int b, std::uint32_t R, std::uint32_t COLS)
    {
        sm::hexfft::cmat<F> out (R, COLS);
        for (std::uint32_t s = 0; s < R; ++s) {
            for (std::uint32_t d = 0; d < COLS; ++d) {
                F ang = -sm::mathconst<F>::pi * ( (static_cast<F>(b) + F{2} * static_cast<F>(d)) / (F{2} * static_cast<F>(COLS))
                                                 + (static_cast<F>(b) + F{2} * static_cast<F>(s)) / static_cast<F>(R) );
                out(s, d) = std::complex<F> (std::cos (ang), std::sin (ang));
            }
        }
        return out;
    }

    //! The forward transform. data0 and data1 hold the two interleaved rectangular arrays (a=0 and a=1).
    template<typename F>
    std::pair<sm::hexfft::cmat<F>, sm::hexfft::cmat<F>> hfft2 (const sm::hexfft::cmat<F>& data0, const sm::hexfft::cmat<F>& data1)
    {
        std::uint32_t R = data0.rows;
        std::uint32_t COLS = data0.cols;

        auto [g00, g01] = dft_nst1 (data0);
        auto [g10, g11] = dft_nst1 (data1);

        sm::hexfft::cmat<F> X0 = nst2 (g00);
        sm::hexfft::cmat<F> t0 = nst2 (g10);
        sm::hexfft::cmat<F> W0 = w_matrix<F> (0, R, COLS);
        for (std::uint32_t i = 0; i < X0.data.size(); ++i) { X0.data[i] += W0.data[i] * t0.data[i]; }

        sm::hexfft::cmat<F> X1 = nst3 (g01);
        sm::hexfft::cmat<F> t1 = nst3 (g11);
        sm::hexfft::cmat<F> W1 = w_matrix<F> (1, R, COLS);
        for (std::uint32_t i = 0; i < X1.data.size(); ++i) { X1.data[i] += W1.data[i] * t1.data[i]; }

        return { X0, X1 };
    }

    template<typename F>
    std::pair<sm::hexfft::cmat<F>, sm::hexfft::cmat<F>> idft_inst1 (const sm::hexfft::cmat<F>& data)
    {
        sm::hexfft::cmat<F> padded = pad_zeros (data);
        dft_rows (padded, true);
        for (auto& v : padded.data) { v *= F{2}; }
        return { decimate_cols (padded, 0), decimate_cols (padded, 1) };
    }

    template<typename F>
    sm::hexfft::cmat<F> inst2 (const sm::hexfft::cmat<F>& data)
    {
        sm::hexfft::cmat<F> folded = fold_half (data, true);
        dft_cols (folded, true);
        for (auto& v : folded.data) { v *= F{0.5}; }
        return repmat2 (folded);
    }

    template<typename F>
    sm::hexfft::cmat<F> inst3 (const sm::hexfft::cmat<F>& data)
    {
        std::uint32_t R = data.rows;
        sm::hexfft::cmat<F> folded = fold_half (data, false);
        for (std::uint32_t r = 0; r < folded.rows; ++r) {
            F ang = F{2} * sm::mathconst<F>::pi * static_cast<F>(r) / static_cast<F>(R);
            std::complex<F> coeff (std::cos (ang), std::sin (ang));
            for (std::uint32_t c = 0; c < folded.cols; ++c) { folded(r, c) *= coeff; }
        }
        dft_cols (folded, true);
        for (auto& v : folded.data) { v *= F{0.5}; }
        return repmat2 (folded);
    }

    template<typename F>
    sm::hexfft::cmat<F> iw_matrix (int a, std::uint32_t R, std::uint32_t COLS)
    {
        sm::hexfft::cmat<F> out (R, COLS);
        for (std::uint32_t r = 0; r < R; ++r) {
            for (std::uint32_t c = 0; c < COLS; ++c) {
                F ang = sm::mathconst<F>::pi * ( (static_cast<F>(a) + F{2} * static_cast<F>(c)) / (F{2} * static_cast<F>(COLS))
                                                + (static_cast<F>(a) + F{2} * static_cast<F>(r)) / static_cast<F>(R) );
                out(r, c) = std::complex<F> (std::cos (ang), std::sin (ang));
            }
        }
        return out;
    }

    //! The inverse transform, undoing hfft2.
    template<typename F>
    std::pair<sm::hexfft::cmat<F>, sm::hexfft::cmat<F>> ihfft2 (const sm::hexfft::cmat<F>& X0, const sm::hexfft::cmat<F>& X1)
    {
        std::uint32_t R = X0.rows;
        std::uint32_t COLS = X0.cols;

        auto [g00, g01] = idft_inst1 (X0);
        auto [g10, g11] = idft_inst1 (X1);

        sm::hexfft::cmat<F> a0 = inst2 (g00);
        sm::hexfft::cmat<F> t0 = inst2 (g10);
        sm::hexfft::cmat<F> IW0 = iw_matrix<F> (0, R, COLS);
        sm::hexfft::cmat<F> out0 (R, COLS);
        for (std::uint32_t i = 0; i < out0.data.size(); ++i) { out0.data[i] = F{0.5} * (a0.data[i] + IW0.data[i] * t0.data[i]); }

        sm::hexfft::cmat<F> a1 = inst3 (g01);
        sm::hexfft::cmat<F> t1 = inst3 (g11);
        sm::hexfft::cmat<F> IW1 = iw_matrix<F> (1, R, COLS);
        sm::hexfft::cmat<F> out1 (R, COLS);
        for (std::uint32_t i = 0; i < out1.data.size(); ++i) { out1.data[i] = F{0.5} * (a1.data[i] + IW1.data[i] * t1.data[i]); }

        return { out0, out1 };
    }

    /*!
     * A hex's (a, r, c) position within the padded ASA rectangle whose (a=0,r=0,c=0) corner
     * sits at hex::ri==ri_min, hex::gi==gi_min.
     *
     * hex::gi's parity and hex::gi/2 give a (the row-parity array) and r (the row within it)
     * directly. The column is NOT simply hex::ri, though: from hex::compute_location,
     * hex::x == d*hex::ri + (d/2)*hex::gi, so for a fixed a, successive rows (hex::gi
     * increasing by 2, i.e. r increasing by 1) each start half a hex further right in x than
     * the row below -- hex::ri on its own decreases by one per row just to (over)compensate.
     * Substituting hex::gi = 2*r + a shows hex::x == d*(hex::ri + r) + (d/2)*a: the a-dependent
     * offset aside, hex::x is a clean, row-independent function of (hex::ri + r) alone. That
     * sum is exactly the c used here (this is the same value that, e.g., dividing (hex::x -
     * a*d/2) by d and rounding would give -- this integer form is exact, and cheaper).
     *
     * Getting this wrong (using hex::ri directly as c, as this function used to) still gives
     * an invertible, self-consistent transform, since it never leaves the algebra of the
     * hfft2/ihfft2 kernel -- but the two "rectangles" it operates on are not actually
     * rectangles in (hex::x, hex::y) space, they're parallelograms.
     */
    template<typename F>
    void asa_position (const sm::hex<F>& h, std::int32_t ri_min, std::int32_t gi_min,
                       std::uint32_t& a, std::uint32_t& r, std::uint32_t& c)
    {
        std::int32_t r_signed = (h.gi - gi_min) / 2; // h.gi - gi_min >= 0, so this is exact floor division
        a = static_cast<std::uint32_t> ((h.gi - gi_min) - 2 * r_signed);
        r = static_cast<std::uint32_t> (r_signed);
        c = static_cast<std::uint32_t> (h.ri + r_signed - ri_min);
    }

    //! Find the smallest rectangle, in the (a, r, c) addressing of asa_position, that encloses
    //! every hex in hg, and round its row count up so that n (the number of rows in each of
    //! the two row-parity arrays) is even and at least 2, as required by fold_half.
    template<typename F>
    void bounding_box (const sm::hexgrid<F>& hg, std::int32_t& ri_min, std::int32_t& gi_min,
                       std::uint32_t& n, std::uint32_t& m)
    {
        if (hg.hexen.empty()) {
            throw std::runtime_error ("sm::hexfft: hexgrid has no hexes");
        }

        // hex::gi's parity/2 addressing (a, r) is unaffected by the column shear discussed in
        // asa_position, so gi_min, gi_max can be found directly.
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

        // The column, c = hex::ri + r (see asa_position), does depend on r, so it needs a
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
    sm::vvec<std::complex<F>> flatten (const sm::hexfft::cmat<F>& d0, const sm::hexfft::cmat<F>& d1)
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
    std::pair<sm::hexfft::cmat<F>, sm::hexfft::cmat<F>> unflatten (const sm::vvec<std::complex<F>>& data, std::uint32_t n, std::uint32_t m)
    {
        std::uint32_t plane = static_cast<std::uint32_t> (n) * static_cast<std::uint32_t> (m);
        if (data.size() != 2 * plane) {
            throw std::runtime_error ("sm::hexfft: spectrum data size does not match its n, m");
        }
        sm::hexfft::cmat<F> d0 (n, m);
        sm::hexfft::cmat<F> d1 (n, m);
        for (std::uint32_t i = 0; i < plane; ++i) {
            d0.data[i] = data[i];
            d1.data[i] = data[plane + i];
        }
        return { d0, d1 };
    }

    //! The inverse of extract_by_vi: scatter one value per hex in hg (indexed by hex::vi, as
    //! usual for hexgrid client data) into a zero-filled padded rectangle of shape (n, m)
    //! whose (a=0,r=0,c=0) corner sits at hex::ri==ri_min, hex::gi==gi_min.
    template<typename F>
    std::pair<sm::hexfft::cmat<F>, sm::hexfft::cmat<F>> populate_from_vi (const sm::hexgrid<F>& hg, const sm::vvec<std::complex<F>>& data,
                                                                          std::int32_t ri_min, std::int32_t gi_min,
                                                                          std::uint32_t n, std::uint32_t m)
    {
        if (data.size() != hg.num()) {
            std::stringstream ee;
            ee << "sm::hexfft: data.size() (" << data.size() << ") does not match hg.num() (" << hg.num() << ")";
            throw std::runtime_error (ee.str());
        }
        sm::hexfft::cmat<F> d0 (n, m);
        sm::hexfft::cmat<F> d1 (n, m);
        for (const auto& h : hg.hexen) {
            std::uint32_t a, r, c;
            asa_position (h, ri_min, gi_min, a, r, c);
            if (a == 0u) { d0 (r, c) = data[h.vi]; } else { d1 (r, c) = data[h.vi]; }
        }
        return { d0, d1 };
    }

    //! Read back the one value per hex in hg from the padded rectangle (d0, d1) (whose
    //! (a=0,r=0,c=0) corner sits at hex::ri==ri_min, hex::gi==gi_min), producing a vvec
    //! indexed by hex::vi as usual for hexgrid client data.
    template<typename F>
    sm::vvec<std::complex<F>> extract_by_vi (const sm::hexgrid<F>& hg, const sm::hexfft::cmat<F>& d0, const sm::hexfft::cmat<F>& d1,
                                             std::int32_t ri_min, std::int32_t gi_min)
    {
        sm::vvec<std::complex<F>> out (hg.num());
        for (const auto& h : hg.hexen) {
            std::uint32_t a, r, c;
            asa_position (h, ri_min, gi_min, a, r, c);
            if (r >= d0.rows || c >= d0.cols) {
                throw std::runtime_error ("sm::hexfft: hg has a hex outside the given rectangle's bounds");
            }
            out[h.vi] = (a == 0u) ? d0 (r, c) : d1 (r, c);
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
     * hexgrid's boundary as a zero input sample (an ordinary zero-padded/windowed FFT). This
     * spectrum covers that whole padded rectangle, and so is generally larger than the
     * hexgrid it was computed from: use sm::hexfft::ifft, passing the same hexgrid, to invert
     * it back down to one value per hex.
     */
    template<typename F = double>
    struct spectrum
    {
        //! Rows in each of the two (even/odd hex::gi) sub-arrays of the padded rectangle.
        std::uint32_t n = 0;
        //! Columns in the padded rectangle (the hex::ri extent).
        std::uint32_t m = 0;
        //! The hex::ri, hex::gi of the padded rectangle's (a=0, r=0, c=0) corner.
        std::int32_t ri_min = 0;
        std::int32_t gi_min = 0;
        //! Flat data, indexed as data[a * n * m + r * m + c], with a in {0,1} selecting the
        //! even/odd hex::gi sub-array.
        sm::vvec<std::complex<F>> data;

        //! This holds the data in the twin rectangular grids (ASA: array set addressing grids)
        std::pair<sm::hexfft::cmat<F>, sm::hexfft::cmat<F>> d_asa;
        //! FFT in twin rectangular ASA grids. Saved to enable plotting/debugging
        std::pair<sm::hexfft::cmat<F>, sm::hexfft::cmat<F>> X_asa;

        //! An fftshifted copy of data (see internal::fftshift), restricted to the frequency
        //! bins at positions occupied by a hex in the hexgrid the transform was computed from,
        //! and indexed by that hex's hex::vi (so hex_data.size() == hg.num()). The fftshift
        //! moves the zero-frequency (DC) bin to the middle of the padded rectangle instead of
        //! its (0,0) corner, so that plotting hex_data on the hexgrid shows a centred spectrum
        //! rather than one that wraps at the edges. A convenience for inspecting or
        //! visualising the spectrum on the original hexgrid; pass it to the
        //! sm::hexfft::ifft (hg, vvec<complex<F>>) overload, which undoes the shift, to invert.
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

        // Save the input data after it has been extracted into ASA format
        result.d_asa = internal::populate_from_vi (hg, data, result.ri_min, result.gi_min, result.n, result.m);
        // Save the FFT'd data in ASA format
        result.X_asa = internal::hfft2 (result.d_asa.first, result.d_asa.second);

        result.data = internal::flatten (result.X_asa.first, result.X_asa.second);

        // hex_data is read off an fftshifted copy of X_asa, so that DC lands in the middle of
        // the hexgrid rather than at a corner when it's plotted. result.data/X_asa themselves
        // are left un-shifted, so sm::hexfft::ifft (hg, spectrum<F>) is unaffected.
        sm::hexfft::cmat<F> X0_shifted = internal::fftshift (result.X_asa.first);
        sm::hexfft::cmat<F> X1_shifted = internal::fftshift (result.X_asa.second);
        result.hex_data = internal::extract_by_vi (hg, X0_shifted, X1_shifted, result.ri_min, result.gi_min);
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

        auto [d0, d1] = internal::populate_from_vi (hg, X, ri_min, gi_min, n, m);
        d0 = internal::ifftshift (d0);
        d1 = internal::ifftshift (d1);
        auto [x0, x1] = internal::ihfft2 (d0, d1);
        return internal::extract_by_vi (hg, x0, x1, ri_min, gi_min);
    }

} // sm::hexfft
