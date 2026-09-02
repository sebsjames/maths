#include <cstdint>
#include <cmath>
#include <complex>
#include <iostream>

import sm.hexfft;
import sm.vvec;

template<typename F>
static bool roundtrips (std::uint32_t n, std::uint32_t m)
{
    sm::hexfft::grid<F> g (n, m);

    sm::vvec<F> data (g.size());
    for (std::uint32_t i = 0; i < g.size(); ++i) {
        data[i] = std::sin (static_cast<F>(i) * F{0.37}) + F{0.5} * std::cos (static_cast<F>(i) * F{1.13});
    }

    sm::vvec<std::complex<F>> X = sm::hexfft::fft (g, data);
    sm::vvec<std::complex<F>> back = sm::hexfft::ifft (g, X);

    F maxerr = F{0};
    for (std::uint32_t i = 0; i < g.size(); ++i) {
        F err = std::abs (back[i] - std::complex<F> (data[i], F{0}));
        maxerr = std::max (maxerr, err);
    }
    std::cout << "roundtrip n=" << n << " m=" << m << " max error: " << maxerr << std::endl;
    return maxerr < F{1e-8};
}

template<typename F>
static bool is_linear (std::uint32_t n, std::uint32_t m)
{
    sm::hexfft::grid<F> g (n, m);

    sm::vvec<std::complex<F>> x1 (g.size());
    sm::vvec<std::complex<F>> x2 (g.size());
    for (std::uint32_t i = 0; i < g.size(); ++i) {
        x1[i] = std::complex<F> (std::sin (static_cast<F>(i) * F{0.21}), std::cos (static_cast<F>(i) * F{0.05}));
        x2[i] = std::complex<F> (std::cos (static_cast<F>(i) * F{0.44}), std::sin (static_cast<F>(i) * F{0.63}));
    }
    std::complex<F> a (F{1.7}, F{-0.3});
    std::complex<F> b (F{-0.9}, F{0.4});

    sm::vvec<std::complex<F>> combo (g.size());
    for (std::uint32_t i = 0; i < g.size(); ++i) { combo[i] = a * x1[i] + b * x2[i]; }

    sm::vvec<std::complex<F>> X1 = sm::hexfft::fft (g, x1);
    sm::vvec<std::complex<F>> X2 = sm::hexfft::fft (g, x2);
    sm::vvec<std::complex<F>> Xcombo = sm::hexfft::fft (g, combo);

    F maxerr = F{0};
    for (std::uint32_t i = 0; i < g.size(); ++i) {
        std::complex<F> expected = a * X1[i] + b * X2[i];
        maxerr = std::max (maxerr, std::abs (Xcombo[i] - expected));
    }
    std::cout << "linearity n=" << n << " m=" << m << " max error: " << maxerr << std::endl;
    return maxerr < F{1e-8};
}

// Check that sm::hexgrid neighbour relations were followed correctly: hexes0[r][c] should sit
// at axial coordinates (ri=c, gi=2r), hexes1[r][c] at (ri=c, gi=2r+1).
template<typename F>
static bool grid_geometry_ok (std::uint32_t n, std::uint32_t m)
{
    sm::hexfft::grid<F> g (n, m);
    bool ok = true;
    for (std::uint32_t r = 0; r < n; ++r) {
        for (std::uint32_t c = 0; c < m; ++c) {
            auto h0 = g.hexes0[r][c];
            auto h1 = g.hexes1[r][c];
            if (h0->ri != static_cast<std::int32_t>(c) || h0->gi != static_cast<std::int32_t>(2 * r)) { ok = false; }
            if (h1->ri != static_cast<std::int32_t>(c) || h1->gi != static_cast<std::int32_t>(2 * r + 1)) { ok = false; }
        }
    }
    std::cout << "grid geometry n=" << n << " m=" << m << (ok ? " OK" : " FAIL") << std::endl;
    return ok;
}

std::int32_t main()
{
    std::int32_t rtn = 0;

    // n=4 (power-of-two row-halves) with m=3 (non-power-of-two column length; exercises Bluestein).
    if (!grid_geometry_ok<double> (4, 3)) { --rtn; }
    if (!roundtrips<double> (4, 3)) { --rtn; }
    if (!is_linear<double> (4, 3)) { --rtn; }

    // n=6 (non-power-of-two row-halves of length 3; exercises Bluestein in the row transform too)
    // with m=5.
    if (!grid_geometry_ok<double> (6, 5)) { --rtn; }
    if (!roundtrips<double> (6, 5)) { --rtn; }
    if (!is_linear<double> (6, 5)) { --rtn; }

    // n=2, m=1: smallest legal grid.
    if (!roundtrips<double> (2, 1)) { --rtn; }

    if (rtn != 0) {
        std::cout << "FAIL" << std::endl;
    } else {
        std::cout << "SUCCESS" << std::endl;
    }
    return rtn;
}
