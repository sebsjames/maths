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

// The (a, r, c) position that hex h occupies in a spectrum with the given ri_min, gi_min.
// This mirrors sm::hexfft::detail::asa_position exactly (see the comment there for the
// reasoning): a, r come straight from hex::gi's parity and hex::gi/2; c is hex::ri + r, not
// hex::ri on its own, because each successive row's hex::ri is shifted by one column relative
// to the row below it (hex::compute_location).
template<typename F>
static void asa_position (const sm::hex<F>& h, std::int32_t ri_min, std::int32_t gi_min,
                          std::uint32_t& a, std::uint32_t& r, std::uint32_t& c)
{
    std::int32_t r_signed = (h.gi - gi_min) / 2;
    a = static_cast<std::uint32_t> ((h.gi - gi_min) - 2 * r_signed);
    r = static_cast<std::uint32_t> (r_signed);
    c = static_cast<std::uint32_t> (h.ri + r_signed - ri_min);
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

// The real point of the fix: for every hex in hg, its (a, r, c) position (computed from
// spectrum::ri_min/gi_min, exactly as sm::hexfft does internally) should predict its actual
// Cartesian position, relative to the (a=0,r=0,c=0) corner (itself at hex::ri==ri_min,
// hex::gi==gi_min): hex::x == x0 + c*d + a*d/2, hex::y == y0 + (2r+a)*v. If that holds for
// every hex, then plotting either of the two (a=0, a=1) arrays by (r, c) gives a genuine
// rectangle, not a parallelogram.
template<typename F>
static bool asa_layout_is_rectangular (sm::hexgrid<F>& hg, const char* label)
{
    sm::vvec<F> data (hg.num(), F{1});
    sm::hexfft::spectrum<F> X = sm::hexfft::fft (hg, data);

    F x0 = static_cast<F>(X.ri_min) * hg.d + static_cast<F>(X.gi_min) * hg.d / F{2};
    F y0 = static_cast<F>(X.gi_min) * hg.v;

    F maxerr = F{0};
    for (const auto& h : hg.hexen) {
        std::uint32_t a, r, c;
        asa_position (h, X.ri_min, X.gi_min, a, r, c);
        F expected_x = x0 + static_cast<F>(c) * hg.d + static_cast<F>(a) * hg.d / F{2};
        F expected_y = y0 + static_cast<F>(2 * r + a) * hg.v;
        maxerr = std::max (maxerr, std::abs (h.x - expected_x));
        maxerr = std::max (maxerr, std::abs (h.y - expected_y));
    }
    std::cout << label << ": asa_layout_is_rectangular max (x,y) error: " << maxerr << std::endl;
    return maxerr < F{1e-8};
}

// hex_data[vi] should equal X.data read at hex h's own (a,r,c) position AFTER fftshifting:
// under fftshift (matching numpy/MATLAB's convention), the value originally at (r,c) in an
// (n,m) array moves to ((r+n/2)%n, (c+m/2)%m), so *reading* the shifted array back at (r,c)
// recovers the ORIGINAL value from ((r-n/2)%n, (c-m/2)%c) -- i.e. subtract the shift, not add
// it. (For even sizes, +shift and -shift give the same answer mod N, which is why this only
// shows up as an error for the odd dimension below.)
template<typename F>
static bool hex_data_matches_data (sm::hexgrid<F>& hg, const char* label)
{
    sm::vvec<F> data = make_data<F> (hg.num());
    sm::hexfft::spectrum<F> X = sm::hexfft::fft (hg, data);

    if (X.hex_data.size() != hg.num()) {
        std::cout << label << ": hex_data size mismatch" << std::endl;
        return false;
    }

    F maxerr = F{0};
    for (const auto& h : hg.hexen) {
        std::uint32_t a, r, c;
        asa_position (h, X.ri_min, X.gi_min, a, r, c);
        std::uint32_t rs = (r + X.n - X.n / 2u) % X.n;
        std::uint32_t cs = (c + X.m - X.m / 2u) % X.m;
        std::complex<F> expected = X.data[a * X.n * X.m + rs * X.m + cs];
        maxerr = std::max (maxerr, std::abs (X.hex_data[h.vi] - expected));
    }
    std::cout << label << ": hex_data vs data max error: " << maxerr << std::endl;
    return maxerr == F{0};
}

// Directly build a hexgrid whose hexes are exactly the n-row-pair by m-column ASA rectangle
// (see asa_position): its perimeter, in (ri, gi) terms, is
//   bottom edge (a=0,r=0):  gi=0,      ri=c,           c=0..m-1
//   right edge (c=m-1):     gi=0..2n-1, ri=(m-1)-gi/2
//   top edge (a=1,r=n-1):   gi=2n-1,   ri=c-(n-1),     c=m-1..0
//   left edge (c=0):        gi=2n-1..0, ri=-(gi/2)
// Passing that ring to hexgrid::set_boundary keeps exactly the hexes inside it, with no
// zero-padding gap at all: hg.num() == 2*n*m exactly.
static sm::hexgrid<double> build_gapless_asa_grid (std::uint32_t n, std::uint32_t m, double d = 1.0)
{
    std::list<sm::hex<double>> perim;
    std::uint32_t idx = 0;
    for (std::uint32_t c = 0; c < m; ++c) {
        perim.emplace_back (idx++, d, static_cast<std::int32_t>(c), 0);
    }
    for (std::int32_t gi = 0; gi <= static_cast<std::int32_t>(2 * n - 1); ++gi) {
        perim.emplace_back (idx++, d, static_cast<std::int32_t>(m - 1) - (gi / 2), gi);
    }
    for (std::int32_t c = static_cast<std::int32_t>(m) - 1; c >= 0; --c) {
        perim.emplace_back (idx++, d, c - static_cast<std::int32_t>(n - 1), static_cast<std::int32_t>(2 * n - 1));
    }
    for (std::int32_t gi = static_cast<std::int32_t>(2 * n - 1); gi >= 0; --gi) {
        perim.emplace_back (idx++, d, -(gi / 2), gi);
    }
    sm::hexgrid<double> hg (d, d * static_cast<double>(2 * (m + n) + 10));
    hg.set_boundary (perim);
    return hg;
}

// When hg's boundary exactly fills its bounding rectangle (no zero-padded region separates
// hg.num() from the spectrum's size), spectrum::hex_data carries the same information as
// spectrum::data, so ifft(hg, hex_data) is an exact inverse, just like ifft(hg, spectrum).
static bool hex_data_ifft_lossless_when_no_padding()
{
    sm::hexgrid<double> hg = build_gapless_asa_grid (4, 5);

    sm::vvec<double> data = make_data<double> (hg.num());
    sm::hexfft::spectrum<double> X = sm::hexfft::fft (hg, data);

    bool ok = (X.size() == hg.num()); // i.e. there is indeed no padding gap here
    sm::vvec<std::complex<double>> back = sm::hexfft::ifft (hg, X.hex_data);
    double maxerr = 0.0;
    for (std::uint32_t i = 0; i < hg.num(); ++i) {
        maxerr = std::max (maxerr, std::abs (back[i] - std::complex<double> (data[i], 0.0)));
    }
    std::cout << "hex_data_ifft_lossless_when_no_padding: hg.num()=" << hg.num() << " X.size()=" << X.size()
               << " max error " << maxerr << (ok ? "" : " (unexpectedly padded!)") << std::endl;
    return ok && maxerr < 1e-8;
}

// When hg's boundary does NOT fill its bounding rectangle (e.g. a circular boundary),
// spectrum::hex_data has already discarded the padded region's frequency content, so
// ifft(hg, hex_data) is only a filtered approximation of the original data, unlike
// ifft(hg, spectrum), which is exact. This just checks the overload runs and returns the
// right size; the (expected, documented) mismatch against the original data is printed for
// information rather than asserted on.
static bool hex_data_ifft_runs_when_padded()
{
    sm::hexgrid<double> hg (1.0, 30.0);
    hg.set_circular_boundary (6.0);

    sm::vvec<double> data = make_data<double> (hg.num());
    sm::hexfft::spectrum<double> X = sm::hexfft::fft (hg, data);
    sm::vvec<std::complex<double>> back = sm::hexfft::ifft (hg, X.hex_data);

    bool ok = (back.size() == hg.num());
    double maxerr = 0.0;
    for (std::uint32_t i = 0; i < hg.num(); ++i) {
        maxerr = std::max (maxerr, std::abs (back[i] - std::complex<double> (data[i], 0.0)));
    }
    std::cout << "hex_data_ifft_runs_when_padded: hg.num()=" << hg.num() << " X.size()=" << X.size()
               << " max error vs original (expected well above zero): " << maxerr
               << (ok ? " OK" : " FAIL") << std::endl;
    return ok;
}

std::int32_t main()
{
    std::int32_t rtn = 0;

    if (!hex_data_ifft_lossless_when_no_padding()) { --rtn; }
    if (!hex_data_ifft_runs_when_padded()) { --rtn; }

    {
        sm::hexgrid<double> hg (1.0, 40.0);
        hg.set_parallelogram_boundary (2, 3);
        if (!roundtrips<double> (hg, "parallelogram")) { --rtn; }
        if (!is_linear<double> (hg, "parallelogram")) { --rtn; }
        if (!hex_data_matches_data<double> (hg, "parallelogram")) { --rtn; }
        if (!asa_layout_is_rectangular<double> (hg, "parallelogram")) { --rtn; }
    }

    // An arbitrary (non-rectangular-in-ri,gi) boundary: a circle. This exercises the
    // zero-padding path: hg.num() will be well short of the padded rectangle's size.
    {
        sm::hexgrid<double> hg (1.0, 30.0);
        hg.set_circular_boundary (6.0);
        if (!roundtrips<double> (hg, "circular")) { --rtn; }
        if (!is_linear<double> (hg, "circular")) { --rtn; }
        if (!hex_data_matches_data<double> (hg, "circular")) { --rtn; }
        if (!asa_layout_is_rectangular<double> (hg, "circular")) { --rtn; }
    }

    // A second, differently-sized circular boundary, to exercise a different mix of
    // power-of-two/non-power-of-two internal FFT sizes.
    {
        sm::hexgrid<double> hg (1.0, 24.0);
        hg.set_circular_boundary (4.5);
        if (!roundtrips<double> (hg, "circular2")) { --rtn; }
        if (!asa_layout_is_rectangular<double> (hg, "circular2")) { --rtn; }
    }

    if (rtn != 0) {
        std::cout << "FAIL" << std::endl;
    } else {
        std::cout << "SUCCESS" << std::endl;
    }
    return rtn;
}
