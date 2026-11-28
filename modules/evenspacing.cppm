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

#include <sm/evenspacing.hpp>

export module sm.evenspacing;

export namespace sm::evenspacing
{
    using sm::evenspacing::estimate_length;
    using sm::evenspacing::find_coordinates;
}
