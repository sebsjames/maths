// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * An implementation of the hexagonal fast Fourier transform decribed in Nicholas I. Rummelt's PhD
 * thesis "Array set addressing: Enabling efficient hexagonally sampled image processing",
 * University of Florida, 2010.
 *
 * \author: AI took some code from Josua Grawitter in https://github.com/gwater/HexFFT.jl and made a
 * conversion to C++. This code generated an FFT in the twin rectangular 'ASA' arrays. Seb made
 * sense of this and arranged the round-trip from hexgrid to ASA to frequency hexgrid and then back again.
 *
 * \date: September 2026
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
import sm.fft;

namespace sm::hexfft::internal
{
    // 'Non standard transform 1' Fourier transform row by row, then decimate_cols.
    template<typename F>
    std::pair<sm::vmat<std::complex<F>>, sm::vmat<std::complex<F>>> nst1 (const sm::vmat<std::complex<F>>& data)
    {
        sm::vmat<std::complex<F>> padded = sm::fft::pad_zeros (data);
        sm::fft::dft_rows (padded, false);
        return { sm::fft::decimate_cols (padded, 0), sm::fft::decimate_cols (padded, 1) };
    }

    // 'Non standard transform 2'
    template<typename F>
    sm::vmat<std::complex<F>> nst2 (const sm::vmat<std::complex<F>>& data)
    {
        sm::vmat<std::complex<F>> folded = sm::fft::fold_half (data, true);
        sm::fft::dft_cols (folded, false);
        return sm::fft::repmat2 (folded);
    }

    // 'Non standard transform 3' Eqn 4-11 in Nick's thesis
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
     * The data structure for a hexagonal FFT
     */
    template<typename F = double, bool construct_hg_asa = false>
    struct fft
    {
        //! Rows in each of the two ASA (Array Set Addressing) grids.
        std::uint32_t asa_rows = 0;
        //! Columns in the ASA grids.
        std::uint32_t asa_cols = 0;

        //! The hex::ri, hex::gi of the padded rectangle's (a=0, r=0, c=0) corner.
        std::int32_t ri_min = 0;
        std::int32_t gi_min = 0;

        //! Non-owning pointer to the input image hexgrid. User defines this and its shape.
        sm::hexgrid<F, sm::hexalign::point_up>* hg = nullptr;

        //! For debug/viz, we construct an ASA-compliant hexgrid from the input image hexgrid.
        std::unique_ptr<sm::hexgrid<F, sm::hexalign::point_up>> hg_asa;

        // Input data arranged to show with hg_asa
        sm::vvec<F> data_asa_real;
        sm::vvec<F> data_asa_imag;

        //! This holds the input (image/spatial) data in the twin rectangular grids (ASA: array set addressing grids)
        sm::vmat<std::complex<F>> d0; // even rows
        sm::vmat<std::complex<F>> d1; // odd rows

        //! FFT in twin rectangular ASA grids. Could be temporary, but saved to enable plotting/debugging
        sm::vmat<std::complex<F>> X0; // even rows
        sm::vmat<std::complex<F>> X1; // odd rows

        //! We construct a frequency hexgrid from the ASA compliant hexgrid.
        std::unique_ptr<sm::hexgrid<F, sm::hexalign::flat_up>> hgf;

        //! Scaling factor (obtained from image data hexgrid spacing) for the frequency hexgrid, hgf
        F Uscale = F{1};

        //! FFT result, suitable for visualization on the frequency space hexgrid, hgf
        sm::vvec<std::complex<F>> X_hexgrid;

        //! The total number of samples in the padded rectangle (2 * r * c). same as hgs->num()
        std::uint32_t size() const { return 2u * this->asa_rows * this->asa_cols; }

        //! Initialize hexgrids ready to compute the forward FFT for data on the grid _hg.
        void init (sm::hexgrid<F, sm::hexalign::point_up>* _hg)
        {
            if (this->hg == _hg) { return; }

            this->hg = _hg;

            this->bounding_box();

            std::cout << "internal::bounding_box computes: ri_min: " << this->ri_min << ", gi_min: " << this->gi_min
                      << " rows: " << this->asa_rows << ", cols: " << this->asa_cols << std::endl;

            if constexpr (construct_hg_asa) {
                // Make the equivalent hexgrid for the ASA.
                this->hg_asa = std::make_unique<sm::hexgrid<F, sm::hexalign::point_up>>(this->hg->d, this->asa_cols * 4 * this->hg->d, 0.0f);
                this->hg_asa->set_rectangular_boundary (this->asa_cols, this->asa_rows * 2u);
                if (this->hg_asa->num() != this->asa_cols * this->asa_rows * 2u) {
                    throw std::runtime_error ("hg_asa has wrong number of elements");
                }
                std::cout << "After set rect boundary, hg_asa size is " << this->hg_asa->num() << " = " << this->asa_cols * this->asa_rows * 2u << std::endl;
            }

            // Construct a frequency hexgrid
            auto V = sm::hexfft::make_V<float>();
            V *= this->hg->d;
            const sm::mat<F, 2, 2> U = sm::hexfft::make_U<F>(V);
            this->Uscale = V.col(0).length() * V.col(0).length(); // a suitable scaling (zoom factor) for the frequency hexgrid
            if (this->hgf) { this->hgf.release(); }

            this->hgf = std::make_unique<sm::hexgrid<F, sm::hexalign::flat_up>>(U.col(0).length(),
                                                                                this->asa_cols * 4 * U.col(0).length(),
                                                                                0.0f);
            this->hgf->set_rectangular_boundary (this->asa_rows * 2u, this->asa_cols, this->asa_rows, this->asa_cols / 2u);

            std::cout << "Frequency hexgrid has width " << this->hgf->width() << " [units 1/L]" << std::endl;
        }

        /*!
         * Compute the hexagonal FFT of data, sampled on the (possibly arbitrarily bounded)
         * hexgrid hg. data must be indexed by hex::vi, as usual for hexgrid client data (so
         * data.size() == hg.num()).
         */
        void forward (const sm::vvec<std::complex<F>>& data)
        {
            if (this->hg == nullptr) { throw std::runtime_error ("fft: Uninitialized"); }

            // Save the input data after it has been extracted into ASA format
            this->image_hexgrid_to_asa (data);

            // If we are working with hg_asa, also write the data into data_asa_real:
            if constexpr (construct_hg_asa) { this->image_asa_to_hg_asa(); }

            // Fourier transform the ASA formatted data into an ASA formatted result (this->X_asa)
            std::tie(this->X0, this->X1) = internal::hfft2 (this->d0, this->d1);
            // Re-quadrant X_asa before putting it on hexgrid
            this->re_quadrant();

            // Populated a frequency space hexgrid with this->X_asa
            this->frequency_asa_to_hexgrid();
        }

        //! As above, but for real-valued input data.
        void forward (const sm::vvec<F>& data)
        {
            sm::vvec<std::complex<F>> cdata (data.size());
            for (std::uint32_t i = 0; i < data.size(); ++i) { cdata[i] = std::complex<F> (data[i], F{0}); }
            this->forward (cdata);
        }

        // Inverse FFT returns image data defined over the original hexgrid hg.
        sm::vvec<std::complex<F>> inverse()
        {
            this->X0.set_zero();
            this->X1.set_zero();
            this->d0.set_zero();
            this->d1.set_zero();

            // 1. Copy values from this->X_hexgrid into X0 and X1 (the ASA grids)
            this->frequency_hexgrid_to_asa();

            // Switch X into the quadranted data that the hfft2/ihfft2 functions work in
            this->de_quadrant();

            // 2. Inverse hexagonal FFT
            std::tie(this->d0, this->d1) = internal::ihfft2 (this->X0, this->X1);

            // Optional hg_asa representation
            if constexpr (construct_hg_asa) { this->image_asa_to_hg_asa(); }

            // Last job - convert from d_asa to image hexgrid
            return this->image_asa_to_hexgrid();
        }

    private:

        // Find parameters for the enclsing hexgrid (hg_asa)
        void bounding_box()
        {
            if (this->hg == nullptr) { throw std::runtime_error ("init first"); }

            if (this->hg->hexen.empty()) {
                throw std::runtime_error ("sm::hexfft: hexgrid has no hexes");
            }

            // Find gi_min (and max)
            auto gi_max = std::numeric_limits<std::int32_t>::lowest();
            for (const auto& h : this->hg->hexen) {
                gi_min = std::min (gi_min, h.gi);
                gi_max = std::max (gi_max, h.gi);
            }

            // Find ri_min
            auto c_min = std::numeric_limits<std::int32_t>::max();
            auto c_max = std::numeric_limits<std::int32_t>::lowest();
            for (const auto& h : this->hg->hexen) {
                std::int32_t r = (h.gi - gi_min) / 2;
                std::int32_t c = h.ri + r;
                c_min = std::min (c_min, c);
                c_max = std::max (c_max, c);
            }
            ri_min = c_min;

            this->asa_cols = static_cast<std::uint32_t> (c_max - c_min + 1);
            if (this->asa_cols % 2 != 0) {
                ++this->asa_cols;
            }

            auto rows = static_cast<std::uint32_t> (gi_max - gi_min + 1);
            this->asa_rows = (rows + 1u) / 2u;
            if (this->asa_rows < 2u) {
                this->asa_rows = 2u;
            } else if ((this->asa_rows % 2u) != 0u) {
                ++this->asa_rows;
            }
        }

        typename std::list<sm::hex<F, sm::hexalign::flat_up>>::iterator
        find_frequency_hexgrid_start()
        {
            // Begin at the start of hexen, which will be somewhere near the middle of the hexgrid.
            typename std::list<sm::hex<F, sm::hexalign::flat_up>>::iterator hi = this->hgf->hexen.begin();
            // Now move in the -red direction until not able, then in the -g direction until not
            // able. This should be the bottom left of a flat_up grid.
            while (hi->has_n3()) { hi = hi->n3; }
            while (hi->has_n2()) { hi = hi->n2; } // in case we hit the bottom of the grid before the left
            while (hi->has_n4()) { hi = hi->n4; }
            return hi;
        }

        typename std::list<sm::hex<F, sm::hexalign::point_up>>::iterator
        find_image_hexgrid_start()
        {
            typename std::list<sm::hex<F, sm::hexalign::point_up>>::iterator hi = this->hg_asa->hexen.begin();
            while (hi->has_n5()) { hi = hi->n5; } // -b
            while (hi->has_n4()) { hi = hi->n4; } // -g
            while (hi->has_n3()) { hi = hi->n3; } // -r
            return hi;
        }

        void image_asa_to_hg_asa()
        {
            this->data_asa_real.resize (this->asa_rows * this->asa_cols * 2u, F{0});

            auto hi = this->find_image_hexgrid_start();
            std::uint32_t r = 0u; // row on hex grid. row on ASA is r / 2.
            std::uint32_t c = 0u;
            bool done = false;
            while (!done) {
                if (r % 2u == 0u) {

                    while (hi->has_n0()) {
                        if (hi->vi < this->data_asa_real.size()) {
                            this->data_asa_real[hi->vi] = std::real (this->d0(r / 2, c));
                        }
                        c++;
                        hi = hi->n0;
                    }
                    if (hi->vi < this->data_asa_real.size()) {
                        this->data_asa_real[hi->vi] = std::real (this->d0(r / 2, c));
                    }
                    c++;

                    if (hi->has_n1()) {
                        hi = hi->n1;
                        r++;
                    } else {
                        done = true;
                    }

                } else {
                    // Walk back along a col
                    while (hi->has_n3()) {
                        --c;
                        if (hi->vi < this->data_asa_real.size()) {
                            this->data_asa_real[hi->vi] = std::real (this->d1(r / 2, c));
                        }
                        hi = hi->n3;
                    }
                    --c;
                    if (hi->vi < this->data_asa_real.size()) {
                        this->data_asa_real[hi->vi] = std::real (this->d1(r / 2, c));
                    }

                    if (hi->has_n2()) {
                        hi = hi->n2;
                        r++;
                    } else {
                        done = true;
                    }
                }
            }
        }

        // Copy data in d0/d1 ASA grids into the output data and return it
        sm::vvec<std::complex<F>> image_asa_to_hexgrid()
        {
            if (this->hg == nullptr) { throw std::runtime_error ("Initialize fft first"); }

            sm::vvec<std::complex<F>> himg (this->hg->num());

            for (std::uint32_t r = 0; r < this->asa_rows; ++r) {
                for (std::uint32_t c = 0; c < this->asa_cols; ++c) {
                    sm::vec<std::int32_t, 2> rigi = internal::asa_to_ri_gi (0u, r, c, this->ri_min, this->gi_min);

                    auto hi = this->hg->find_hex_at (rigi.plus_one_dim());
                    if (hi != this->hg->hexen.end()) {
                        if (hi->vi < himg.size()) { himg[hi->vi] = this->d0(r, c); }
                    }

                    rigi = internal::asa_to_ri_gi (1u, r, c, this->ri_min, this->gi_min);
                    hi = this->hg->find_hex_at (rigi.plus_one_dim());
                    if (hi != this->hg->hexen.end()) {
                        if (hi->vi < himg.size()) { himg[hi->vi] = this->d1(r, c); }
                    }
                }
            }

            return himg;
        }

        // Copy the data, defined over the input image hexgrid, hg, into d0/d1. The ASA grid may be
        // larger than the image hexgrid; elements for which there is no input value on hg are set
        // to 0.
        void image_hexgrid_to_asa (const sm::vvec<std::complex<F>>& data)
        {
            if (this->hg == nullptr) { throw std::runtime_error ("Initialize fft first"); }

            if (data.size() != this->hg->num()) {
                std::stringstream ee;
                ee << "sm::hexfft: data.size() (" << data.size() << ") does not match hg->num() (" << this->hg->num() << ")";
                throw std::runtime_error (ee.str());
            }
            this->d0.resize (this->asa_rows, this->asa_cols);
            this->d1.resize (this->asa_rows, this->asa_cols);
            sm::vec<std::uint32_t> arc = {};
            //std::cout << "For each hex, do ri_gi_to_asa with ri_min = " << ri_min << " and gi_min = " << gi_min << std::endl;
            for (const auto& h : this->hg->hexen) {
                arc = internal::ri_gi_to_asa (h.ri, h.gi, this->ri_min, this->gi_min);
                //std::cout << "rg(" << h.ri << "," << h.gi << ") maps to arc " << arc << std::endl;
                if (arc[0] == 0u) {
                    if (h.vi < data.size()) {
                        this->d0 (arc[1], arc[2]) = data[h.vi];
                    } else {
                        std::cout << "NOT Setting d0 (" << arc[1] << ", " << arc[2] << ")" << std::endl;
                    }
                } else {
                    if (h.vi < data.size()) {
                        this->d1 (arc[1], arc[2]) = data[h.vi];
                    } else {
                        std::cout << "NOT Setting d1 (" << arc[1] << ", " << arc[2] << ")" << std::endl;
                    }
                }
            }
        }

        // Copy ASA laid-out frequency data into X_hexgrid (over the hexgrid hgf).
        void frequency_asa_to_hexgrid()
        {
            auto sz = this->hgf->num();
            this->X_hexgrid.resize (sz, std::complex<F>{0,0});

            // We use neighbour info: Find bottom-left hex and then raster up/down each row, filling X0/X1 in turn.
            auto hi = this->find_frequency_hexgrid_start();
            std::uint32_t r = 0u; // row on hex grid. row on ASA is r / 2.
            std::uint32_t c = 0u;
            bool done = false;
            while (!done) {
                if (r % 2u == 0u) {
                    // Walk up a hex col
                    while (hi->has_n1()) {
                        if (hi->vi < this->X_hexgrid.size()) {
                            this->X_hexgrid[hi->vi] = this->X0 (r / 2, c);
                        }
                        c++;
                        hi = hi->n1;
                    }
                    if (hi->vi < this->X_hexgrid.size()) {
                        this->X_hexgrid[hi->vi] = this->X0 (r / 2, c);
                    }
                    c++;

                    if (hi->has_n5()) {
                        hi = hi->n5;
                        r++;
                    } else {
                        done = true;
                    }

                } else {
                    // Walk down a col
                    while (hi->has_n4()) {
                        --c;
                        if (hi->vi < this->X_hexgrid.size()) {
                            this->X_hexgrid[hi->vi] = this->X1 (r / 2, c);
                        }
                        hi = hi->n4;
                    }
                    --c;
                    if (hi->vi < this->X_hexgrid.size()) {
                        this->X_hexgrid[hi->vi] = this->X1 (r / 2, c);
                    }

                    if (hi->has_n0()) {
                        hi = hi->n0;
                        r++;
                    } else {
                        done = true;
                    }
                }
            }
        }

        void frequency_hexgrid_to_asa()
        {
            if (this->X_hexgrid.size() != this->hgf->num()) {
                std::stringstream ee;
                ee << "sm::hexfft: X_hexgrid.size() (" << this->X_hexgrid.size() << ") does not match hgf->num() (" << this->hgf->num() << ")";
                throw std::runtime_error (ee.str());
            }

            // We use neighbour info: Find bottom-left hex and then raster up/down each row, filling X0/X1 in turn.
            auto hi = this->find_frequency_hexgrid_start();
            std::uint32_t r = 0u; // row on hex grid. row on ASA is r / 2.
            std::uint32_t c = 0u;
            bool done = false;
            while (!done) {
                if (r % 2u == 0u) {
                    // Walk up a hex col
                    while (hi->has_n1()) {
                        if (hi->vi < this->X_hexgrid.size()) {
                            this->X0(r / 2, c) = this->X_hexgrid[hi->vi];
                        }
                        c++;
                        hi = hi->n1;
                    }
                    if (hi->vi < this->X_hexgrid.size()) {
                        this->X0(r / 2, c) = this->X_hexgrid[hi->vi];
                    }
                    c++;

                    if (hi->has_n5()) {
                        hi = hi->n5;
                        r++;
                    } else {
                        done = true;
                    }

                } else {
                    // Walk down a col
                    while (hi->has_n4()) {
                        --c;
                        if (hi->vi < this->X_hexgrid.size()) {
                            this->X1(r / 2, c) = this->X_hexgrid[hi->vi];
                        }
                        hi = hi->n4;
                    }
                    --c;
                    if (hi->vi < this->X_hexgrid.size()) {
                        this->X1(r / 2, c) = this->X_hexgrid[hi->vi];
                    }

                    if (hi->has_n0()) {
                        hi = hi->n0;
                        r++;
                    } else {
                        done = true;
                    }
                }
            }
        }

        // Re-arrange ASA data in X0/X1 so that it is in a human-readable quadrant arrangement
        void re_quadrant()
        {
            // cmat has rows, cols and vvec<> data
            if (X1.cols() % 2) {
                std::cout << "cols not divisible by 2\n";
                return;
            }
            if (X1.rows() % 2) {
                std::cout << "rows not divisible by 2\n";
                return;
            }

            if ((X0.rows() != X1.rows()) || (X0.cols() != X1.cols())) {
                std::cerr << "re_quadrant: Size mismatch, returning without changing anything\n";
                return;
            }

            sm::vmat<std::complex<F>> _X0 (X0.rows(), X0.cols());
            sm::vmat<std::complex<F>> _X1 (X1.rows(), X1.cols());

            std::uint32_t hcols = X1.cols() / 2;
            std::uint32_t hrows = X1.rows() / 2;

            for (std::uint32_t c = 0; c < hcols; c++) {
                for (std::uint32_t r = 0; r < hrows; r++) {
                    _X0(r + hrows, c + hcols) = X0(r, c);
                    _X1(r + hrows, c + hcols) = X1(r, c);
                    _X0(r, c) = X0(r, c + hcols);
                    _X1(r, c) = X1(r, c + hcols);
                    _X0(r + hrows, c) = X0(r + hrows, c + hcols);
                    _X1(r + hrows, c) = X1(r + hrows, c + hcols);
                    _X0(r, c + hcols) = X0(r + hrows, c);
                    _X1(r, c + hcols) = X1(r + hrows, c);
                }
            }
            this->X0 = _X0;
            this->X1 = _X1;
        }

        // Reverse of re_quadrant
        void de_quadrant()
        {
            if (X1.cols() % 2) {
                std::cout << "cols not divisible by 2\n";
                return;
            }
            if (X1.rows() % 2) {
                std::cout << "rows not divisible by 2\n";
                return;
            }

            if ((X0.rows() != X1.rows()) || (X0.cols() != X1.cols())) {
                std::cerr << "de_quadrant: Size mismatch, returning without changing anything\n";
                return;
            }

            sm::vmat<std::complex<F>> _X0 (X0.rows(), X0.cols());
            sm::vmat<std::complex<F>> _X1 (X1.rows(), X1.cols());

            std::uint32_t hcols = X1.cols() / 2;
            std::uint32_t hrows = X1.rows() / 2;

            for (std::uint32_t c = 0; c < hcols; c++) {
                for (std::uint32_t r = 0; r < hrows; r++) {
                    _X0(r, c) = X0(r + hrows, c + hcols);
                    _X1(r, c) = X1(r + hrows, c + hcols);
                    _X0(r, c + hcols) = X0(r, c);
                    _X1(r, c + hcols) = X1(r, c);
                    _X0(r + hrows, c + hcols) = X0(r + hrows, c);
                    _X1(r + hrows, c + hcols) = X1(r + hrows, c);
                    _X0(r + hrows, c) = X0(r, c + hcols);
                    _X1(r + hrows, c) = X1(r, c + hcols);
                }
            }
            this->X0 = _X0;
            this->X1 = _X1;
        }
    };

} // sm::hexfft
