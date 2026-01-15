#include <sm/polysolve>
#include <iostream>
#include <iomanip>

void print_roots(const sm::vvec<std::complex<double>>& roots) {
    std::cout << std::fixed << std::setprecision(6);
    for (size_t i = 0; i < roots.size(); ++i) {
        std::cout << "  x" << i << " = " << roots[i].real();
        if (std::abs(roots[i].imag()) > 1e-10) {
            std::cout << (roots[i].imag() >= 0 ? " + " : " - ") 
                     << std::abs(roots[i].imag()) << "i";
        }
        std::cout << std::endl;
    }
}

void test_linear(int& rtn) {
    std::cout << "\n=== Linear: 2x - 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = 3" << std::endl;
    sm::vvec<std::complex<double>> roots = sm::polysolve::solve<double, 1>(sm::vec<double, 2>{-6, 2});
    print_roots(roots);
    if (roots.size() != 1 || std::abs(roots[0].real() - 3.0) > 1e-6) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Linear: -3x + 12 = 0 ===" << std::endl;
    std::cout << "Expected: x = 4" << std::endl;
    roots = sm::polysolve::solve<double, 1>(sm::vec<double, 2>{12, -3});
    print_roots(roots);
    if (roots.size() != 1 || std::abs(roots[0].real() - 4.0) > 1e-6) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Linear: 0.5x + 2.5 = 0 ===" << std::endl;
    std::cout << "Expected: x = -5" << std::endl;

    //roots = sm::polysolve::solve<1, double, double>({2.5, 0.5});
    roots = sm::polysolve::solve<double, 1>(sm::vec<double, 2>{2.5, 0.5});

    print_roots(roots);
    if (roots.size() != 1 || std::abs(roots[0].real() + 5.0) > 1e-6) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
}

void test_quadratic(int& rtn) {
    std::cout << "\n=== Quadratic: x^2 - 5x + 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = 2, 3" << std::endl;
    sm::vvec<std::complex<double>> roots = sm::polysolve::solve<double, 2>(sm::vec<double, 3>{6, -5, 1});
    print_roots(roots);
    if (roots.size() != 2) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Quadratic (complex): x^2 + 1 = 0 ===" << std::endl;
    std::cout << "Expected: x = +/-i" << std::endl;
    roots = sm::polysolve::solve<double, 2>(sm::vec<double, 3>{1, 0, 1});
    print_roots(roots);
    if (roots.size() != 2) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Quadratic: x^2 + 4x + 4 = 0 (repeated root) ===" << std::endl;
    std::cout << "Expected: x = -2, -2" << std::endl;
    roots = sm::polysolve::solve<double, 2>(sm::vec<double, 3>{4, 4, 1});
    print_roots(roots);
    
    std::cout << "\n=== Quadratic: 2x^2 - 8x + 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = 1, 3" << std::endl;
    roots = sm::polysolve::solve<double, 2>(sm::vec<double, 3>{6, -8, 2});
    print_roots(roots);
    
    std::cout << "\n=== Quadratic: x^2 + 2x + 5 = 0 (complex) ===" << std::endl;
    std::cout << "Expected: x = -1 +/- 2i" << std::endl;
    roots = sm::polysolve::solve<double, 2>(sm::vec<double, 3>{5, 2, 1});
    print_roots(roots);
    
    std::cout << "\n=== Quadratic: x^2 - 2 = 0 ===" << std::endl;
    std::cout << "Expected: x = +/-sqrt(2) ~= +/-1.414" << std::endl;
    roots = sm::polysolve::solve<double, 2>(sm::vec<double, 3>{-2, 0, 1});
    print_roots(roots);
    
    std::cout << "\n=== Quadratic: 3x^2 + 6x + 9 = 0 ===" << std::endl;
    std::cout << "Expected: complex conjugate roots" << std::endl;
    roots = sm::polysolve::solve<double, 2>(sm::vec<double, 3>{9, 6, 3});
    print_roots(roots);
}

void test_cubic(int& rtn) {
    std::cout << "\n=== Cubic: x^3 - 6x^2 + 11x - 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = 1, 2, 3" << std::endl;
    sm::vvec<std::complex<double>> roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{-6, 11, -6, 1});
    print_roots(roots);
    if (roots.size() != 3) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Cubic: x^3 - 1 = 0 (cube roots of unity) ===" << std::endl;
    std::cout << "Expected: x = 1, -0.5+/-0.866i" << std::endl;
    roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{-1, 0, 0, 1});
    print_roots(roots);
    
    std::cout << "\n=== Cubic: x^3 + 8 = 0 ===" << std::endl;
    std::cout << "Expected: x = -2, 1+/-sqrt(3)i" << std::endl;
    roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{8, 0, 0, 1});
    print_roots(roots);
    
    std::cout << "\n=== Cubic: x^3 - 3x^2 + 3x - 1 = 0 (repeated root) ===" << std::endl;
    std::cout << "Expected: x = 1, 1, 1 (triple root)" << std::endl;
    roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{-1, 3, -3, 1});
    print_roots(roots);
    
    std::cout << "\n=== Cubic: x^3 + 3x^2 + 3x + 1 = 0 ===" << std::endl;
    std::cout << "Expected: x = -1, -1, -1" << std::endl;
    roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{1, 3, 3, 1});
    print_roots(roots);
    
    std::cout << "\n=== Cubic: 2x^3 - 4x^2 - 22x + 24 = 0 ===" << std::endl;
    std::cout << "Expected: x = -3, 1, 4" << std::endl;
    roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{24, -22, -4, 2});
    print_roots(roots);
    
    std::cout << "\n=== Cubic: x^3 - 7x - 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = -1, -2, 3" << std::endl;
    roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{-6, -7, 0, 1});
    print_roots(roots);
    
    std::cout << "\n=== Cubic: x^3 - 15x - 4 = 0 ===" << std::endl;
    std::cout << "Expected: three real roots" << std::endl;
    roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{-4, -15, 0, 1});
    print_roots(roots);
}

void test_quartic(int& rtn) {
    std::cout << "\n=== Quartic: x^4 - 10x^2 + 9 = 0 (biquadratic) ===" << std::endl;
    std::cout << "Expected: x = +/-1, +/-3" << std::endl;
    sm::vvec<std::complex<double>> roots = sm::polysolve::solve<double, 4>(sm::vec<double, 5>{9, 0, -10, 0, 1});
    print_roots(roots);
    if (roots.size() != 4) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Quartic: (x-1)(x-2)(x-3)(x-4) = x^4 - 10x^3 + 35x^2 - 50x + 24 = 0 ===" << std::endl;
    std::cout << "Expected: x = 1, 2, 3, 4" << std::endl;
    roots = sm::polysolve::solve<double, 4>(sm::vec<double, 5>{24, -50, 35, -10, 1});
    print_roots(roots);
    
    // Verify product of roots
    std::complex<double> product(1.0, 0.0);
    for (const auto& r : roots) {
        product *= r;
    }
    std::cout << "  Product: " << product << " (expected: 24)" << std::endl;
    if (std::abs(product.real() - 24.0) > 1e-6 || std::abs(product.imag()) > 1e-6) {
        std::cout << "  FAILED: Product mismatch" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Quartic: (x^2+1)(x-1)^2 = x^4 - 2x^3 + 2x^2 - 2x + 1 = 0 ===" << std::endl;
    std::cout << "Expected: x = i, -i, 1, 1 (complex eigenvalue test case)" << std::endl;
    roots = sm::polysolve::solve<double, 4>(sm::vec<double, 5>{1, -2, 2, -2, 1});
    print_roots(roots);
    
    // Verify roots satisfy the polynomial
    std::cout << "  Verification - |p(root)|:" << std::endl;
    bool verification_passed = true;
    for (size_t i = 0; i < roots.size(); ++i) {
        std::complex<double> z = roots[i];
        std::complex<double> val = z*z*z*z - 2.0*z*z*z + 2.0*z*z - 2.0*z + 1.0;
        double err = std::abs(val);
        std::cout << "    |p(root[" << i << "])| = " << std::scientific << err << std::fixed << std::endl;
        if (err > 1e-10) {
            verification_passed = false;
        }
    }
    
    product = std::complex<double>(1.0, 0.0);
    for (const auto& r : roots) {
        product *= r;
    }
    std::cout << "  Product: " << product << " (expected: 1)" << std::endl;
    if (!verification_passed || std::abs(product.real() - 1.0) > 1e-6 || std::abs(product.imag()) > 1e-6) {
        std::cout << "  FAILED: Verification or product mismatch" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Quartic: x^4 - 1 = 0 (fourth roots of unity) ===" << std::endl;
    std::cout << "Expected: x = +/-1, +/-i" << std::endl;
    roots = sm::polysolve::solve<double, 4>(sm::vec<double, 5>{-1, 0, 0, 0, 1});
    print_roots(roots);
    
    std::cout << "\n=== Quartic: x^4 + 4x^2 + 4 = 0 ===" << std::endl;
    std::cout << "Expected: complex roots" << std::endl;
    roots = sm::polysolve::solve<double, 4>(sm::vec<double, 5>{4, 0, 4, 0, 1});
    print_roots(roots);
    
    std::cout << "\n=== Quartic: x^4 - 5x^2 + 4 = 0 ===" << std::endl;
    std::cout << "Expected: x = +/-1, +/-2" << std::endl;
    roots = sm::polysolve::solve<double, 4>(sm::vec<double, 5>{4, 0, -5, 0, 1});
    print_roots(roots);
    
    std::cout << "\n=== Quartic: x^4 + x^3 - 7x^2 - x + 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = -3, -1, 1, 2" << std::endl;
    roots = sm::polysolve::solve<double, 4>(sm::vec<double, 5>{6, -1, -7, 1, 1});
    print_roots(roots);
    
    std::cout << "\n=== Quartic: 2x^4 - 8x^3 + 8x^2 - 8x + 6 = 0 ===" << std::endl;
    std::cout << "Expected: two real and two complex roots" << std::endl;
    roots = sm::polysolve::solve<double, 4>(sm::vec<double, 5>{6, -8, 8, -8, 2});
    print_roots(roots);
}

void test_real_roots(int& rtn) {
    std::cout << "\n=== Real Roots Filter: x^3 - 6x^2 + 11x - 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = 1, 2, 3 (all real)" << std::endl;
    sm::vvec<double> realRoots = sm::polysolve::real<double, 3>(sm::vec<double, 4>{-6, 11, -6, 1});
    for (size_t i = 0; i < realRoots.size(); ++i) {
        std::cout << "  x" << i << " = " << realRoots[i] << std::endl;
    }
    if (realRoots.size() != 3) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Real Roots Filter: x^3 - 1 = 0 ===" << std::endl;
    std::cout << "Expected: x = 1 (only real root)" << std::endl;
    realRoots = sm::polysolve::real<double, 3>(sm::vec<double, 4>{-1, 0, 0, 1});
    for (size_t i = 0; i < realRoots.size(); ++i) {
        std::cout << "  x" << i << " = " << realRoots[i] << std::endl;
    }
    if (realRoots.size() != 1) {
        std::cout << "FAILED: Expected 1 real root, got " << realRoots.size() << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Real Roots Filter: x^2 + 1 = 0 ===" << std::endl;
    std::cout << "Expected: no real roots" << std::endl;
    realRoots = sm::polysolve::real<double, 2>(sm::vec<double, 3>{1, 0, 1});
    if (realRoots.empty()) {
        std::cout << "  No real roots (correct!)" << std::endl;
    } else {
        for (size_t i = 0; i < realRoots.size(); ++i) {
            std::cout << "  x" << i << " = " << realRoots[i] << std::endl;
        }
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
}

void test_special_cases(int& rtn) {
    std::cout << "\n=== Special Case: 100x^2 - 500x + 600 = 0 (large coefficients) ===" << std::endl;
    std::cout << "Expected: x = 2, 3" << std::endl;
    sm::vvec<std::complex<double>> roots = sm::polysolve::solve<double, 2>(sm::vec<double, 3>{600, -500, 100});
    print_roots(roots);
    if (roots.size() != 2) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Special Case: 0.001x^2 - 0.003x + 0.002 = 0 (small coefficients) ===" << std::endl;
    std::cout << "Expected: x = 1, 2" << std::endl;
    roots = sm::polysolve::solve<double, 2>(sm::vec<double, 3>{0.002, -0.003, 0.001});
    print_roots(roots);
    
    std::cout << "\n=== Special Case: -x^3 + 6x^2 - 11x + 6 = 0 (negative leading) ===" << std::endl;
    std::cout << "Expected: x = 1, 2, 3" << std::endl;
    roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{6, -11, 6, -1});
    print_roots(roots);
    
    std::cout << "\n=== Special Case: x^4 - 16 = 0 (zero coefficient terms) ===" << std::endl;
    std::cout << "Expected: x = +/-2, +/-2i" << std::endl;
    roots = sm::polysolve::solve<double, 4>(sm::vec<double, 5>{-16, 0, 0, 0, 1});
    print_roots(roots);
}

void test_mixed_roots(int& rtn) {
    std::cout << "\n=== Mixed: x^3 - 5x^2 - 29x + 105 = 0 ===" << std::endl;
    std::cout << "Expected: x = -5, 3, 7 (positive and negative)" << std::endl;
    sm::vvec<std::complex<double>> roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{105, -29, -5, 1});
    print_roots(roots);
    if (roots.size() != 3) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Mixed: x^3 - 4.5x^2 + 6.25x - 1.875 = 0 ===" << std::endl;
    std::cout << "Expected: x = 0.5, 1.5, 2.5 (fractional)" << std::endl;
    roots = sm::polysolve::solve<double, 3>(sm::vec<double, 4>{-1.875, 6.25, -4.5, 1});
    print_roots(roots);
    
    std::cout << "\n=== Mixed: x^4 - 3x^3 + 3x^2 - 3x + 2 = 0 ===" << std::endl;
    std::cout << "Expected: x = 1, 2, +/-i (complex and real)" << std::endl;
    roots = sm::polysolve::solve<double, 4>(sm::vec<double, 5>{2, -3, 3, -3, 1});
    print_roots(roots);
}

void test_higher_degree(int& rtn) {
    std::cout << "\n=== Degree 5: (x-1)(x-2)(x-3)(x-4)(x-5) = 0 ===" << std::endl;
    std::cout << "Expected: x = 1, 2, 3, 4, 5" << std::endl;
    sm::vvec<std::complex<double>> roots = sm::polysolve::solve<double, 5>(sm::vec<double, 6>{-120, 274, -225, 85, -15, 1});
    print_roots(roots);
    if (roots.size() != 5) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Degree 6: (x+1)(x-1)(x+2)(x-2)(x+3)(x-3) = 0 ===" << std::endl;
    std::cout << "Expected: x = +/-1, +/-2, +/-3" << std::endl;
    sm::vvec<std::complex<double>> roots6 = sm::polysolve::solve<double, 6>(sm::vec<double, 7>{-36, 0, 49, 0, -14, 0, 1});
    print_roots(roots6);
    if (roots6.size() != 6) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Degree 5: x^5 - 32 = 0 ===" << std::endl;
    std::cout << "Expected: fifth roots of 32" << std::endl;
    roots = sm::polysolve::solve<double, 5>(sm::vec<double, 6>{-32, 0, 0, 0, 0, 1});
    print_roots(roots);
    if (roots.size() != 5) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Degree 7: (x-1)(x-2)...(x-7) = 0 ===" << std::endl;
    std::cout << "Expected: x = 1, 2, 3, 4, 5, 6, 7" << std::endl;
    sm::vec<double, 8> coeffs7 = {5040, -13068, 13132, -6769, 1960, -322, 28, 1};
    sm::vvec<std::complex<double>> roots7 = sm::polysolve::solve<double, 7>(coeffs7);
    print_roots(roots7);
    if (roots7.size() != 7) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
}

void test_template_types(int& rtn) {
    std::cout << "\n=== Template Type: x^2 - 5x + 6 = 0 (float) ===" << std::endl;
    std::cout << "Expected: x = 2, 3" << std::endl;
    sm::vvec<std::complex<float>> rootsFloat = sm::polysolve::solve<float, 2>(sm::vec<float, 3>{6.0f, -5.0f, 1.0f});
    std::cout << std::fixed << std::setprecision(6);
    for (size_t i = 0; i < rootsFloat.size(); ++i) {
        std::cout << "  x" << i << " = " << rootsFloat[i].real();
        if (std::abs(rootsFloat[i].imag()) > 1e-6f) {
            std::cout << (rootsFloat[i].imag() >= 0 ? " + " : " - ") 
                     << std::abs(rootsFloat[i].imag()) << "i";
        }
        std::cout << " (float)" << std::endl;
    }
    if (rootsFloat.size() != 2) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Template Type: x^3 - 6x^2 + 11x - 6 = 0 (long double) ===" << std::endl;
    std::cout << "Expected: x = 1, 2, 3" << std::endl;
    sm::vvec<std::complex<long double>> rootsLongDouble = sm::polysolve::solve<long double, 3>(sm::vec<long double, 4>{-6.0L, 11.0L, -6.0L, 1.0L});
    std::cout << std::fixed << std::setprecision(10);
    for (size_t i = 0; i < rootsLongDouble.size(); ++i) {
        std::cout << "  x" << i << " = " << rootsLongDouble[i].real();
        if (std::abs(rootsLongDouble[i].imag()) > 1e-10L) {
            std::cout << (rootsLongDouble[i].imag() >= 0 ? " + " : " - ") 
                     << std::abs(rootsLongDouble[i].imag()) << "i";
        }
        std::cout << " (long double)" << std::endl;
    }
    if (rootsLongDouble.size() != 3) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Template Type: x^2 + 1 = 0 (float, complex roots) ===" << std::endl;
    std::cout << "Expected: x = +/-i" << std::endl;
    rootsFloat = sm::polysolve::solve<float, 2>(sm::vec<float, 3>{1.0f, 0.0f, 1.0f});
    std::cout << std::fixed << std::setprecision(6);
    for (size_t i = 0; i < rootsFloat.size(); ++i) {
        std::cout << "  x" << i << " = " << rootsFloat[i].real();
        if (std::abs(rootsFloat[i].imag()) > 1e-6f) {
            std::cout << (rootsFloat[i].imag() >= 0 ? " + " : " - ") 
                     << std::abs(rootsFloat[i].imag()) << "i";
        }
        std::cout << " (float)" << std::endl;
    }
    if (rootsFloat.size() != 2) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Template Type: x^4 - 10x^2 + 9 = 0 (float, real roots) ===" << std::endl;
    std::cout << "Expected: x = +/-1, +/-3" << std::endl;
    sm::vvec<float> realRootsFloat = sm::polysolve::real<float, 4>(sm::vec<float, 5>{9.0f, 0.0f, -10.0f, 0.0f, 1.0f});
    std::cout << std::fixed << std::setprecision(6);
    for (size_t i = 0; i < realRootsFloat.size(); ++i) {
        std::cout << "  x" << i << " = " << realRootsFloat[i] << " (float)" << std::endl;
    }
    if (realRootsFloat.size() != 4) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
}

int main() {
    int rtn = 0;
    std::cout << "Polynomial Solver Extended Test Suite" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "Testing analytical solutions (degrees 1-4) and" << std::endl;
    std::cout << "numerical Durand-Kerner method (degree > 4)" << std::endl;
    
    try {
        test_linear(rtn);
        test_quadratic(rtn);
        test_cubic(rtn);
        test_quartic(rtn);
        test_real_roots(rtn);
        test_special_cases(rtn);
        test_mixed_roots(rtn);
        test_higher_degree(rtn);
        test_template_types(rtn);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        --rtn;
    }
    
    std::cout << (rtn == 0 ? "\nAll tests passed :) \n" : "\nSome tests failed\n");
    return rtn;
}
