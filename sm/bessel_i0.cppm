// -*- C++ -*-

/*
 * Compute cylindrical Bessel function of order 0. This code is a small part of the boost
 * implementation of Cylindrical Bessel functions.
 *
 * Seb copied this from Boost to use in sm::random, and so the copyright is:
 *
 * Copyright (c) 2006 Xiaogang Zhang
 * Copyright (c) 2017 John Maddock
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt)
 */
module;

#include <type_traits>
#include <vector>
#include <cmath>

export module sm.bessel_i0;

import sm.polysolve;

export namespace sm
{
    // I only require the 24 digit implementation from boost
    template <typename T> requires std::is_floating_point_v<T>
    T bessel_i0 (const T& x)
    {
        if (x < T{7.75}) {
            // Max error in interpolated form: 3.929e-08
            // Max Error found at float precision = Poly: 1.991226e-07
            std::vector<T> P = { // which order does boost use?
                T{1.00000003928615375e+00},
                T{2.49999576572179639e-01},
                T{2.77785268558399407e-02},
                T{1.73560257755821695e-03},
                T{6.96166518788906424e-05},
                T{1.89645733877137904e-06},
                T{4.29455004657565361e-08},
                T{3.90565476357034480e-10},
                T{1.48095934745267240e-11}
            };
            T a = x * x / T{4};

            return a * sm::polysolve::evaluate<T> (P, a) + T{1};

        } else if (x < T{50}) {
            // Max error in interpolated form: 5.195e-08
            // Max Error found at float precision = Poly: 8.502534e-08
            std::vector<T> P = {
                T{3.98942651588301770e-01},
                T{4.98327234176892844e-02},
                T{2.91866904423115499e-02},
                T{1.35614940793742178e-02},
                T{1.31409251787866793e-01}
            };
            return std::exp(x) * sm::polysolve::evaluate<T> (P, T{1} / x) / std::sqrt(x);

        } else {
            // Max error in interpolated form: 1.782e-09
            // Max Error found at float precision = Poly: 6.473568e-08
            std::vector<T> P = {
                T{3.98942391532752700e-01},
                T{4.98455950638200020e-02},
                T{2.94835666900682535e-02}
            };
            T ex = std::exp (x / T{2});
            T result = ex * sm::polysolve::evaluate<T> (P, T{1} / x) / std::sqrt(x);
            result *= ex;
            return result;
        }
    }
}
