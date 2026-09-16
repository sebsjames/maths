#include <cstdint>
#include <cmath>
#include <complex>
#include <list>
#include <iostream>

import sm.hexfft;
import sm.hexgrid;
import sm.hex;
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

    sm::hexfft::fft<F> hfft (&hg, data);
    sm::vvec<std::complex<F>> back = hfft.inverse();

    F maxerr = F{0};
    for (std::uint32_t i = 0; i < hg.num(); ++i) {
        F err = std::abs (back[i] - std::complex<F> (data[i], F{0}));
        maxerr = std::max (maxerr, err);
    }
    std::cout << label << ": hg.num()=" << hg.num() << " spectrum rows=" << hfft.asa_rows << " cols=" << hfft.asa_cols
               << " (size " << hfft.size() << ") max roundtrip error: " << maxerr << std::endl;
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

    sm::hexfft::fft<F> hfft1 (&hg, x1);
    sm::hexfft::fft<F> hfft2  (&hg, x2);
    sm::hexfft::fft<F> hfft_combined  (&hg, combo);

    F maxerr = F{0};
    for (std::uint32_t i = 0; i < hfft_combined.X_hexgrid.size(); ++i) {
        std::complex<F> expected = a * hfft1.X_hexgrid[i] + b * hfft2.X_hexgrid[i];
        maxerr = std::max (maxerr, std::abs (hfft_combined.X_hexgrid[i] - expected));
    }
    std::cout << label << ": linearity max error: " << maxerr << std::endl;
    return maxerr < F{1e-8};
}

static bool hex_data_ifft_runs_when_padded()
{
    sm::hexgrid<double> hg (1.0, 30.0);
    hg.set_circular_boundary (6.0);

    sm::vvec<double> data = make_data<double> (hg.num());
    sm::hexfft::fft<double> hfft (&hg, data);
    sm::vvec<std::complex<double>> back = hfft.inverse();

    bool ok = (back.size() == hg.num());
    double maxerr = 0.0;
    for (std::uint32_t i = 0; i < hg.num(); ++i) {
        maxerr = std::max (maxerr, std::abs (back[i] - std::complex<double> (data[i], 0.0)));
    }
    std::cout << "hex_data_ifft_runs_when_padded: hg.num()=" << hg.num() << " hfft.size()=" << hfft.size()
               << " max error vs original (does not affect test): " << maxerr
               << (ok ? " Runs OK" : " Run returns wrong size") << std::endl;
    return ok;
}

std::int32_t main()
{
    std::int32_t rtn = 0;

    if (!hex_data_ifft_runs_when_padded()) { --rtn; }

    // A parallelogram boundary
    {
        sm::hexgrid<double> hg (1.0, 40.0);
        hg.set_parallelogram_boundary (2, 3);
        if (!roundtrips<double> (hg, "parallelogram")) { --rtn; }
        if (!is_linear<double> (hg, "parallelogram")) { --rtn; }
    }

    // An arbitrary boundary: a circle.
    {
        sm::hexgrid<double> hg (1.0, 30.0);
        hg.set_circular_boundary (6.0);
        if (!roundtrips<double> (hg, "circular")) { --rtn; }
        if (!is_linear<double> (hg, "circular")) { --rtn; }
    }

    // A second, differently-sized circular boundary.
    {
        sm::hexgrid<double> hg (1.0, 24.0);
        hg.set_circular_boundary (4.5);
        if (!roundtrips<double> (hg, "circular2")) { --rtn; }
        if (!is_linear<double> (hg, "circular2")) { --rtn; }
    }

    if (rtn != 0) {
        std::cout << "FAIL" << std::endl;
    } else {
        std::cout << "SUCCESS" << std::endl;
    }
    return rtn;
}
