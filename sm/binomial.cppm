// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 * \brief sm::binomial - Functions to compute binomial coefficients and Pascal's triangle as a table.
 * \author Seb James
 * \date 2026 (based on older code from sm::bezcurve and before that morphologica)
 */
module;

#include <cstdint>
#include <array>
#include <limits>
#include <type_traits>

export module sm.binomial;

export namespace sm::binomial
{
    /*!
     * Compute the binomial coefficient (n k) = n! / k! (n - k)!
     */
    constexpr std::uint64_t nk (const std::uint64_t n, std::uint64_t k)
    {
        std::uint64_t res = 1u;
        k = (k > n - k) ? n - k : k;
        for (std::uint64_t i = 0u; i < k; ++i) {
            res *= (n - i);
            res /= (i + 1u);
        }
        return res;
    }

    //! Return the number of elements in Pascal's triangle with N rows
    template <std::uint64_t N>
    constexpr std::uint64_t pascal_size() { return (N * (N + 1u) / 2u); }

    /*!
     * Return the first N rows of Pascal's triangle in a linear array.
     *
     * \tparam N number of rows in Pascal's triangle
     */
    template <std::uint64_t N>
    constexpr std::array<std::uint64_t, (N * (N + 1u) / 2u)> make_pascals_triangle()
    {
        std::array<std::uint64_t, (N * (N + 1u) / 2u)> pt;
        std::uint64_t idx = 0u;
        for (std::uint64_t row = 0u; row < N; row++) {
            for (std::uint64_t i = 0u; i <= row; i++) {
                pt[idx++] = binomial::nk (row, i);
            }
        }
        return pt;
    }

    /*!
     * Look up the binomial coefficient (n,k) from the passed in Pascal's Triangle array.
     *
     * Create the Pascal's Triangle array using sm::make_pascals_triangle.
     *
     * To get the values from row n, where n starts at 0 (and ends at N-1), you step
     * along a number given by the triangle sequence (n(n+1)/2) and then read n+1
     * values. OR to get n,k, step along a number given by the triangle sequence
     * (n(n+1)/2) and then step another k spaces to the result.
     *
     * \tparam T The type in which to return the looked-up element of the triangle
     *
     * \tparam N number of rows in Pascal's triangle
     *
     * \return result (with static_cast to type T if requested) or T::max if the index is out of
     * range of the pascals_triangle array.
     */
    template <std::uint64_t N, typename T = std::uint64_t>
    constexpr T lookup (const std::uint64_t n, const std::uint64_t k,
                        const std::array<std::uint64_t, (N * (N + 1u) / 2u)>& pascals_triangle)
    {
        std::uint64_t idx = (n * (n + 1u) / 2u) + k;
        if (idx >= (N * (N + 1u) / 2u)) { return std::numeric_limits<T>::max(); }
        if constexpr (std::is_same_v<T, std::uint64_t>) {
            return pascals_triangle[idx];
        } else {
            return static_cast<T>(pascals_triangle[idx]);
        }
    }
}
