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
 * sm::hexgrid is used to build the physical hex lattice and, in particular, to
 * supply the E/NE/NW hex-to-hex neighbour relationships that are followed to
 * lay the input samples out into the two rectangular arrays that the
 * algorithm requires.
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
#include <list>
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

} // sm::hexfft::detail

export namespace sm::hexfft
{
    /*!
     * A hexagonal grid, arranged for use with sm::hexfft::fft/ifft.
     *
     * The samples form a parallelogram-shaped region of a hex lattice, spanning 2*n rows
     * (hex::gi running from 0 to 2*n-1) and m columns (hex::ri running from 0 to m-1). This
     * parallelogram is exactly the "two interleaved rectangular arrays" of Birdsong &
     * Rummelt's Array Set Addressing scheme: hexes on even rows (gi even) form one m-wide
     * rectangular array of n rows, and hexes on odd rows (gi odd) form the other.
     *
     * n must be even (and at least 2), since the algorithm recursively folds each of the two
     * arrays in half along its row dimension; m may be any value of at least 1.
     */
    template<typename F = double>
    struct grid
    {
        //! Number of rows in each of the two (even/odd row) sub-arrays. Must be even.
        std::uint32_t n = 0;
        //! Number of columns (the length of a row of hexes).
        std::uint32_t m = 0;
        //! The underlying hex lattice, built large enough to contain the samples used by
        //! this grid, along with their full set of hex-to-hex neighbour relationships.
        sm::hexgrid<F> hg;

        /*!
         * hexes_a[a][r][c] is the iterator into hg.hexen for the sample at array a (0 or 1,
         * selecting the even or odd row set), row r (0..n-1) and column c (0..m-1).
         */
        std::vector<std::vector<typename std::list<sm::hex<F>>::iterator>> hexes0;
        std::vector<std::vector<typename std::list<sm::hex<F>>::iterator>> hexes1;

        grid (std::uint32_t n_, std::uint32_t m_, F d_ = F{1}) : n(n_), m(m_)
        {
            if (n < 2 || (n % 2) != 0) {
                throw std::runtime_error ("sm::hexfft::grid: n (rows per row-parity array) must be even and at least 2");
            }
            if (m < 1) {
                throw std::runtime_error ("sm::hexfft::grid: m (number of columns) must be at least 1");
            }

            // A generous span (radius, really, doubled to make a diameter) so that the
            // hexgrid, built with no boundary applied, comfortably contains every hex out to
            // ri == m-1, gi == 2*n-1 with all of its neighbour relations intact.
            F span = d_ * static_cast<F> (2 * (this->m + 3 * this->n) + 20);
            this->hg.init (d_, span);

            this->build_index();
        }

        grid (const grid&) = delete;
        grid& operator= (const grid&) = delete;
        grid (grid&&) = default;
        grid& operator= (grid&&) = default;

        //! The total number of samples handled by this grid (2 * n * m).
        std::uint32_t size() const { return 2u * this->n * this->m; }

    private:
        void build_index()
        {
            this->hexes0.assign (this->n, std::vector<typename std::list<sm::hex<F>>::iterator> (this->m));
            this->hexes1.assign (this->n, std::vector<typename std::list<sm::hex<F>>::iterator> (this->m));

            typename std::list<sm::hex<F>>::iterator h0 = this->hg.hexen.begin(); // hex at ri=0, gi=0

            this->lay_out_rows (h0, this->hexes0);

            if (!h0->has_nne()) {
                throw std::runtime_error ("sm::hexfft::grid: internal hexgrid is too small (nne from origin)");
            }
            this->lay_out_rows (h0->nne, this->hexes1); // hex at ri=0, gi=1
        }

        //! Starting from rowstart (column 0 of row 0 of this sub-array), walk the E neighbour
        //! to fill each row, and two successive NE neighbours (a net displacement of ri+=0,
        //! gi+=2) to move up from one row to the next, storing iterators into dest.
        void lay_out_rows (typename std::list<sm::hex<F>>::iterator rowstart,
                           std::vector<std::vector<typename std::list<sm::hex<F>>::iterator>>& dest)
        {
            for (std::uint32_t r = 0; r < this->n; ++r) {
                typename std::list<sm::hex<F>>::iterator h = rowstart;
                for (std::uint32_t c = 0; c < this->m; ++c) {
                    dest[r][c] = h;
                    if (c + 1 < this->m) {
                        if (!h->has_ne()) {
                            throw std::runtime_error ("sm::hexfft::grid: internal hexgrid is too small (ne)");
                        }
                        h = h->ne;
                    }
                }
                if (r + 1 < this->n) {
                    if (!rowstart->has_nne() || !rowstart->nne->has_nne()) {
                        throw std::runtime_error ("sm::hexfft::grid: internal hexgrid is too small (nne/nne)");
                    }
                    rowstart = rowstart->nne->nne;
                }
            }
        }
    };

    //! Copy data (indexed as data[a * g.n * g.m + r * g.m + c]) into the pair of matrices
    //! used internally by the transform.
    template<typename F>
    std::pair<detail::cmat<F>, detail::cmat<F>> unpack (const grid<F>& g, const sm::vvec<std::complex<F>>& data)
    {
        if (data.size() != g.size()) {
            std::stringstream ee;
            ee << "sm::hexfft: data.size() (" << data.size() << ") does not match grid size (" << g.size() << ")";
            throw std::runtime_error (ee.str());
        }
        detail::cmat<F> d0 (g.n, g.m);
        detail::cmat<F> d1 (g.n, g.m);
        std::size_t plane = static_cast<std::size_t>(g.n) * static_cast<std::size_t>(g.m);
        for (std::size_t r = 0; r < g.n; ++r) {
            for (std::size_t c = 0; c < g.m; ++c) {
                d0(r, c) = data[r * g.m + c];
                d1(r, c) = data[plane + r * g.m + c];
            }
        }
        return { d0, d1 };
    }

    //! The inverse of unpack: flatten a pair of (a=0, a=1) matrices back into a single vvec.
    template<typename F>
    sm::vvec<std::complex<F>> pack (const grid<F>& g, const detail::cmat<F>& d0, const detail::cmat<F>& d1)
    {
        sm::vvec<std::complex<F>> out (g.size());
        std::size_t plane = static_cast<std::size_t>(g.n) * static_cast<std::size_t>(g.m);
        for (std::size_t r = 0; r < g.n; ++r) {
            for (std::size_t c = 0; c < g.m; ++c) {
                out[r * g.m + c] = d0(r, c);
                out[plane + r * g.m + c] = d1(r, c);
            }
        }
        return out;
    }

    /*!
     * Compute the hexagonal FFT of the complex-valued data on grid g.
     *
     * data must have g.size() elements, laid out as data[a * g.n * g.m + r * g.m + c], where
     * a selects the even (0) or odd (1) row sub-array (see sm::hexfft::grid), r is the row
     * (0..g.n-1) and c is the column (0..g.m-1); g.hexes0[r][c]/g.hexes1[r][c] give the
     * corresponding hex (and hence its Cartesian x,y location) in g.hg.
     *
     * The result has the same layout and size as the input.
     */
    template<typename F>
    sm::vvec<std::complex<F>> fft (const grid<F>& g, const sm::vvec<std::complex<F>>& data)
    {
        auto [d0, d1] = unpack (g, data);
        auto [X0, X1] = detail::hfft2 (d0, d1);
        return pack (g, X0, X1);
    }

    //! As above, but for real-valued input data.
    template<typename F>
    sm::vvec<std::complex<F>> fft (const grid<F>& g, const sm::vvec<F>& data)
    {
        sm::vvec<std::complex<F>> cdata (data.size());
        for (std::size_t i = 0; i < data.size(); ++i) { cdata[i] = std::complex<F> (data[i], F{0}); }
        return sm::hexfft::fft (g, cdata);
    }

    //! Compute the inverse hexagonal FFT, undoing sm::hexfft::fft. See sm::hexfft::fft for
    //! the data layout.
    template<typename F>
    sm::vvec<std::complex<F>> ifft (const grid<F>& g, const sm::vvec<std::complex<F>>& data)
    {
        auto [X0, X1] = unpack (g, data);
        auto [d0, d1] = detail::ihfft2 (X0, X1);
        return pack (g, d0, d1);
    }

} // sm::hexfft
