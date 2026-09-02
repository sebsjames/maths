#include <cstdint>
#include <cmath>
#include <complex>
#include <iostream>

import sm.hexfft;
import sm.hexgrid;
import sm.vvec;

template<typename F>
static sm::vvec<F> make_data (std::uint32_t size)
{
    sm::vvec<F> data (size);
    for (std::uint32_t i = 0; i < size; ++i) {
        data[i] = std::sin (static_cast<F>(i) * F{0.37}) + F{0.5} * std::cos (static_cast<F>(i) * F{1.13});
    }
    return data;
}

template<typename F>
static bool roundtrips (sm::hexgrid<F>& hg, const char* label)
{
    sm::vvec<F> data = make_data<F> (hg.num());

    sm::hexfft::spectrum<F> X = sm::hexfft::fft (hg, data);
    sm::vvec<std::complex<F>> back = sm::hexfft::ifft (hg, X);

    F maxerr = F{0};
    for (std::uint32_t i = 0; i < hg.num(); ++i) {
        F err = std::abs (back[i] - std::complex<F> (data[i], F{0}));
        maxerr = std::max (maxerr, err);
    }
    std::cout << label << ": hg.num()=" << hg.num() << " spectrum n=" << X.n << " m=" << X.m
               << " (size " << X.size() << ") max roundtrip error: " << maxerr << std::endl;
    return maxerr < F{1e-8};
}

template<typename F>
static bool is_linear (sm::hexgrid<F>& hg, const char* label)
{
    sm::vvec<std::complex<F>> x1 (hg.num());
    sm::vvec<std::complex<F>> x2 (hg.num());
    for (std::uint32_t i = 0; i < hg.num(); ++i) {
        x1[i] = std::complex<F> (std::sin (static_cast<F>(i) * F{0.21}), std::cos (static_cast<F>(i) * F{0.05}));
        x2[i] = std::complex<F> (std::cos (static_cast<F>(i) * F{0.44}), std::sin (static_cast<F>(i) * F{0.63}));
    }
    std::complex<F> a (F{1.7}, F{-0.3});
    std::complex<F> b (F{-0.9}, F{0.4});

    sm::vvec<std::complex<F>> combo (hg.num());
    for (std::uint32_t i = 0; i < hg.num(); ++i) { combo[i] = a * x1[i] + b * x2[i]; }

    sm::hexfft::spectrum<F> X1 = sm::hexfft::fft (hg, x1);
    sm::hexfft::spectrum<F> X2 = sm::hexfft::fft (hg, x2);
    sm::hexfft::spectrum<F> Xcombo = sm::hexfft::fft (hg, combo);

    F maxerr = F{0};
    for (std::uint32_t i = 0; i < Xcombo.size(); ++i) {
        std::complex<F> expected = a * X1.data[i] + b * X2.data[i];
        maxerr = std::max (maxerr, std::abs (Xcombo.data[i] - expected));
    }
    std::cout << label << ": linearity max error: " << maxerr << std::endl;
    return maxerr < F{1e-8};
}

// A perfectly rectangular (in ri,gi index space) boundary should give a spectrum whose
// bounding rectangle exactly matches the parallelogram's own extent (bar any rounding of the
// row count up to an even number).
static bool bounding_box_matches_parallelogram()
{
    sm::hexgrid<double> hg (1.0, 40.0);
    hg.set_parallelogram_boundary (2, 3); // ri in [-2,2] (m=5), gi in [-3,3] (rows=7)

    sm::vvec<double> data (hg.num(), 1.0);
    sm::hexfft::spectrum<double> X = sm::hexfft::fft (hg, data);

    bool ok = (X.m == 5) && (X.n == 4) && (X.ri_min == -2) && (X.gi_min == -3);
    // rows=7 is odd, so it rounds up to 2*n=8; every hex is present in the padded rectangle
    // except for the one extra zero-padded row, so hg.num() should be exactly one row short.
    ok = ok && (hg.num() == 35) && (X.size() == 40);
    std::cout << "bounding_box_matches_parallelogram: n=" << X.n << " m=" << X.m
               << " ri_min=" << X.ri_min << " gi_min=" << X.gi_min
               << " hg.num()=" << hg.num() << " X.size()=" << X.size()
               << (ok ? " OK" : " FAIL") << std::endl;
    return ok;
}

std::int32_t main()
{
    std::int32_t rtn = 0;

    if (!bounding_box_matches_parallelogram()) { --rtn; }

    {
        sm::hexgrid<double> hg (1.0, 40.0);
        hg.set_parallelogram_boundary (2, 3);
        if (!roundtrips<double> (hg, "parallelogram")) { --rtn; }
        if (!is_linear<double> (hg, "parallelogram")) { --rtn; }
    }

    // An arbitrary (non-rectangular-in-ri,gi) boundary: a circle. This exercises the
    // zero-padding path: hg.num() will be well short of the padded rectangle's size.
    {
        sm::hexgrid<double> hg (1.0, 30.0);
        hg.set_circular_boundary (6.0);
        if (!roundtrips<double> (hg, "circular")) { --rtn; }
        if (!is_linear<double> (hg, "circular")) { --rtn; }
    }

    // A second, differently-sized circular boundary, to exercise a different mix of
    // power-of-two/non-power-of-two internal FFT sizes.
    {
        sm::hexgrid<double> hg (1.0, 24.0);
        hg.set_circular_boundary (4.5);
        if (!roundtrips<double> (hg, "circular2")) { --rtn; }
    }

    if (rtn != 0) {
        std::cout << "FAIL" << std::endl;
    } else {
        std::cout << "SUCCESS" << std::endl;
    }
    return rtn;
}
