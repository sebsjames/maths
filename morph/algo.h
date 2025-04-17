#pragma once

/*
 * Miscellaneous algorithms
 */

#include <type_traits>
#include <cmath>
#include <cstdint>
#include <limits>
#include <bitset>
#include <morph/mathconst.h>
#include <morph/constexpr_math.h> // constexpr math functions from Keith O'Hara
#include <morph/range.h>
#include <morph/vec.h>
#include <iostream> // debug

namespace morph
{
    enum class rotation_sense : uint32_t
    {
        colinear,
        clockwise,
        anticlockwise
    };
}

namespace morph::algo
{
    // Find the significant base10 columns in a floating point number of type F Return a
    // range whose max is the order of magnitude of the largest base10 column and whose
    // min is the order of magnitude of the smallest significant base10 column.
    template <typename F> requires std::is_floating_point_v<F>
    constexpr morph::range<int> significant_cols (const F& f)
    {
        morph::range<int> sigcols = { 0, 0 };

        // If NaN or infinity, return 0, 0. Or something else?
        if (morph::math::isnan(f) || morph::math::isinf(f)) { return sigcols; }

        // How many sig figures does the type (float, double...) support?
        constexpr F epsilon = std::numeric_limits<F>::epsilon();
        constexpr int type_sf = static_cast<int>(morph::math::floor (morph::math::log10 (F{1} / epsilon)));

        // copy absolute value of f
        F fcpy = f > F{0} ? f : -f;

        // Find the biggest column
        sigcols.max = static_cast<int>(morph::math::floor (morph::math::log10 (fcpy)));

        // Initialize smallest column
        sigcols.min = sigcols.max;

        // We will need to use 10^sigcols.max
        F tentothe = morph::math::pow (F{10}, sigcols.max);

        // Loop down sigcols.min until we hit the limit for type F
        for (; sigcols.min > (sigcols.max - type_sf); --sigcols.min) {

            // What's the value in this column?
            F colval = morph::math::floor(fcpy / tentothe);

            // Is colval very small? Break if so.
            if (std::abs(colval) < epsilon) {
                ++sigcols.min;
                break;
            }

            // Is col val very close to 10? Break if so. This should relate to epsilon and current position
            if (colval > F{8}) {
                // additional cols depends on max precision of type and current col.
                int cols_remaining = type_sf - (sigcols.max - sigcols.min); // How many significant column have we already used?
                if (cols_remaining > 0) {
                    F additional_cols = morph::math::pow (F{10}, cols_remaining);
                    if (morph::math::abs((fcpy - colval * tentothe) - tentothe) < (tentothe / additional_cols)) {
                        ++sigcols.min;
                        break;
                    }
                } else { // no cols left
                    ++sigcols.min;
                    break;
                }
            }

            // Update fcpy by subtracting the column value, then shifting-right with multiply by 10
            fcpy = (fcpy - colval * tentothe) * F{10};
        }

        return sigcols;
    }

    // Return the amount of precision (significant figures in base 10) required for the
    // floating point number f
    template <typename F> requires std::is_floating_point_v<F>
    constexpr int significant_figs (const F& f)
    {
        morph::range<int> sc = morph::algo::significant_cols<F> (f);
        return sc.span();
    }

    static constexpr bool rc_debug = false;

    // Round the number f to the base10 col mincol. In f, our convention is that column
    // 1 is 10s, 0 is 1s, -1 is tenths, -2 hundreds, etc. So round_to_col (1.2345f, -2)
    // should return 1.23f.
    template <typename F> requires std::is_floating_point_v<F>
    constexpr F round_to_col (const F& f, const int mincol)
    {
        if (morph::math::isnan(f) || morph::math::isinf(f)) { return f; }
        // How many sig figures does the type (float, double...) support?
        constexpr F epsilon = std::numeric_limits<F>::epsilon();
        constexpr int type_sf = static_cast<int>(morph::math::floor (morph::math::log10 (F{1} / epsilon)));
        if constexpr (rc_debug) { std::cout << "rtc: round_to_col(" << f << ", " << mincol << ") called\n"; }
        // Save sign and a positive version of the input
        int sign = f > F{0} ? 1 : -1;
        F fcpy = f > F{0} ? f : -f;
        // Find the biggest column
        int maxcol = static_cast<int>(morph::math::floor (morph::math::log10 (fcpy)));
        // We will need to use 10^maxcol
        F tentothe = morph::math::pow (F{10}, maxcol);
        // If user passed a mincol that's too big, return number rounded to maxcol
        if (mincol > maxcol) { return (sign > 0 ? tentothe : -tentothe); }
        F rounded = F{0};
        // Loop down curcol until we hit the limit for type F or mincol
        for (int curcol = maxcol; curcol >= mincol && curcol > (maxcol - type_sf); --curcol) {
            // What's the value in this column?
            F colval = morph::math::floor(fcpy / tentothe);
            if constexpr (rc_debug) { std::cout << "rtc: curcol = " << curcol << ". colval = " << colval << std::endl; }
            // Add to rounded
            rounded += morph::math::pow (F{10}, curcol) * colval;
            if constexpr (rc_debug) { std::cout << "rtc: rounded now = " << rounded << std::endl; }

            if (curcol == mincol) {
                // We're on last col. What's the colval in the next col? Peek into it
                F fcpy_peek = (fcpy - colval * tentothe) * F{10};
                F colval_peek = morph::math::floor (fcpy_peek / tentothe);
                if (colval_peek >= F{5}) { // need to round curcol up
                    rounded += morph::math::pow (F{10}, curcol) * F{1};
                }
            }

            // Update fcpy by subtracting the column value, then shifting-right with multiply by 10
            fcpy = (fcpy - colval * tentothe) * F{10};
        }
        if constexpr (rc_debug) { std::cout << "rtc: return " << (sign > 0 ? rounded : -rounded) << std::endl; }
        return (sign > 0 ? rounded : -rounded);
    }

    // Return n!
    template <typename T, typename I>
    constexpr T factorial (const I n)
    {
        T fac = T{1};
        for (I i = I{1}; i <= n; ++i) { fac *= i; }
        return fac;
    }

    // Compute the normalization function N^l_m. If invalid l<0 is passed, return 0.
    template <typename T, typename UI, typename I>
    requires (std::is_integral_v<UI> && !std::is_signed_v<UI> && std::is_integral_v<I> && std::is_signed_v<I>)
    T Nlm (const UI l, const I m)
    {
        using mc = morph::mathconst<T>;
        I absm = m < I{0} ? -m : m;
        return std::sqrt ( mc::one_over_four_pi * (T{2} * static_cast<T>(l) + T{1}) * (morph::algo::factorial<T, I>(l - absm) / morph::algo::factorial<T, I>(l + absm)));
    }

#ifndef __APPLE__ // There is no std::assoc_legendre on Mac, apparently
    //! Wraps std::assoc_legendre, allowing signed m (abs(m) always passed to std::assoc_legendre)
    template <typename T, typename UI, typename I>
    requires (std::is_integral_v<UI> && !std::is_signed_v<UI> && std::is_integral_v<I> && std::is_signed_v<I>)
    T Plm (const UI l, const I m, const T x)
    {
        unsigned int absm = m >= 0 ? static_cast<unsigned int>(m) : static_cast<unsigned int>(-m);
        return std::assoc_legendre (static_cast<unsigned int>(l), absm, x);
    }

    //! Compute the real spherical harmonic function with pre-computed normalization term Nlm.
    template <typename T, typename UI, typename I>
    requires (std::is_integral_v<UI> && !std::is_signed_v<UI> && std::is_integral_v<I> && std::is_signed_v<I>)
    T real_spherical_harmonic (const UI l, const I m, const T nlm, const T phi, const T theta)
    {
        using mc = morph::mathconst<T>;
        T ylm = T{0};
        if (m > I{0}) {
            ylm = mc::root_2 * nlm * std::cos (m * phi) * morph::algo::Plm<T, UI, I> (l, m, std::cos (theta));
        } else if (m < I{0}) {
            ylm = mc::root_2 * nlm * std::sin (-m * phi) * morph::algo::Plm<T, UI, I> (l, -m, std::cos (theta));
        } else { // m == 0
            ylm = nlm * morph::algo::Plm<T, UI, I> (l, I{0}, std::cos (theta));
        }
        return ylm;
    }

    //! Compute the real spherical harmonic function without a pre-computed normalization term
    template <typename T, typename UI, typename I>
    T real_spherical_harmonic (const UI l, const I m, const T phi, const T theta)
    {
        return morph::algo::real_spherical_harmonic<T, UI, I>(l, m, morph::algo::Nlm<T, UI, I>(l, m), phi, theta);
    }
#endif // __APPLE__

    //! Compute orientation of three points which form a triangle pqr.
    //! \return 0 if co-linear, 1 if clockwise; 2 if anticlockwise
    //! Algorithm, which uses slopes, taken from
    //! https://www.geeksforgeeks.org/orientation-3-ordered-points/
    template<typename T>
    rotation_sense orientation (const morph::vec<T, 2>& p,
                                const morph::vec<T, 2>& q,
                                const morph::vec<T, 2>& r)
    {
        T val = (q[1] - p[1]) * (r[0] - q[0])  -  (q[0] - p[0]) * (r[1] - q[1]);
        if (val == T{0}) { return rotation_sense::colinear; }
        return (val > T{0}) ? rotation_sense::clockwise : rotation_sense::anticlockwise;
    }

    // Given three colinear points p, q, r, the function checks if
    // point q lies on line segment 'pr'. Copied from:
    // https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
    template<typename T>
    static bool onsegment (const morph::vec<T, 2>& p,
                           const morph::vec<T, 2>& q,
                           const morph::vec<T, 2>& r)
    {
        return (q[0] <= std::max(p[0], r[0]) && q[0] >= std::min(p[0], r[0])
                && q[1] <= std::max(p[1], r[1]) && q[1] >= std::min(p[1], r[1]));
    }

    /*!
     * Do the line segments p1-q1 and p2-q2 intersect? Are they instead colinear? Return these
     * booleans in a bitset (bit0: intersection, bit1: colinear)
     *
     * \param p1 Start of line segment 1
     * \param q1 End of line segment 1
     * \param p2 Start of line segment 2
     * \param q2 End of line segment 2
     *
     * \return A bitset whose bit 0 indicates if the lines intersect and whose bit 1 indicates
     * if the lines are colinear
     */
    template<typename T>
    static std::bitset<2> segments_intersect (const morph::vec<T, 2>& p1, const morph::vec<T, 2> q1,
                                              const morph::vec<T, 2>& p2, const morph::vec<T, 2> q2)
    {
        constexpr bool debug_this = false;
        if constexpr (debug_this) {
            std::cout << "Testing intersection for " << p1 << "->" << q1
                      << " and " << p2 << "->" << q2 << std::endl;
        }
        std::bitset<2> rtn;
        morph::rotation_sense p1q1p2 = morph::algo::orientation (p1, q1, p2);
        morph::rotation_sense p1q1q2 = morph::algo::orientation (p1, q1, q2);
        morph::rotation_sense p2q2p1 = morph::algo::orientation (p2, q2, p1);
        morph::rotation_sense p2q2q1 = morph::algo::orientation (p2, q2, q1);
        if (p1q1p2 != p1q1q2 && p2q2p1 != p2q2q1) { // They intersect
            rtn.set(0, true);
        } else { // Are they colinear?
            if constexpr (debug_this) {
                std::cout << "Test colinearity... epsilon is " << std::numeric_limits<T>::epsilon() << "\n";
                if (p1q1p2 == morph::rotation_sense::colinear) {
                    std::cout << "p1q1p2 rotn sense is colinear. On segment? "
                              << (morph::algo::onsegment (p1, p2, q1) ? "T" : "F") << std::endl;
                } else if (p1q1q2 == morph::rotation_sense::colinear) {
                    std::cout << "p1q1q2 rotn sense is colinear On segment? "
                              << (morph::algo::onsegment (p1, q2, q1) ? "T" : "F") << std::endl;
                } else if (p2q2p1 == morph::rotation_sense::colinear) {
                    std::cout << "p2q2p1 rotn sense is colinear On segment? "
                              << (morph::algo::onsegment (p2, p1, q2) ? "T" : "F") << std::endl;
                } else if (p2q2q1 == morph::rotation_sense::colinear) {
                    std::cout << "p2q2q1 rotn sense is colinear On segment? "
                              << (morph::algo::onsegment (p2, q1, q2) ? "T" : "F") << std::endl;
                } else {
                    std::cout << "NO rotn sense is colinear\n";
                }
            }
            if (p1q1p2 == morph::rotation_sense::colinear && morph::algo::onsegment (p1, p2, q1)) { rtn.set(1, true); }
            else if (p1q1q2 == morph::rotation_sense::colinear && morph::algo::onsegment (p1, q2, q1)) { rtn.set(1, true); }
            else if (p2q2p1 == morph::rotation_sense::colinear && morph::algo::onsegment (p2, p1, q2)) { rtn.set(1, true); }
            else if (p2q2q1 == morph::rotation_sense::colinear && morph::algo::onsegment (p2, q1, q2)) { rtn.set(1, true); }
        }
        if constexpr (debug_this) { std::cout << "return " << rtn << std::endl; }
        return rtn;
    }

    /*!
     * Find the coordinate of the crossing point of the two line segments p1-q1 and p2-q2,
     * *assuming* the segments intersect. Call this *after* you have used
     * algo::segments_intersect!
     *
     * \param p1 Start of line segment 1
     * \param q1 End of line segment 1
     * \param p2 Start of line segment 2
     * \param q2 End of line segment 2
     *
     * \return Coordinate of the crossing point
     */
    template<typename T>
    static morph::vec<T, 2> crossing_point (const morph::vec<T, 2>& p1, const morph::vec<T, 2>& q1,
                                            const morph::vec<T, 2>& p2, const morph::vec<T, 2>& q2)
    {
        morph::vec<T, 2> p = p1;
        morph::vec<T, 2> r = p1 - q1;
        morph::vec<T, 2> q = p2;
        morph::vec<T, 2> s = p2 - q2;
        auto t = (q - p).cross(s / r.cross(s));
        morph::vec<T, 2> cross_point = p + t * r;
        return cross_point;
    }

} // morph::math
