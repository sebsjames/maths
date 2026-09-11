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

        // Access
        F& operator() (std::uint32_t r, std::uint32_t c) { return this->arr[r * this->Nc + c]; }
        const F& operator() (std::uint32_t r, std::uint32_t c) const { return this->arr[r * this->Nc + c]; }

        //! Access elements of the matrix (returns ref, so not const)
        F& operator[] (const std::uint32_t idx) { return this->arr[idx]; }

        //! Access elements of the matrix with const promise
        F operator[] (const std::uint32_t idx) const noexcept
        {
            constexpr F badrtn = std::numeric_limits<F>::has_quiet_NaN ? std::numeric_limits<F>::quiet_NaN() : std::numeric_limits<F>::max();
            return idx < this->arr.size() ? this->arr[idx] : badrtn;
        }

        std::uint32_t size() const { return this->Nr * this->Nc; }
        sm::vec<std::uint32_t, 2> shape() const { return sm::vec<std::uint32_t, 2>{ this->Nr, this->Nc }; }
        std::uint32_t rows() const { return this->Nr; }
        std::uint32_t cols() const { return this->Nc; }

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
        std::string str (const std::uint32_t prec = std::numeric_limits<float>::digits10) const noexcept { return this->str (prec); }

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
