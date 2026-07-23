// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * A function to compute n evenly spaced points along a curve defined by the user-supplied function
 * f(x).
 *
 * Also exports a function to numerically (and somewhat roughly) estimate the length along a
 * function f(x) between two locations on the abscissa, x0 and x1.
 *
 * Example usage:
 *
 * // Define some 1D function
 * float f(const float x) { return std::log (x) + 5.3f; }
 *
 * int main()
 * {
 *     // Start and end x values to space coordinates along f(x)
 *     const float x0 = 0.005f;
 *     const float x1 = 1.0f;
 *     // number of coordinates to find
 *     const std::int32_t n = 10;
 *     std::cout << "The function f(x) between " << x0 << " and " << x1 << " has length approx: "
 *               << sm::evenspacing::estimate_length<float> (x0, x1, n * 100, f) << '\n';
 *     sm::vvec<sm::vec<float, 2>> es = sm::evenspacing::find_coordinates<float> (x0, x1, n, f);
 * }
 *
 * \author Seb James
 * \date July 2026
 */
module;

#include <cstdint>
#include <functional>
#include <cmath>

export module sm.evenspacing;

export import sm.vec;
export import sm.vvec;

export namespace sm::evenspacing
{
    // Compute the approximate length of the path from f(x0) to f(x1) by summing n - 1 linear
    // sections, evenly spaced along the abscissa.
    template<typename F>
    F estimate_length (F x0, F x1, std::int32_t n, std::function<F(const F)> f)
    {
        F d = F{0};
        sm::vec<F, 2> last = { x0, f(x0) };
        sm::vec<F, 2> cur = {};
        const F dx = (x1 - x0) / (n - 1);
        for (std::int32_t i = 1; i < n; ++i) {
            const F cx = x0 + dx * i;
            cur = { cx, f(cx) };
            d += (cur - last).length();
            last = cur;
        }
        return d;
    }

    // Return a container of n evenly spaced (by distance along the curve) coordinates of the
    // function f(x), starting at xs, f(xs) and ending at xe, f(xe)
    template<typename F>
    sm::vvec<sm::vec<F, 2>> find_coordinates (F xs, F xe, std::int32_t n, std::function<F(const F)> f)
    {
        sm::vvec<sm::vec<F, 2>> rtn (n, sm::vec<F, 2>{});
        // require n >= 2
        if (n < 2) { return rtn; }
        // Compute the distance between n evenly spaced points on the function f()
        F dd = sm::evenspacing::estimate_length<F> (xs, xe, n * 100, f) / (n - 1);
        // This is the 'last coord for which we output a line'
        sm::vec<F, 2> last = { xs, f(xs) };
        // Add the start point
        rtn[0] = last;
        // Find the intermediate points
        for (std::int32_t i = 1; i < n - 1; ++i) {
            // For each i, find the x that adds dd to the path starting from (last_x, f(last_x))
            sm::vec<F, 2> far = { xe, f(xe) };
            F near_x = last[0]; // Don't need near_y
            std::int32_t searching = 100000; // Don't allow infinite search (this gets decremented in the while loop)
            while (searching) {
                sm::vec<F, 2> mid = { near_x + (far[0] - near_x) / F{2}, F{0} };
                mid[1] = f(mid[0]);
                const F dfar = (far - last).length();
                const F dmid = (mid - last).length();

                if (std::abs(dfar - dd) <= (dd / F{1000})) {
                    last = far;
                    rtn[i] = last;
                    searching = 0;

                } else if (std::abs(dmid - dd) <= (dd / F{1000})) {
                    last = mid;
                    rtn[i] = last;
                    searching = 0;

                } else if (dmid < dd) {
                    // Bisect mid and far; far remains unchanged; near becomes mid
                    near_x = mid[0];
                    --searching;

                } else if (dmid > dd) {
                    // Bisect near and mid: near_x unchanged; far becomes mid
                    far = mid;
                    --searching;
                }
            }
        }
        // Finally, set the end point and return
        rtn[n - 1] = { xe, f(xe) };
        return rtn;
    }
}
