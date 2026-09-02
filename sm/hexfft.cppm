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
 * See sm::hexfft::spectrum for how that rectangle relates back to the hexgrid.
 *
 * claude --resume c364f967-91ac-4ab9-9422-42b410ef007c
 *
 * \author: AI, based in part on work by Josua Grawitter in https://github.com/gwater/HexFFT.jl and on the Wikipedia page
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

namespace sm::hexfft::detail
{
    /*!
     * A minimal row-major complex matrix, used only as scratch space while computing the
     * hexagonal FFT. Not exported; sm::hexfft's public interface deals only in sm::vvec.
     */
    template<typename F>
    struct cmat
    {
        std::size_t rows = 0;
        std::size_t cols = 0;
        sm::vvec<std::complex<F>> data;

        cmat() = default;
        cmat (std::size_t r, std::size_t c) : rows(r), cols(c), data (r * c, std::complex<F>{F{0}, F{0}}) {}

        std::complex<F>& operator() (std::size_t r, std::size_t c) { return this->data[r * this->cols + c]; }
        const std::complex<F>& operator() (std::size_t r, std::size_t c) const { return this->data[r * this->cols + c]; }
    };

    //! In-place iterative radix-2 Cooley-Tukey FFT. invert selects the inverse transform
    //! (which includes the 1/N normalisation). a.size() MUST be a power of two (or 0/1).
    template<typename F>
    void fft_pow2 (std::vector<std::complex<F>>& a, bool invert)
    {
        const std::size_t n = a.size();
        if (n <= 1) { return; }

        for (std::size_t i = 1, j = 0; i < n; ++i) {
            std::size_t bit = n >> 1;
            for (; j & bit; bit >>= 1) { j ^= bit; }
            j ^= bit;
            if (i < j) { std::swap (a[i], a[j]); }
        }

        for (std::size_t len = 2; len <= n; len <<= 1) {
            F ang = (invert ? F{2} : F{-2}) * sm::mathconst<F>::pi / static_cast<F>(len);
            std::complex<F> wlen (std::cos (ang), std::sin (ang));
            for (std::size_t i = 0; i < n; i += len) {
                std::complex<F> w (F{1}, F{0});
                std::size_t half = len / 2;
                for (std::size_t j = 0; j < half; ++j) {
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
        const std::size_t n = a.size();
        if (n <= 1) { return; }

        std::size_t m = 1;
        while (m < 2 * n + 1) { m <<= 1; }

        std::vector<std::complex<F>> exptab (n);
        for (std::size_t i = 0; i < n; ++i) {
            unsigned long long j = (static_cast<unsigned long long>(i) * static_cast<unsigned long long>(i)) % (2ull * static_cast<unsigned long long>(n));
            F ang = (invert ? F{1} : F{-1}) * sm::mathconst<F>::pi * static_cast<F>(j) / static_cast<F>(n);
            exptab[i] = std::complex<F> (std::cos (ang), std::sin (ang));
        }

        std::vector<std::complex<F>> av (m, std::complex<F>{F{0}, F{0}});
        for (std::size_t i = 0; i < n; ++i) { av[i] = a[i] * exptab[i]; }

        std::vector<std::complex<F>> bv (m, std::complex<F>{F{0}, F{0}});
        bv[0] = exptab[0];
        for (std::size_t i = 1; i < n; ++i) { bv[i] = bv[m - i] = std::conj (exptab[i]); }

        fft_pow2 (av, false);
        fft_pow2 (bv, false);
        for (std::size_t i = 0; i < m; ++i) { av[i] *= bv[i]; }
        fft_pow2 (av, true);

        for (std::size_t i = 0; i < n; ++i) { a[i] = av[i] * exptab[i]; }
        if (invert) {
            for (std::size_t i = 0; i < n; ++i) { a[i] /= static_cast<F>(n); }
        }
    }

    //! A DFT/IDFT of an arbitrary, non-zero length.
    template<typename F>
    void dft1d (std::vector<std::complex<F>>& a, bool invert)
    {
        std::size_t n = a.size();
        if (n <= 1) { return; }
        if ((n & (n - 1)) == 0) {
            fft_pow2 (a, invert);
        } else {
            fft_bluestein (a, invert);
        }
    }

    //! Transform each of mat's rows (a 1D signal of length mat.cols) independently.
    template<typename F>
    void dft_rows (cmat<F>& mat, bool invert)
    {
        std::vector<std::complex<F>> buf (mat.cols);
        for (std::size_t r = 0; r < mat.rows; ++r) {
            for (std::size_t c = 0; c < mat.cols; ++c) { buf[c] = mat(r, c); }
            dft1d (buf, invert);
            for (std::size_t c = 0; c < mat.cols; ++c) { mat(r, c) = buf[c]; }
        }
    }

    //! Transform each of mat's columns (a 1D signal of length mat.rows) independently.
    template<typename F>
    void dft_cols (cmat<F>& mat, bool invert)
    {
        std::vector<std::complex<F>> buf (mat.rows);
        for (std::size_t c = 0; c < mat.cols; ++c) {
            for (std::size_t r = 0; r < mat.rows; ++r) { buf[r] = mat(r, c); }
            dft1d (buf, invert);
            for (std::size_t r = 0; r < mat.rows; ++r) { mat(r, c) = buf[r]; }
        }
    }

    //! Horizontally concatenate in with a same-sized block of zeros: (rows,cols) -> (rows,2*cols)
    template<typename F>
    cmat<F> pad_zeros (const cmat<F>& in)
    {
        cmat<F> out (in.rows, in.cols * 2);
        for (std::size_t r = 0; r < in.rows; ++r) {
            for (std::size_t c = 0; c < in.cols; ++c) { out(r, c) = in(r, c); }
        }
        return out;
    }

    //! Take every other column of in, starting at column offset (0 or 1): (rows,2*cols) -> (rows,cols)
    template<typename F>
    cmat<F> decimate_cols (const cmat<F>& in, std::size_t offset)
    {
        std::size_t outcols = in.cols / 2;
        cmat<F> out (in.rows, outcols);
        for (std::size_t r = 0; r < in.rows; ++r) {
            for (std::size_t c = 0; c < outcols; ++c) { out(r, c) = in(r, offset + 2 * c); }
        }
        return out;
    }

    //! Add (add==true) or subtract the bottom half of in's rows from the top half.
    template<typename F>
    cmat<F> fold_half (const cmat<F>& in, bool add)
    {
        std::size_t half = in.rows / 2;
        cmat<F> out (half, in.cols);
        for (std::size_t r = 0; r < half; ++r) {
            for (std::size_t c = 0; c < in.cols; ++c) {
                out(r, c) = add ? (in(r, c) + in(r + half, c)) : (in(r, c) - in(r + half, c));
            }
        }
        return out;
    }

    //! Vertically stack two copies of in, on top of each other.
    template<typename F>
    cmat<F> repmat2 (const cmat<F>& in)
    {
        cmat<F> out (in.rows * 2, in.cols);
        for (std::size_t r = 0; r < in.rows; ++r) {
            for (std::size_t c = 0; c < in.cols; ++c) {
                out(r, c) = in(r, c);
                out(r + in.rows, c) = in(r, c);
            }
        }
        return out;
    }

    // The remaining functions follow the naming used by Birdsong & Rummelt (and by
    // gwater/HexFFT.jl, a reference implementation of the same algorithm). data has shape
    // (R, COLS), where R (== grid::n) MUST be even.

    template<typename F>
    std::pair<cmat<F>, cmat<F>> dft_nst1 (const cmat<F>& data)
    {
        cmat<F> padded = pad_zeros (data);
        dft_rows (padded, false);
        return { decimate_cols (padded, 0), decimate_cols (padded, 1) };
    }

    template<typename F>
    cmat<F> nst2 (const cmat<F>& data)
    {
        cmat<F> folded = fold_half (data, true);
        dft_cols (folded, false);
        return repmat2 (folded);
    }

    template<typename F>
    cmat<F> nst3 (const cmat<F>& data)
    {
        std::size_t R = data.rows;
        cmat<F> folded = fold_half (data, false);
        for (std::size_t r = 0; r < folded.rows; ++r) {
            F ang = F{-2} * sm::mathconst<F>::pi * static_cast<F>(r) / static_cast<F>(R);
            std::complex<F> coeff (std::cos (ang), std::sin (ang));
            for (std::size_t c = 0; c < folded.cols; ++c) { folded(r, c) *= coeff; }
        }
        dft_cols (folded, false);
        return repmat2 (folded);
    }

    template<typename F>
    cmat<F> w_matrix (int b, std::size_t R, std::size_t COLS)
    {
        cmat<F> out (R, COLS);
        for (std::size_t s = 0; s < R; ++s) {
            for (std::size_t d = 0; d < COLS; ++d) {
                F ang = -sm::mathconst<F>::pi * ( (static_cast<F>(b) + F{2} * static_cast<F>(d)) / (F{2} * static_cast<F>(COLS))
                                                 + (static_cast<F>(b) + F{2} * static_cast<F>(s)) / static_cast<F>(R) );
                out(s, d) = std::complex<F> (std::cos (ang), std::sin (ang));
            }
        }
        return out;
    }

    //! The forward transform. data0 and data1 hold the two interleaved rectangular arrays
    //! (a=0 and a=1) each of shape (R, COLS), with R == grid::n and COLS == grid::m.
    template<typename F>
    std::pair<cmat<F>, cmat<F>> hfft2 (const cmat<F>& data0, const cmat<F>& data1)
    {
        std::size_t R = data0.rows;
        std::size_t COLS = data0.cols;

        auto [g00, g01] = dft_nst1 (data0);
        auto [g10, g11] = dft_nst1 (data1);

        cmat<F> X0 = nst2 (g00);
        cmat<F> t0 = nst2 (g10);
        cmat<F> W0 = w_matrix<F> (0, R, COLS);
        for (std::size_t i = 0; i < X0.data.size(); ++i) { X0.data[i] += W0.data[i] * t0.data[i]; }

        cmat<F> X1 = nst3 (g01);
        cmat<F> t1 = nst3 (g11);
        cmat<F> W1 = w_matrix<F> (1, R, COLS);
        for (std::size_t i = 0; i < X1.data.size(); ++i) { X1.data[i] += W1.data[i] * t1.data[i]; }

        return { X0, X1 };
    }

    template<typename F>
    std::pair<cmat<F>, cmat<F>> idft_inst1 (const cmat<F>& data)
    {
        cmat<F> padded = pad_zeros (data);
        dft_rows (padded, true);
        for (auto& v : padded.data) { v *= F{2}; }
        return { decimate_cols (padded, 0), decimate_cols (padded, 1) };
    }

    template<typename F>
    cmat<F> inst2 (const cmat<F>& data)
    {
        cmat<F> folded = fold_half (data, true);
        dft_cols (folded, true);
        for (auto& v : folded.data) { v *= F{0.5}; }
        return repmat2 (folded);
    }

    template<typename F>
    cmat<F> inst3 (const cmat<F>& data)
    {
        std::size_t R = data.rows;
        cmat<F> folded = fold_half (data, false);
        for (std::size_t r = 0; r < folded.rows; ++r) {
            F ang = F{2} * sm::mathconst<F>::pi * static_cast<F>(r) / static_cast<F>(R);
            std::complex<F> coeff (std::cos (ang), std::sin (ang));
            for (std::size_t c = 0; c < folded.cols; ++c) { folded(r, c) *= coeff; }
        }
        dft_cols (folded, true);
        for (auto& v : folded.data) { v *= F{0.5}; }
        return repmat2 (folded);
    }

    template<typename F>
    cmat<F> iw_matrix (int a, std::size_t R, std::size_t COLS)
    {
        cmat<F> out (R, COLS);
        for (std::size_t r = 0; r < R; ++r) {
            for (std::size_t c = 0; c < COLS; ++c) {
                F ang = sm::mathconst<F>::pi * ( (static_cast<F>(a) + F{2} * static_cast<F>(c)) / (F{2} * static_cast<F>(COLS))
                                                + (static_cast<F>(a) + F{2} * static_cast<F>(r)) / static_cast<F>(R) );
                out(r, c) = std::complex<F> (std::cos (ang), std::sin (ang));
            }
        }
        return out;
    }

    //! The inverse transform, undoing hfft2.
    template<typename F>
    std::pair<cmat<F>, cmat<F>> ihfft2 (const cmat<F>& X0, const cmat<F>& X1)
    {
        std::size_t R = X0.rows;
        std::size_t COLS = X0.cols;

        auto [g00, g01] = idft_inst1 (X0);
        auto [g10, g11] = idft_inst1 (X1);

        cmat<F> a0 = inst2 (g00);
        cmat<F> t0 = inst2 (g10);
        cmat<F> IW0 = iw_matrix<F> (0, R, COLS);
        cmat<F> out0 (R, COLS);
        for (std::size_t i = 0; i < out0.data.size(); ++i) { out0.data[i] = F{0.5} * (a0.data[i] + IW0.data[i] * t0.data[i]); }

        cmat<F> a1 = inst3 (g01);
        cmat<F> t1 = inst3 (g11);
        cmat<F> IW1 = iw_matrix<F> (1, R, COLS);
        cmat<F> out1 (R, COLS);
        for (std::size_t i = 0; i < out1.data.size(); ++i) { out1.data[i] = F{0.5} * (a1.data[i] + IW1.data[i] * t1.data[i]); }

        return { out0, out1 };
    }

    //! Find the smallest rectangle in (hex::ri, hex::gi) index space that encloses every hex
    //! in hg, and round its row count up so that n (the number of rows in each of the two
    //! row-parity arrays) is even and at least 2, as required by fold_half.
    template<typename F>
    void bounding_box (const sm::hexgrid<F>& hg, std::int32_t& ri_min, std::int32_t& gi_min,
                       std::uint32_t& n, std::uint32_t& m)
    {
        if (hg.hexen.empty()) {
            throw std::runtime_error ("sm::hexfft: hexgrid has no hexes");
        }
        std::int32_t ri_max = 0;
        std::int32_t gi_max = 0;
        bool first = true;
        for (const auto& h : hg.hexen) {
            if (first) {
                ri_min = ri_max = h.ri;
                gi_min = gi_max = h.gi;
                first = false;
            } else {
                ri_min = std::min (ri_min, h.ri);
                ri_max = std::max (ri_max, h.ri);
                gi_min = std::min (gi_min, h.gi);
                gi_max = std::max (gi_max, h.gi);
            }
        }
        m = static_cast<std::uint32_t> (ri_max - ri_min + 1);
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
    sm::vvec<std::complex<F>> flatten (const cmat<F>& d0, const cmat<F>& d1)
    {
        std::size_t plane = d0.rows * d0.cols;
        sm::vvec<std::complex<F>> out (2 * plane);
        for (std::size_t i = 0; i < plane; ++i) {
            out[i] = d0.data[i];
            out[plane + i] = d1.data[i];
        }
        return out;
    }

    //! The inverse of flatten.
    template<typename F>
    std::pair<cmat<F>, cmat<F>> unflatten (const sm::vvec<std::complex<F>>& data, std::uint32_t n, std::uint32_t m)
    {
        std::size_t plane = static_cast<std::size_t> (n) * static_cast<std::size_t> (m);
        if (data.size() != 2 * plane) {
            throw std::runtime_error ("sm::hexfft: spectrum data size does not match its n, m");
        }
        cmat<F> d0 (n, m);
        cmat<F> d1 (n, m);
        for (std::size_t i = 0; i < plane; ++i) {
            d0.data[i] = data[i];
            d1.data[i] = data[plane + i];
        }
        return { d0, d1 };
    }

} // sm::hexfft::detail

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
        if (data.size() != hg.num()) {
            std::stringstream ee;
            ee << "sm::hexfft::fft: data.size() (" << data.size() << ") does not match hg.num() (" << hg.num() << ")";
            throw std::runtime_error (ee.str());
        }

        spectrum<F> result;
        detail::bounding_box (hg, result.ri_min, result.gi_min, result.n, result.m);

        detail::cmat<F> d0 (result.n, result.m);
        detail::cmat<F> d1 (result.n, result.m);
        for (const auto& h : hg.hexen) {
            std::uint32_t gr = static_cast<std::uint32_t> (h.gi - result.gi_min);
            std::uint32_t c = static_cast<std::uint32_t> (h.ri - result.ri_min);
            std::uint32_t r = gr / 2u;
            if ((gr % 2u) == 0u) { d0 (r, c) = data[h.vi]; } else { d1 (r, c) = data[h.vi]; }
        }

        auto [X0, X1] = detail::hfft2 (d0, d1);
        result.data = detail::flatten (X0, X1);
        return result;
    }

    //! As above, but for real-valued input data.
    template<typename F>
    spectrum<F> fft (const sm::hexgrid<F>& hg, const sm::vvec<F>& data)
    {
        sm::vvec<std::complex<F>> cdata (data.size());
        for (std::size_t i = 0; i < data.size(); ++i) { cdata[i] = std::complex<F> (data[i], F{0}); }
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
        auto [d0, d1] = detail::unflatten (X.data, X.n, X.m);
        auto [x0, x1] = detail::ihfft2 (d0, d1);

        sm::vvec<std::complex<F>> out (hg.num());
        for (const auto& h : hg.hexen) {
            std::uint32_t gr = static_cast<std::uint32_t> (h.gi - X.gi_min);
            std::uint32_t c = static_cast<std::uint32_t> (h.ri - X.ri_min);
            std::uint32_t r = gr / 2u;
            if (r >= X.n || c >= X.m) {
                throw std::runtime_error ("sm::hexfft::ifft: hg has a hex outside the spectrum's bounding rectangle");
            }
            out[h.vi] = (gr % 2u) == 0u ? x0 (r, c) : x1 (r, c);
        }
        return out;
    }

} // sm::hexfft
