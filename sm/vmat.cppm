// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * A runtime-variable-sized matrix class
 *
 * \author Seb James
 * \date 2026
 */
module;

#include <cstdint>
#include <limits>
#include <vector>
#include <complex>
#include <string>
#include <format>

export module sm.vmat;

export import sm.mathconst;
export import sm.vvec;
export import sm.vec;
import sm.trait_tests;
import sm.polysolve;

export namespace sm
{
    // Forward declare class and stream operator
    template <typename F>
    requires (std::is_floating_point_v<F> || sm::is_complex<F>::value == true) struct vmat;

    template <typename F> std::ostream& operator<< (std::ostream&, const vmat<F>&);

    /*!
     * A matrix class with runtime-variable (rather than compile-time fixed) size.
     *
     * \templateparam F Thefloating point or complex type in which to store the vmat's data
     */
    template <typename F>
    requires (std::is_floating_point_v<F> || sm::is_complex<F>::value == true)
    struct vmat
    {
        // Construct
        vmat() = default;
        vmat (const std::uint32_t r, const std::uint32_t c) { this->resize (r, c); }

        // Resize
        void resize (const std::uint32_t r, const std::uint32_t c)
        {
            this->arr.resize (r * c, F{0});
            this->Nc = c;
            this->Nr = r;
        }

        //! set_identity is self-explanatory
        void set_identity()
        {
            if (Nr != Nc) { throw std::runtime_error ("vmat::set_identity: requires square matrix"); }
            this->arr.zero();
            for (std::uint32_t i = 0; i < Nr * Nc; i += Nr + 1) { this->arr[i] = F{1}; }
        }

        //! Reset the whole matrix to zero
        void set_zero() { this->arr.zero(); }

        void set_from (const F el) { this->arr.set_from (el); }

        template <typename T> requires (std::is_floating_point_v<T> || sm::is_complex<T>::value == true)
        sm::vmat<T> as() const
        {
            sm::vmat<T> rtn;
            rtn.resize (this->Nr, this->Nc);
            for (std::uint32_t i = 0; i < this->arr.size(); ++i) {
                rtn.arr[i] = static_cast<T>(this->arr[i]);
            }
            return rtn;
        }

        //! Set element at row r, column c to value val
        void set (const std::uint32_t r, const std::uint32_t c, const F& val)
        {
            if (r + (c * this->Nr) < this->size()) { this->arr[r + (c * this->Nr)] = val; }
        }

        //! Set a row with the passed-in vec
        template<std::uint32_t _Nc>
        void set_row (const std::uint32_t idx, const sm::vec<F, _Nc>& rowdata)
        {
            for (std::uint32_t i = 0; i < Nc; ++i) {
                this->arr[idx + Nr * i] = (i < _Nc ? rowdata[i] : F{0});
            }
        }

        //! Set a row with the passed-in vvec
        void set_row (const std::uint32_t idx, const sm::vvec<F>& rowdata)
        {
            const std::uint32_t _Nc = rowdata.size();
            for (std::uint32_t i = 0; i < Nc; ++i) {
                this->arr[idx + Nr * i] = (i < _Nc ? rowdata[i] : F{0});
            }
        }

        //! Set a col with the passed-in vec
        template<std::uint32_t _Nr>
        void set_col (const std::uint32_t idx, const sm::vec<F, _Nr>& coldata) noexcept
        {
            std::uint32_t arridx_start = idx * Nr;
            for (std::uint32_t i = 0; i < Nr; ++i) {
                this->arr[arridx_start + i] = (i < _Nr ? coldata[i] : F{0});
            }
        }

        //! Set a col with the passed-in vvec
        void set_col (const std::uint32_t idx, const sm::vvec<F>& coldata) noexcept
        {
            const std::uint32_t _Nr = coldata.size();
            std::uint32_t arridx_start = idx * Nr;
            for (std::uint32_t i = 0; i < Nr; ++i) {
                this->arr[arridx_start + i] = (i < _Nr ? coldata[i] : F{0});
            }
        }

        /*!
         * Access with operator returning reference can easily lead to memory bugs. Here, we return
         * ref to last element if r, c are out of range. In the special case that arr is empty, we
         * resize it and return reference to that value.
         */
        F& operator() (std::uint32_t r, std::uint32_t c)
        {
            if (this->arr.size() == 0u) { this->arr.resize (1, F{0}); }
            std::uint64_t idx = r + (c * this->Nr);
            if (idx < this->arr.size()) {
                return this->arr[idx];
            } else {
                return this->arr[this->arr.size() - 1];
            }
        }
        const F& operator() (std::uint32_t r, std::uint32_t c) const { return this->arr[r + (c * this->Nr)]; }

        //! Access elements of the matrix (returns ref, so not const)
        F& operator[] (const std::uint32_t idx) { return this->arr[idx]; }

        //! Access elements of the matrix with const promise
        F operator[] (const std::uint32_t idx) const noexcept
        {
            constexpr F badrtn = std::numeric_limits<F>::has_quiet_NaN ? std::numeric_limits<F>::quiet_NaN() : std::numeric_limits<F>::max();
            return idx < this->arr.size() ? this->arr[idx] : badrtn;
        }
        bool checksize() const { return this->size() == this->arr.size(); }
        std::uint32_t size() const { return this->Nr * this->Nc; }
        sm::vec<std::uint32_t, 2> shape() const { return sm::vec<std::uint32_t, 2>{ this->Nr, this->Nc }; }

        //! Access a given row of the matrix
        sm::vvec<F> row (const std::uint32_t idx) const
        {
            sm::vvec<F> r (this->Nc, F{0});
            if (idx >= this->Nr) { return r; }
            for (std::uint32_t i = 0; i < this->Nc; ++i) { r[i] = this->arr[idx + Nr * i]; }
            return r;
        }
        std::uint32_t rows() const { return this->Nr; }

        //! Access a given column of the matrix
        sm::vvec<F> col (const std::uint32_t idx) const
        {
            sm::vvec<F> c (this->Nr, F{0});
            if (idx >= this->Nc) { return c; }
            std::uint32_t arridx_start = idx * this->Nr;
            for (std::uint32_t i = 0; i < this->Nr; ++i) { c[i] = this->arr[arridx_start++]; }
            return c;
        }
        std::uint32_t cols() const { return this->Nc; }

        //! Right multiply this->arr with m2.arr
        template<typename Fy=F> requires (std::is_arithmetic_v<Fy> || (sm::is_complex<Fy>::value && sm::is_complex<F>::value))
        constexpr vmat<F> operator* (const vmat<Fy>& m2) const
        {
            const std::uint32_t Nry = m2.Nr;
            const std::uint32_t Ncy = m2.Nc;

            if (Nc != Nry) {
                throw std::runtime_error ("vmat::operator*: m1.cols != m2.rows in m1 * m2");
            }

            vmat<F> m (Nr, Ncy);

            // General case left matrix is this->arr, right matrix is m2.arr
            for (std::uint32_t r = 0; r < Nr; ++r) {         // row in result matrix
                for (std::uint32_t cy = 0; cy < Ncy; ++cy) { // col in result matrix
                    m.arr[(cy * Nr) + r] = F{0};
                    for (std::uint32_t i = 0; i < Nc; ++i) { // i iterates along row in left matrix and down col in right matrix
                        m.arr[(cy * Nr) + r] += this->arr[(i * Nr) + r] * m2.arr[(cy * Nry) + i];
                    }
                }
            }

            return m;
        }

        //! *= operator for a scalar value.
        template <typename T=F> requires (std::is_arithmetic_v<T> || (sm::is_complex<T>::value && sm::is_complex<F>::value))
        void operator*= (const T& f) noexcept
        {
            for (std::uint32_t i = 0; i < Nr * Nc; ++i) { this->arr[i] *= f; }
        }

        //! * operator for a scalar value.
        template <typename T=F> requires (std::is_arithmetic_v<T> || (sm::is_complex<T>::value && sm::is_complex<F>::value))
        vmat<F> operator* (const T& f) noexcept
        {
            vmat<F> m (Nr, Nc);
            for (std::uint32_t i = 0; i < Nr * Nc; ++i) { m[i] = this->arr[i] * f; }
            return m;
        }

        //! Transpose this matrix
        void transpose_inplace()
        {
            if (Nr != Nc) { throw std::runtime_error ("vmat::transpose_inplace: valid only for square matrices"); }

            const std::uint32_t N_diamond = ((Nr * Nc) - Nr ) / 2;

            sm::vvec<F> a (N_diamond, F{0});

            std::uint32_t i = 0;
            for (std::uint32_t c = 0; c < (Nc / 2) + 1; ++c) {
                for (std::uint32_t r = c + 1; r < Nr; ++r) {
                    a[i++] = this->arr[c * Nr + r];
                    this->arr[c * Nr + r] = this->arr[r * Nr + c];
                }
            }
            i = 0;
            for (std::uint32_t c = 0; c < (Nc / 2) + 1; ++c) {
                for (std::uint32_t r = c + 1; r < Nr; ++r) {
                    this->arr[r * Nr + c] = a[i++];
                }
            }
        }

        //! Return the matrix transposed
        vmat<F> transpose() const
        {
            if (Nr != Nc) { throw std::runtime_error ("vmat::transpose: valid only for square matrices"); }

            const std::uint32_t N_diamond = ((Nr * Nc) - Nr ) / 2;

            sm::vvec<F> a (N_diamond, F{0});

            vmat<F> m = *this;

            std::uint32_t i = 0;
            for (std::uint32_t c = 0; c < (Nc / 2) + 1; ++c) {
                for (std::uint32_t r = c + 1; r < Nr; ++r) {
                    a[i++] = this->arr[c * Nr + r];
                    m[c * Nr + r] = this->arr[r * Nr + c];
                }
            }
            i = 0;
            for (std::uint32_t c = 0; c < (Nc / 2) + 1; ++c) {
                for (std::uint32_t r = c + 1; r < Nr; ++r) {
                    m[r * Nr + c] = a[i++];
                }
            }

            return m;
        }

        //! Transpose the matrix @cm, returning the transposed version.
        static sm::vvec<F> transpose (const sm::vvec<F>& cm, const std::uint32_t _Nr)
        {
            if (cm.size() != _Nr * _Nr) { throw std::runtime_error ("static transpose: cm has wrong size"); }

            const std::uint32_t N_diamond = ((_Nr * _Nr) - _Nr ) / 2;

            sm::vvec<F> a (N_diamond, F{0});

            sm::vvec<F> m (_Nr * _Nr);

            // Copy diagonal to m
            for (std::uint32_t c = 0; c < _Nr; ++c) { m[c * _Nr + c] = cm[c * _Nr + c]; }

            std::uint32_t i = 0;
            for (std::uint32_t c = 0; c < (_Nr / 2) + 1; ++c) {
                for (std::uint32_t r = c + 1; r < _Nr; ++r) {
                    a[i++] = cm[c * _Nr + r];
                    m[c * _Nr + r] = cm[r * _Nr + c];
                }
            }
            i = 0;
            for (std::uint32_t c = 0; c < (_Nr / 2) + 1; ++c) {
                for (std::uint32_t r = c + 1; r < _Nr; ++r) {
                    m[r * _Nr + c] = a[i++];
                }
            }

            return m;
        }

        /*!
         * Return the inverse matrix.
         *
         * Implement inversion using determinant method. inverse is (1/det) x adjugate
         * matrix.
         *
         * 1. Compute determinant of this->arr (if 0, then there's no inverse)
         * 2. Obtain the adjugate matrix
         * 3. Get the inverse by multiplying 1/determinant by the adjugate
         */
        vmat<F> inverse() const
        {
            if (Nr != Nc) { throw std::runtime_error ("vmat::inverse: valid only for square matrices"); }

            F det = this->determinant();
            vmat<F> m (Nr, Nc);
            if (det == F{0}) {
                // The transform matrix has no inverse (determinant is 0)
                m.arr.set_from (F{0});
            } else {
                m.arr = this->adjugate();
                m *= (F{1} / det);
            }
            return m;
        }

        /*!
         * Turn the matrix into its inverse.
         *
         * Implement inversion using determinant method. inverse is (1/det) x adjugate
         * matrix.
         *
         * 1. Compute determinant of this->arr (if 0, then there's no inverse)
         * 2. Obtain the adjugate matrix
         * 3. Get the inverse by multiplying 1/determinant by the adjugate
         */
        void inverse_inplace()
        {
            if (Nr != Nc) { throw std::runtime_error ("vmat::inverse_inplace: valid only for square matrices"); }

            F det = this->determinant();
            if (det == F{0}) {
                // The transform matrix has no inverse (determinant is 0)
                this->arr.set_from (F{0});
            } else {
                this->arr = this->adjugate();
                *this *= (F{1} / det);
            }
        }

        //! Compute determinant for 3x3 or 4x4 matrix @cm
        static F determinant_low_order (const sm::vvec<F>& cm, const std::uint32_t _Nr)
        {
            if (cm.size() != _Nr * _Nr) {
                throw std::runtime_error ("vmat::determinant_low_order: cm.size() != Nr * Nr");
            }

            F det = F{0};

            if (_Nr == 2) {
                det = ((cm[0] * cm[3]) - (cm[1] * cm[2]));

            } else if (_Nr == 3) {
                det = (cm[0] * cm[4] * cm[8])
                + (cm[3] * cm[7] * cm[2])
                + (cm[6] * cm[1] * cm[5])
                - (cm[6] * cm[4] * cm[2])
                - (cm[0] * cm[7] * cm[5])
                - (cm[3] * cm[1] * cm[8]);

            } else if (_Nr == 4) {
                // Configure the 3x3 matrices that have to be evaluated to get the 4x4 det.
                sm::vvec<F> cm33 (9);

                // First 3x3 matrix
                cm33[0] = cm[5];
                cm33[1] = cm[6];
                cm33[2] = cm[7];
                cm33[3] = cm[9];
                cm33[4] = cm[10];
                cm33[5] = cm[11];
                cm33[6] = cm[13];
                cm33[7] = cm[14];
                cm33[8] = cm[15];

                det = cm[0] * vmat<F>::determinant_low_order (cm33, 3u);

                // Second 3x3
                cm33[0] = cm[1];
                cm33[1] = cm[2];
                cm33[2] = cm[3];
                cm33[3] = cm[9];
                cm33[4] = cm[10];
                cm33[5] = cm[11];
                cm33[6] = cm[13];
                cm33[7] = cm[14];
                cm33[8] = cm[15];

                det -= cm[4] * vmat<F>::determinant_low_order (cm33, 3u);

                // Third 3x3
                cm33[0] = cm[1];
                cm33[1] = cm[2];
                cm33[2] = cm[3];
                cm33[3] = cm[5];
                cm33[4] = cm[6];
                cm33[5] = cm[7];
                cm33[6] = cm[13];
                cm33[7] = cm[14];
                cm33[8] = cm[15];

                det += cm[8] * vmat<F>::determinant_low_order (cm33, 3u);

                // Final 3x3
                cm33[0] = cm[1];
                cm33[1] = cm[2];
                cm33[2] = cm[3];
                cm33[3] = cm[5];
                cm33[4] = cm[6];
                cm33[5] = cm[7];
                cm33[6] = cm[9];
                cm33[7] = cm[10];
                cm33[8] = cm[11];

                det -= cm[12] * vmat<F>::determinant_low_order (cm33, 3u);

            } else {
                throw std::runtime_error ("vmat::determinant_low_order implemented only for 2x2, 3x3, 4x4");
            }

            return det;
        }

        static F determinant_high_order (const sm::vvec<F>& cm, const std::uint32_t _Nr)
        {
            F det = F{0};
            throw std::runtime_error (std::format("vmat::determinant_high_order: write me for matrix {} with _Nr={}", cm.str(), _Nr));
            return det;
        }

        static F determinant (const sm::vvec<F>& cm, const std::uint32_t _Nr)
        {
            if (cm.size() != _Nr * _Nr) {
                throw std::runtime_error ("vmat::determinant_low_order: cm.size() != Nr * Nr");
            }
            if (_Nr < 5) {
                return vmat<F>::determinant_low_order (cm, _Nr);
            } else {
                return vmat<F>::determinant_high_order (cm, _Nr);
            }
        }

        //! Compute determinant for this->arr
        F determinant() const
        {
            if (Nr != Nc) {
                throw std::runtime_error ("vmat::determinant: valid only for square matrices");
            }
            return vmat<F>::determinant (this->arr, Nr);
        }

        /*!
         * The adjugate is the transpose of the cofactor matrix. Recipe:
         * 1. Get the cofactor matrix (with this->cofactor())
         * 2. Obtain the adjugate matrix by transposing the cofactor matrix
         */
        sm::vvec<F> adjugate() const noexcept
        {
            sm::vvec<F> adj (Nr * Nc, F{0});
            if (Nr == Nc && Nr == 2) {
                // Simple recipe for 2x2:
                adj = { this->arr[3], -this->arr[1], -this->arr[2], this->arr[0] };
            } else {
                adj = this->transpose (this->cofactor(), Nr);
            }
            return adj;
        }

        /*!
         * Compute the cofactor matrix of this->arr. Recipe:
         * 1. Create matrix of minors
         * 2. Multiply matrix of minors by a checkerboard pattern to give the cofactor matrix
         */
        sm::vvec<F> cofactor() const
        {
            if (Nr != Nc) { throw std::runtime_error ("vmat::cofactor: valid only for square matrices"); }

            sm::vvec<F> cofac (Nr * Nc, F{0});

            if (Nr == 3) {

                // Keep to column-major format for all matrices. The elements of the matrix
                // of minors is found, but the cofactor matrix is populated, applying the
                // alternating pattern of +/- as we go.

                // 0.
                sm::vvec<F> minor_elem(4);
                minor_elem[0] = this->arr[4];
                minor_elem[1] = this->arr[5];
                minor_elem[2] = this->arr[7];
                minor_elem[3] = this->arr[8];
                cofac[0] = vmat<F>::determinant (minor_elem, 2u);

                // 3. Next minor elem matrix has only 2 elements changed
                minor_elem[0] = this->arr[1];
                minor_elem[1] = this->arr[2];
                cofac[3] = -vmat<F>::determinant (minor_elem, 2u);

                // 6.
                minor_elem[2] = this->arr[4];
                minor_elem[3] = this->arr[5];
                cofac[6] = vmat<F>::determinant (minor_elem, 2u);

                // 1.
                minor_elem[0] = this->arr[3];
                minor_elem[1] = this->arr[5];
                minor_elem[2] = this->arr[6];
                minor_elem[3] = this->arr[8];
                cofac[1] = -vmat<F>::determinant (minor_elem, 2u);

                // 4.
                minor_elem[0] = this->arr[0];
                minor_elem[1] = this->arr[2];
                cofac[4] = vmat<F>::determinant (minor_elem, 2u);

                // 7.
                minor_elem[2] = this->arr[3];
                minor_elem[3] = this->arr[5];
                cofac[7] = -vmat<F>::determinant (minor_elem, 2u);

                // 2.
                minor_elem[0] = this->arr[3];
                minor_elem[1] = this->arr[4];
                minor_elem[2] = this->arr[6];
                minor_elem[3] = this->arr[7];
                cofac[2] = vmat<F>::determinant (minor_elem, 2u);

                // 5.
                minor_elem[0] = this->arr[0];
                minor_elem[1] = this->arr[1];
                cofac[5] = -vmat<F>::determinant (minor_elem, 2u);

                // 8.
                minor_elem[2] = this->arr[3];
                minor_elem[3] = this->arr[4];
                cofac[8] = vmat<F>::determinant (minor_elem, 2u);

            } else if (Nr == 4) {

                // Keep to column-major format for all matrices. The elements of the matrix
                // of minors is found, but the cofactor matrix is populated, applying the
                // alternating pattern of +/- as we go.

                // 0.
                sm::vvec<F> minor_elem(9);
                minor_elem[0] = this->arr[5];
                minor_elem[3] = this->arr[9];
                minor_elem[6] = this->arr[13];

                minor_elem[1] = this->arr[6];
                minor_elem[4] = this->arr[10];
                minor_elem[7] = this->arr[14];

                minor_elem[2] = this->arr[7];
                minor_elem[5] = this->arr[11];
                minor_elem[8] = this->arr[15];

                cofac[0] = vmat<F>::determinant (minor_elem, 3u);

                // 1. Next minor elem matrix has only 3 elements changed
                minor_elem[0] = this->arr[4];
                minor_elem[3] = this->arr[8];
                minor_elem[6] = this->arr[12];
                cofac[1] = -vmat<F>::determinant (minor_elem, 3u);

                // 2
                minor_elem[1] = this->arr[5];
                minor_elem[4] = this->arr[9];
                minor_elem[7] = this->arr[13];
                cofac[2] = vmat<F>::determinant (minor_elem, 3u);

                // 3
                minor_elem[2] = this->arr[6];
                minor_elem[5] = this->arr[10];
                minor_elem[8] = this->arr[14];
                cofac[3] = -vmat<F>::determinant (minor_elem, 3u);

                // 4.
                minor_elem[0] = this->arr[1];
                minor_elem[3] = this->arr[9];
                minor_elem[6] = this->arr[13];

                minor_elem[1] = this->arr[2];
                minor_elem[4] = this->arr[10];
                minor_elem[7] = this->arr[14];

                minor_elem[2] = this->arr[3];
                minor_elem[5] = this->arr[11];
                minor_elem[8] = this->arr[15];

                cofac[4] = -vmat<F>::determinant (minor_elem, 3u);

                // 5.
                minor_elem[0] = this->arr[0];
                minor_elem[3] = this->arr[8];
                minor_elem[6] = this->arr[12];
                cofac[5] = vmat<F>::determinant (minor_elem, 3u);

                // 6.
                minor_elem[1] = this->arr[1];
                minor_elem[4] = this->arr[9];
                minor_elem[7] = this->arr[13];
                cofac[6] = -vmat<F>::determinant (minor_elem, 3u);

                // 7.
                minor_elem[2] = this->arr[2];
                minor_elem[5] = this->arr[10];
                minor_elem[8] = this->arr[14];
                cofac[7] = vmat<F>::determinant (minor_elem, 3u);

                // 8.
                minor_elem[0] = this->arr[1];
                minor_elem[3] = this->arr[5];
                minor_elem[6] = this->arr[13];

                minor_elem[1] = this->arr[2];
                minor_elem[4] = this->arr[6];
                minor_elem[7] = this->arr[14];

                minor_elem[2] = this->arr[3];
                minor_elem[5] = this->arr[7];
                minor_elem[8] = this->arr[15];

                cofac[8] = vmat<F>::determinant (minor_elem, 3u);

                // 9.
                minor_elem[0] = this->arr[0];
                minor_elem[3] = this->arr[4];
                minor_elem[6] = this->arr[12];
                cofac[9] = -vmat<F>::determinant (minor_elem, 3u);

                // 10.
                minor_elem[1] = this->arr[1];
                minor_elem[4] = this->arr[5];
                minor_elem[7] = this->arr[13];
                cofac[10] = vmat<F>::determinant (minor_elem, 3u);

                // 11.
                minor_elem[2] = this->arr[2];
                minor_elem[5] = this->arr[6];
                minor_elem[8] = this->arr[14];
                cofac[11] = -vmat<F>::determinant (minor_elem, 3u);

                // 12.
                minor_elem[0] = this->arr[1];
                minor_elem[3] = this->arr[5];
                minor_elem[6] = this->arr[9];

                minor_elem[1] = this->arr[2];
                minor_elem[4] = this->arr[6];
                minor_elem[7] = this->arr[10];

                minor_elem[2] = this->arr[3];
                minor_elem[5] = this->arr[7];
                minor_elem[8] = this->arr[11];

                cofac[12] = -vmat<F>::determinant (minor_elem, 3u);

                // 13.
                minor_elem[0] = this->arr[0];
                minor_elem[3] = this->arr[4];
                minor_elem[6] = this->arr[8];
                cofac[13] = vmat<F>::determinant (minor_elem, 3u);

                // 14.
                minor_elem[1] = this->arr[1];
                minor_elem[4] = this->arr[5];
                minor_elem[7] = this->arr[9];
                cofac[14] = -vmat<F>::determinant (minor_elem, 3u);

                // 15.
                minor_elem[2] = this->arr[2];
                minor_elem[5] = this->arr[6];
                minor_elem[8] = this->arr[10];
                cofac[15] = vmat<F>::determinant (minor_elem, 3u);

            } else {
                throw std::runtime_error ("vmat::cofactor is currently implemented only for 3x3 and 4x4 matrices");
                // note: this limits vmat::inverse to 3x3 and 4x4 for now :(
            }

            return cofac;
        }

        /*!
         * Compute the complex eigenvalues of this square Nr x Nr matrix of real numbers.
         *
         * Returns a vector of Nr complex eigenvalues.
         *
         * Uses the Faddeev-LeVerrier algorithm to compute the characteristic polynomial det(A -
         * lambda*I) = 0, then solves for roots using polysolve.
         *
         * The eigenvalues will be returned with lexicographic ordering, which is ensured in sm::polysolve::sort_roots:
         *
         * Sort complex roots in standard order:
         * Lexicographic ordering by (real, imag) components.
         * This places real roots first in ascending order, followed by complex roots.
         * Complex conjugate pairs naturally appear adjacent.
         */
        template<typename Fy=F> requires std::is_floating_point_v<Fy>
        sm::vvec<std::complex<F>> eigenvalues() const
        {
            if (!((Nr == Nc) && (Nr >= 2u))) {
                throw std::runtime_error ("eigenvalues method is valid only for square matrices");
            }

            // Use Faddeev-LeVerrier algorithm to get characteristic polynomial
            std::vector<F> coeffs(Nr + 1, F{0});
            coeffs[Nr] = F{1};  // Leading coefficient

            sm::vmat<F> M(Nr, Nr);
            M.set_identity();

            for (std::uint32_t k = 1u; k <= Nr; ++k) {
                M = (*this) * M;  // M_k = A * M_{k-1}, where M_0 = I
                coeffs[Nr - k] = -M.trace() / static_cast<F>(k);

                if (k < Nr) {
                    // M = M_k + c_{n-k} * I
                    for (std::uint32_t d = 0u; d < (Nr * Nr); d += (Nr + 1)) { // accesses diagonal elements
                        M[d] += coeffs[Nr - k];
                    }
                }
            }

            std::vector<std::complex<F>> roots_vector = sm::polysolve::solve<F> (coeffs);
            return *(static_cast<sm::vvec<std::complex<F>>*>(&roots_vector));
        }

        // We could also have a specialization to compute Eigenvalues of a complex matrix:
        template<typename Fy=F> requires sm::is_complex<Fy>::value
        sm::vvec<F> eigenvalues() const noexcept
        {
            static_assert (false, "mat<>::eigenvalues for a complex matrix is not yet implemented");
        }

        // in row_echelon_form_inplace we need the underlying type of std::complex<T>
        template<typename T> struct value_type_of { using type = T; };
        template<typename T> struct value_type_of<std::complex<T>> { using type = typename std::complex<T>::value_type; };

        // Convert *this to (unreduced) row echelon form using Gaussian elimination, updating *this
        void row_echelon_form_inplace()
        {
            if (Nr > Nc) { throw std::runtime_error ("vmat::row_echelon_form_inplace requires (Nc >= Nr)"); }

            std::uint32_t r = 0u; // Initialization of the pivot row
            std::uint32_t c = 0u; // Initialization of the pivot column

            std::uint32_t i_piv = 0u;
            std::uint32_t r_piv = 0u;
            sm::vvec<F> t_row(Nc, F{0}); // temp row

            using F_el = typename value_type_of<F>::type;
            sm::vvec<F_el> t_col(Nr, F_el{0}); // temp col for norms of values. This will be real whether F is real or std::complex

            // Converted from the Pseudocode on https://en.wikipedia.org/wiki/Gaussian_elimination
            while (r < Nr && c < Nc) {
                // Find the pivot for col c - the row in column c that has the greated abs. value
                for (std::uint32_t i = r; i < Nr; ++i) { t_col[i - r] = std::norm ((*this)(i, c)); }
                i_piv = t_col.argmax (Nr - r); // find index of max element in first (Nr - r) elements of t_col
                r_piv = i_piv + r;

                if (t_col[i_piv] == F_el{0}) {
                    // No pivot in this column, pass to next column
                    c++;
                } else {
                    if (i_piv != 0u) { // and thus r_piv != r
                        // swap rows (r, r_piv);
                        t_row = this->row (r_piv);
                        this->set_row (r_piv, this->row (r));
                        this->set_row (r, t_row);
                    } // else no need to swap rows

                    // Do for all rows below pivot:
                    for (std::uint32_t i = r + 1u; i < Nr; i++) {
                        F f = (*this)(i, c) / (*this)(r, c);
                        // Fill the lower part of pivot column with zeros:
                        (*this)(i, c) = F{0};
                        // Do for all remaining elements in current row:
                        for (std::uint32_t j = c + 1u; j < Nc; j++) { (*this)(i, j) -= (*this)(r, j) * f; }
                    }
                    // Increase pivot row and column
                    r++;
                    c++;
                }
            }
        }

        // Convert a copy of this into (unreduced) row echelon form, using Gaussian elimination. Return the result.
        sm::vmat<F> row_echelon_form() const
        {
            sm::vmat<F> ref = *this;
            ref.row_echelon_form_inplace();
            return ref;
        }

        // Divide each row, r,  of *this by the diagonal element (r, r)
        void divide_rows_by_diagonals_inplace()
        {
            if (Nr > Nc) { throw std::runtime_error ("vmat::divide_rows_by_diagonals_inplace requires (Nc >= Nr)"); }
            using F_el = typename value_type_of<F>::type;
            for (std::uint32_t r = 0; r < Nr; ++r) {
                F f = (*this)(r, r);
                f = std::abs(f) > F_el{0} ? f : F{1};
                for (std::uint32_t c = 0; c < Nc; ++c) { (*this)(r, c) /= f; }
            }
        }

        // Convert *this into reduced row echelon form
        void reduced_row_echelon_form_inplace()
        {
            if (Nr > Nc) { throw std::runtime_error ("vmat::row_echelon_form_inplace requires (Nc >= Nr)"); }
            this->row_echelon_form_inplace();
            this->divide_rows_by_diagonals_inplace();
        }

        // Convert a copy of *this into reduced row echelon form and return the result
        sm::vmat<F> reduced_row_echelon_form() const
        {
            sm::vmat<F> rref = *this;
            rref.row_echelon_form_inplace();
            rref.divide_rows_by_diagonals_inplace();
            return rref;
        }

        /*!
         * Using back-substitution, solve the equation Ax = B, where A and B are represented in
         * *this in the usual way, with A being a square matrix on the left and B the right-most
         * column. *this should thus be the 'augmented matrix'. This matrix should have been reduced
         * to row_echelon_form before calling back_substitution().
         *
         * \return A vvec of the solutions, x. NaNs in result mean there was no solution.
         */
        sm::vvec<F> back_substitution() const
        {
            if (Nc != Nr + 1) { throw std::runtime_error ("vmat::back_substitution: requires (Nc == Nr + 1)"); }

            using F_el = typename value_type_of<F>::type;
            constexpr F_el eps = F_el{1e-14}; // needs to be F_el if F is complex

            sm::vvec<F> x (Nr, F{0});

            for (std::uint32_t i = (Nr - 1u); i != std::numeric_limits<std::uint32_t>::max(); i--) {
                const std::uint32_t de = (i * Nr) + i;        // diagonal element A(r, r)
                const std::uint32_t le = ((Nc - 1) * Nr) + i; // last element A(r, Nc - 1)
                if (std::abs (this->arr[de]) > eps) { // std::abs not sm::cem::abs as F may be complex
                    const std::uint32_t n = Nr - i - 1;
                    x[i] = this->arr[le];
                    for (std::uint32_t j = 0; j < n; ++j) {
                        x[i] -= this->arr[(Nr - 1 - j) * Nr + i] * x[i + n - j]; // sum element is A(i, i + n - j)
                    }
                    x[i] /= this->arr[de];
                } else {
                    // arr[de] == 0. If the value in the right-most column is 0 AND the value in the
                    // diagonal is 0, then we have a free value and x[i] remains 0. If the value in
                    // the right most column is != 0, but arr[de] == 0, we have no solution.
                    if constexpr (sm::is_complex<F>::value) {
                        if (this->arr[le] != F{0}) { x[i] = F{std::numeric_limits<F_el>::quiet_NaN()}; }
                    } else {
                        if (this->arr[le] != F{0}) { x[i] = std::numeric_limits<F>::quiet_NaN(); } // else x[i] remains 0
                    }
                }
            }

            return x;
        }

        /*!
         * Find the eigenvectors for the set of eigenvalues found for this matrix of real values.
         * Returns a normalized eigenvector as a complex vector.
         */
        template<typename Fy=F> requires std::is_floating_point_v<Fy>
        sm::vvec<std::complex<F>> eigenvector (const std::complex<F>& lambda) const
        {
            if (Nr != Nc || Nr < 2u) { throw std::runtime_error ("eigenvector method is valid only for square matrices"); }

            // Let A = (*this)
            // Find non-zero X that satisfies A X = lamba X;
            // I.e. (A - lambda * I) X = 0

            // Create augmented matrix. Here's (A...
            sm::vmat<std::complex<F>> aug (Nr, Nc + 1);

            for (std::uint32_t c = 0; c < Nc; ++c) {
                for (std::uint32_t r = 0; r < Nr; ++r) {
                    aug(r, c) = (*this)(r, c);
                }
            }
            // ... and here's: - lambda I)
            for (std::uint32_t d = 0u; d < (Nr * Nr); d += (Nr + 1)) { // accesses diagonal elements
                aug[d] -= lambda;
            }
            // Last col of augmented matrix contains zeros
            for (std::uint32_t r = 0; r < Nr; ++r) {
                aug(r, Nc) = std::complex<F>{ F{0}, F{0} };
            }

            // Change the augmented matrix into row echelon form
            aug.row_echelon_form_inplace();

            // The row in the matrix for the lambda we've been passed will be all zeros and so that v is
            // a 'free value'. It will be the bottom row, so set these two elements to the value 1:
            aug(Nr - 1, Nc - 1) = F{1};
            aug(Nr - 1, Nc) = F{1};

            sm::vvec<std::complex<F>> v = aug.back_substitution();
            if (v.size() != Nr) { throw std::runtime_error ("Unexpected result size form aug.back_substitution()"); }

            // Normalize v
            using F_el = typename value_type_of<F>::type;
            constexpr F_el my_epsilon = F_el{1e-14}; // needs to be F_el if F is complex
            F_el normsq = F_el{0};
            for (std::uint32_t i = 0; i < Nr; ++i) { normsq += std::norm (v[i]); }
            F_el norm = std::sqrt (normsq);
            if (norm > my_epsilon) { v /= norm; }

            return v;
        }

        // Get eigenvector of a complex matrix, given a complex eigenvalue (placeholder)
        template<typename Fy=F> requires sm::is_complex<Fy>::value
        sm::vvec<F> eigenvector ([[maybe_unused]] const F& lambda) const
        {
            static_assert(false, "vmat<>::eigenvector for a complex matrix is not yet implemented");
        }

        /*!
         * Compute both eigenvalues and their corresponding eigenvectors.
         * Returns a vector of 4 pairs, each containing an eigenvalue and its eigenvector.
         */
        struct eigenpair
        {
            std::complex<F> eigenvalue = {};
            sm::vvec<std::complex<F>> eigenvector = {};
        };

        sm::vvec<eigenpair> eigenpairs() const
        {
            if (Nr != Nc || Nr < 2u) { throw std::runtime_error ("vmat::trace: valid only for square matrices"); }
            sm::vvec<std::complex<F>> lambdas = this->eigenvalues();
            sm::vvec<eigenpair> pairs (lambdas.size());
            for (std::uint32_t i = 0; i < Nr; ++i) {
                pairs[i].eigenvalue = lambdas[i];
                pairs[i].eigenvector = this->eigenvector (lambdas[i]);
            }
            return pairs;
        }

        //! Compute the trace of the matrix (the sum of the diagonal elements)
        F trace() const
        {
            if (Nr != Nc) { throw std::runtime_error ("vmat::trace: valid only for square matrices"); }

            F tr = F{0};
            for (std::uint32_t i = 0; i < Nr * Nc; i += Nr + 1) {
                tr += this->arr[i];
            }
            return tr;
        }

        // Return vmat with the order of the rows reversed
        vmat<F> flipud() const noexcept
        {
            vmat<F> m (Nr, Nc);
            for (std::uint32_t i = 0; i < Nr; ++i) { m.set_row (Nr - 1 - i, this->row (i)); }
            return m;
        }

        void flipud_inplace() noexcept
        {
            vmat<F> m = this->flipud();
            (*this) = m;
        }

        // Return vmat with the order of the cols reversed
        vmat<F> fliplr() const noexcept
        {
            vmat<F> m (Nr, Nc);
            for (std::uint32_t i = 0; i < Nc; ++i) { m.set_col (Nc - 1 - i, this->col (i)); }
            return m;
        }

        void fliplr_inplace() noexcept
        {
            vmat<F> m = this->fliplr();
            (*this) = m;
        }

        /*!
         * Return a string representation of the matrix
         *
         * \param prec The number of significant figures to show in each element
         * \tparam approximate_zero If true, then for values < 20*epsilon, show "~0" isntead of something like "-1.99349e-07"
         */
        template<bool approximate_zero = true>
        std::string str (const std::uint32_t prec = std::numeric_limits<F>::max_digits10) const noexcept
        {
            // Extra formatting space. +6 allows for the characters '-.e-NN' in '-x.xxxxxe-NN' and ensure that matrices print out neatly
            constexpr std::uint32_t extra_space = 6u;

            std::string s;
            for (std::uint32_t r = 0; r < this->Nr; ++r) {
                s += "| ";
                for (std::uint32_t c = 0; c < this->Nc; ++c) {
                    if constexpr (sm::is_complex<F>::value) {
                        std::string cplx = std::format (" ({:^.{}}, {:^.{}}) ",
                                                        std::real (this->arr[r + (c * this->Nr)]), prec,
                                                        std::imag (this->arr[r + (c * this->Nr)]), prec);
                        s += std::format ("{:^{}}", cplx, prec + extra_space);
                    } else {
                        if constexpr (approximate_zero == true) {
                            const F el = this->arr[r + (c * this->Nr)];
                            if (el != F{0} && sm::cem::abs (el) < 20 * std::numeric_limits<F>::epsilon()) {
                                s += std::format ("{:^{}}", "~0 ", prec + extra_space);
                            } else {
                                s += std::format ("{:^{}.{}}", el, prec + extra_space, prec);
                            }
                        } else {
                            s += std::format ("{:^{}.{}}", this->arr[r + (c * this->Nr)], prec + extra_space, prec);
                        }
                    }
                }
                s += " |\n";
            }
            s += "\n";
            return s;
        }

        //! Return a string representation of the matrix. Note choice of digits10 for the type float, regardless of the type F
        std::string str (const std::uint32_t prec = std::numeric_limits<float>::digits10) const noexcept { return this->str<true> (prec); }

        // An array containing the matrix values, arranged (like sm::mat) in column major format
        sm::vvec<F> arr;

    private:
        // Number of rows in matrix. Named the same as the corresponding template parameter in sm::mat
        std::uint32_t Nr = 0u;
        // Number of cols in this matrix. Named the same as the corresponding template parameter in sm::mat
        std::uint32_t Nc = 0u;
    };

    template <typename F>
    std::ostream& operator<< (std::ostream& os, const vmat<F>& tm)
    {
        os << tm.str();
        return os;
    }

} // namespace sm
