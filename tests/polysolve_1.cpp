#include <sm/vvec>
#include <sm/polysolve>
#include <iostream>
#include <iomanip>

template<typename T>
void print_roots (const sm::vvec<std::complex<T>>& roots, const T thresh = 1e-10)
{
    std::cout << std::fixed << std::setprecision(6);
    for (size_t i = 0; i < roots.size(); ++i) {
        std::cout << "  x" << i << " = " << roots[i].real();
        if (std::abs (roots[i].imag()) > thresh) {
            std::cout << (roots[i].imag() >= 0 ? " + " : " - ")
                      << std::abs(roots[i].imag()) << "i";
        }
        std::cout << std::endl;
    }
}

// Return true if rt matches rt_expect
template<typename T>
bool test_root (const std::complex<T>& rt,
                const std::complex<T>& rt_expect,
                const T thresh = std::numeric_limits<T>::epsilon())
{
    bool real_part = std::abs(std::real(rt) - std::real(rt_expect)) <= thresh;
    bool imag_part = std::abs(std::imag(rt) - std::imag(rt_expect)) <= thresh;
    if (!real_part) {
        std::cout << std::scientific
                  << "Real part delta = " << std::abs(std::real(rt) - std::real(rt_expect))
                  << " > thresh = " << thresh << " for expected root " << rt_expect << "\n";
    }
    if (!imag_part) {
        std::cout << std::scientific
                  << "Imag part delta = " << std::abs(std::imag(rt) - std::imag(rt_expect))
                  << " > thresh = " << thresh << " for expected root " << rt_expect << "\n";
    }
    return real_part && imag_part;
}

template<typename T>
void test_polysolve (const sm::vvec<T>& poly,
                     const sm::vvec<std::complex<T>>& expected_roots,
                     const T thresh = std::numeric_limits<T>::epsilon())
{
    std::cout << "\nPolynomial: ";
    for (uint32_t i = poly.size(); i > 0; i--) {
        if (poly[i-1] != T{0}) {
            std::cout << ((poly[i-1] > T{0} && i != poly.size()) ? "+" : "") << poly[i-1];
            if (i - 1 > 0) { std::cout << "x^" << (i-1); }
            std::cout << " ";
        }
    }
    std::cout << " = 0" << std::endl;
    std::cout << "Expected: x = " << std::endl;
    for (auto er : expected_roots) {
        std::cout << er << ", ";
    }
    std::cout << std::endl;
    sm::vvec<std::complex<T>> roots = sm::polysolve::solve<T>(poly);
    print_roots (roots, thresh);
    if (roots.size() != expected_roots.size()) { throw std::runtime_error ("Wrong number of roots"); }
    for (uint32_t i = 0; i < expected_roots.size(); ++i) {
        if (test_root (roots[i], expected_roots[i], thresh) == false)
        { throw std::runtime_error ("FAILED"); }
    }
}

void test_linear()
{
    std::cout << "\n=== LINEAR TESTS ===\n";

    // 2x - 6 = 0
    test_polysolve<double> (sm::vvec<double>{-6, 2},
                            sm::vvec<std::complex<double>>{{3}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // -3x + 12 = 0
    test_polysolve<double> (sm::vvec<double>{12, -3},
                            sm::vvec<std::complex<double>>{{4}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // 0.5x + 2.5 = 0
    test_polysolve<double> (sm::vvec<double>{2.5, 0.5},
                            sm::vvec<std::complex<double>>{{-5}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
}

void test_quadratic()
{
    std::cout << "\n=== QUADRATIC TESTS ===\n";

    // x^2 - 5x + 6 = 0
    test_polysolve<double> (sm::vvec<double>{6, -5, 1},
                            sm::vvec<std::complex<double>>{{2, 0}, {3, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^2 + 1 = 0 (complex roots)
    test_polysolve<double> (sm::vvec<double>{1, 0, 1},
                            sm::vvec<std::complex<double>>{{0, -1}, {0, 1}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^2 + 4x + 4 = 0 (repeated root)
    test_polysolve<double> (sm::vvec<double>{4, 4, 1},
                            sm::vvec<std::complex<double>>{{-2, 0}, {-2, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // 2x^2 - 8x + 6 = 0
    test_polysolve<double> (sm::vvec<double>{6, -8, 2},
                            sm::vvec<std::complex<double>>{{1, 0}, {3, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^2 + 2x + 5 = 0 (complex)
    test_polysolve<double> (sm::vvec<double>{5, 2, 1},
                            sm::vvec<std::complex<double>>{{-1, -2}, {-1, 2}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^2 - 2 = 0
    test_polysolve<double> (sm::vvec<double>{-2, 0, 1},
                            sm::vvec<std::complex<double>>{{-sm::mathconst<double>::root_2, 0}, {sm::mathconst<double>::root_2, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // 3x^2 + 6x + 9 = 0 (complex conjugate roots)
    test_polysolve<double> (sm::vvec<double>{9, 6, 3},
                            sm::vvec<std::complex<double>>{{-1, -sm::mathconst<double>::root_2}, {-1, sm::mathconst<double>::root_2}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
}

void test_cubic()
{
    std::cout << "\n=== CUBIC TESTS ===\n";

    // x^3 - 6x^2 + 11x - 6 = 0
    test_polysolve<double> (sm::vvec<double>{-6, 11, -6, 1},
                            sm::vvec<std::complex<double>>{{1, 0}, {2, 0}, {3, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^3 - 1 = 0 (cube roots of unity)
    test_polysolve<double> (sm::vvec<double>{-1, 0, 0, 1},
                            sm::vvec<std::complex<double>>{{-0.5, -sm::mathconst<double>::root_3_over_2},
                                                           {-0.5, sm::mathconst<double>::root_3_over_2}, {1, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^3 + 8 = 0
    test_polysolve<double> (sm::vvec<double>{8, 0, 0, 1},
                            sm::vvec<std::complex<double>>{{-2, 0}, {1, -sm::mathconst<double>::root_3}, {1, sm::mathconst<double>::root_3}},
                            (std::numeric_limits<double>::epsilon() * 2.0));
    // x^3 - 3x^2 + 3x - 1 = 0 (repeated root)
    test_polysolve<double> (sm::vvec<double>{-1, 3, -3, 1},
                            sm::vvec<std::complex<double>>{{1, 0}, {1, 0}, {1, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^3 + 3x^2 + 3x + 1 = 0
    test_polysolve<double> (sm::vvec<double>{1, 3, 3, 1},
                            sm::vvec<std::complex<double>>{{-1, 0}, {-1, 0}, {-1, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // 2x^3 - 4x^2 - 22x + 24 = 0
    test_polysolve<double> (sm::vvec<double>{24, -22, -4, 2},
                            sm::vvec<std::complex<double>>{{-3, 0}, {1, 0}, {4, 0}},
                            (std::numeric_limits<double>::epsilon() * 5.0));
    // x^3 - 7x - 6 = 0
    test_polysolve<double> (sm::vvec<double>{-6, -7, 0, 1},
                            sm::vvec<std::complex<double>>{{-2, 0}, {-1, 0}, {3, 0}},
                            (std::numeric_limits<double>::epsilon() * 5.0));
    // x^3 - 15x - 4 = 0
    test_polysolve<double> (sm::vvec<double>{-4, -15, 0, 1},
                            sm::vvec<std::complex<double>>{{(-2.0 - sm::mathconst<double>::root_3), 0},
                                                           {(sm::mathconst<double>::root_3 - 2.0), 0},
                                                           {4, 0}},
                            (std::numeric_limits<double>::epsilon() * 7.0));
}

void test_quartic()
{
    std::cout << "\n=== QUARTIC TESTS ===\n";

    // x^4 - 10x^2 + 9 = 0 (biquadratic)
    test_polysolve<double> (sm::vvec<double>{9, 0, -10, 0, 1},
                            sm::vvec<std::complex<double>>{{-3, 0}, {-1, 0}, {1, 0}, {3, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // (x-1)(x-2)(x-3)(x-4) = x^4 - 10x^3 + 35x^2 - 50x + 24 = 0
    test_polysolve<double> (sm::vvec<double>{24, -50, 35, -10, 1},
                            sm::vvec<std::complex<double>>{{1, 0}, {2, 0}, {3, 0}, {4, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // (x^2+1)(x-1)^2 = x^4 - 2x^3 + 2x^2 - 2x + 1 = 0
    test_polysolve<double> (sm::vvec<double>{1, -2, 2, -2, 1},
                            sm::vvec<std::complex<double>>{{0, -1}, {0, 1}, {1, 0}, {1, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^4 - 1 = 0 (fourth roots of unity)
    test_polysolve<double> (sm::vvec<double>{-1, 0, 0, 0, 1},
                            sm::vvec<std::complex<double>>{{-1, 0}, {0, -1}, {0, 1}, {1, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^4 + 4x^2 + 4 = 0
    test_polysolve<double> (sm::vvec<double>{4, 0, 4, 0, 1},
                            sm::vvec<std::complex<double>>{{0, -sm::mathconst<double>::root_2}, {0, -sm::mathconst<double>::root_2},
                                                           {0, sm::mathconst<double>::root_2}, {0, sm::mathconst<double>::root_2}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^4 - 5x^2 + 4 = 0
    test_polysolve<double> (sm::vvec<double>{4, 0, -5, 0, 1},
                            sm::vvec<std::complex<double>>{{-2, 0}, {-1, 0}, {1, 0}, {2, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^4 + x^3 - 7x^2 - x + 6 = 0
    test_polysolve<double> (sm::vvec<double>{6, -1, -7, 1, 1},
                            sm::vvec<std::complex<double>>{{-3, 0}, {-1, 0}, {1, 0}, {2, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));

    // 2x^4 - 8x^3 + 8x^2 - 8x + 6 = 0
    test_polysolve<double> (sm::vvec<double>{6, -8, 8, -8, 2},
                            sm::vvec<std::complex<double>>{{0, -1}, {0, 1}, {1, 0}, {3, 0}},
                            (std::numeric_limits<double>::epsilon() * 5.0));
}

void test_special_cases()
{
    std::cout << "\n=== 'SPECIAL' TESTS ===\n";

    // 100x^2 - 500x + 600 = 0 (large coefficients)
    test_polysolve<double> (sm::vvec<double>{600, -500, 100},
                            sm::vvec<std::complex<double>>{{2}, {3}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // 0.001x^2 - 0.003x + 0.002 = 0 (small coefficients)
    test_polysolve<double> (sm::vvec<double>{0.002, -0.003, 0.001},
                            sm::vvec<std::complex<double>>{{1}, {2}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // -x^3 + 6x^2 - 11x + 6 = 0 (negative leading)
    test_polysolve<double> (sm::vvec<double>{6, -11, 6, -1},
                            sm::vvec<std::complex<double>>{{1}, {2}, {3}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
    // x^4 - 16 = 0 (zero coefficient terms)
    test_polysolve<double> (sm::vvec<double>{-16, 0, 0, 0, 1},
                            sm::vvec<std::complex<double>>{{-2, 0}, {0, -2}, {0, 2}, {2, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
}

void test_mixed_roots()
{
    std::cout << "\n=== 'MIXED ROOT' TESTS ===\n";

    // x^3 - 5x^2 - 29x + 105 = 0 (+ve and -ve roots)
    test_polysolve<double> (sm::vvec<double>{105, -29, -5, 1},
                            sm::vvec<std::complex<double>>{{-5}, {3}, {7}},
                            (std::numeric_limits<double>::epsilon() * 12.0));

#if 0
    // x^3 - 4.5x^2 + 6.25x - 1.875 = 0 // This is a real failure??
    test_polysolve<double> (sm::vvec<double>{-1.875, 6.25, -4.5, 1},
                            sm::vvec<std::complex<double>>{{0.4100094639209213574907889, 0},
                                                           {2.044995268039539321254606, -0.625347524626481535021127},
                                                           {2.044995268039539321254606, 0.625347524626481535021127}},
                            (std::numeric_limits<double>::epsilon() * 12.0));
#endif

    // x^4 - 3x^3 + 3x^2 - 3x + 2 = 0 (complex and real)
    test_polysolve<double> (sm::vvec<double>{2, -3, 3, -3, 1},
                            sm::vvec<std::complex<double>>{{0, -1}, {0, 1}, {1, 0}, {2, 0}},
                            (std::numeric_limits<double>::epsilon() * 1.0));
}

void test_higher_degree()
{
    std::cout << "\n=== HIGHER DEGREE TESTS ===\n";

    // (x-1)(x-2)(x-3)(x-4)(x-5) = x^5 - 15x^4 + 85x^3 - 225x^2 + 274x - 120 = 0 (degree 5)
    test_polysolve<double> (sm::vvec<double>{-120, 274, -225, 85, -15, 1},
                            sm::vvec<std::complex<double>>{{1}, {2}, {3}, {4}, {5}}, 4e-14);

    // (x+1)(x-1)(x+2)(x-2)(x+3)(x-3) = x^6 -14x^4 + 49 x^2 - 36 = 0
    test_polysolve<double> (sm::vvec<double>{-36, 0, 49, 0, -14, 0, 1},
                            sm::vvec<std::complex<double>>{{-3}, {-2}, {-1}, {1}, {2}, {3}},
                            1e-16);
    // x^5 - 32 = 0
    test_polysolve<double> (sm::vvec<double>{-32, 0, 0, 0, 0, 1},
                            sm::vvec<std::complex<double>>{{-1.61803398874989, -1.17557050458495},
                                                           {-1.61803398874989, 1.17557050458495},
                                                           {0.61803398874989, -1.90211303259031},
                                                           {0.61803398874989, 1.90211303259031},
                                                           {2, 0}}, 1e-14);
    // (x-1)(x-2)...(x-7) = x^7 - 28x^6 + 322x^5 - 1960x^4 + 6769x^3 - 13132x^2 + 13068x - 5040 = 0
    test_polysolve<double> (sm::vvec<double>{-5040, 13068, -13132, 6769, -1960, 322, -28, 1},
                            sm::vvec<std::complex<double>>{{1},{2},{3},{4},{5},{6},{7}}, 2e-12);
}

void test_template_types()
{
    // x^2 - 5x + 6 = 0 (float)
    test_polysolve<float> (sm::vvec<float>{6.0f, -5.0f, 1.0f}, sm::vvec<std::complex<float>>{{2.0f},{3.0f}});
    // x^3 - 6x^2 + 11x - 6 = 0 (long double)
    test_polysolve<long double> (sm::vvec<long double>{-6.0L, 11.0L, -6.0L, 1.0L},
                                 sm::vvec<std::complex<long double>>{{1.0L},{2.0L},{3.0L}}, 1e-16);
    // x^2 + 1 = 0 (float, complex roots)
    test_polysolve<float> (sm::vvec<float>{1.0f, 0.0f, 1.0f}, sm::vvec<std::complex<float>>{{0.0f, -1.0f},{0.0f, 1.0f}});
    // x^4 - 10x^2 + 9 = 0 (float, real roots)
    test_polysolve<float> (sm::vvec<float>{9.0f, 0.0f, -10.0f, 0.0f, 1.0f},
                           sm::vvec<std::complex<float>>{{-3.0f},{-1.0f},{1.0f},{3.0f}});
}

// Tests that really seem to be computed incorrectly
void test_failures()
{
    std::cout << "\n=== FALSE FAILURES (THESE SHOULD PASS) ===\n";

    // This is a real failure?? from test_mixed_roots(). Expected roots obtained from https://www.wolframalpha.com
    // x^3 - 4.5x^2 + 6.25x - 1.875 = 0
    test_polysolve<double> (sm::vvec<double>{-1.875, 6.25, -4.5, 1},
                            sm::vvec<std::complex<double>>{{0.4100094639209213574907889, 0},
                                                           {2.044995268039539321254606, -0.625347524626481535021127},
                                                           {2.044995268039539321254606, 0.625347524626481535021127}},
                            (std::numeric_limits<double>::epsilon() * 12.0));
}

int main()
{
    int rtn = 0;
    std::cout << "Polynomial Solver Tests" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "Testing analytical solutions (degrees 1-4) and" << std::endl;
    std::cout << "numerical Durand-Kerner method (degree > 4)" << std::endl;

    try {
        // Each test will throw exceptions if the test fails
        test_linear();
        test_quadratic();
        test_cubic();
        test_quartic();
        test_special_cases();
        test_mixed_roots();
        test_higher_degree();
        test_template_types ();
        test_failures();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        --rtn;
    }

    // Could also do some tests that are expected to throw

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nTest failed\n");
    return rtn;
}
