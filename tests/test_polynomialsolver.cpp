#include <sm/polynomialsolver>
#include <iostream>
#include <iomanip>

void printRoots(const sm::vvec<std::complex<double>>& roots) {
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

void testLinear(int& rtn) {
    std::cout << "\n=== Linear: 2x - 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = 3" << std::endl;
    sm::vvec<std::complex<double>> roots = PolynomialSolver<double>::solve({-6, 2});
    printRoots(roots);
    if (roots.size() != 1 || std::abs(roots[0].real() - 3.0) > 1e-6) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Linear: -3x + 12 = 0 ===" << std::endl;
    std::cout << "Expected: x = 4" << std::endl;
    roots = PolynomialSolver<double>::solve({12, -3});
    printRoots(roots);
    if (roots.size() != 1 || std::abs(roots[0].real() - 4.0) > 1e-6) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Linear: 0.5x + 2.5 = 0 ===" << std::endl;
    std::cout << "Expected: x = -5" << std::endl;
    roots = PolynomialSolver<double>::solve({2.5, 0.5});
    printRoots(roots);
    if (roots.size() != 1 || std::abs(roots[0].real() + 5.0) > 1e-6) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
}

void testQuadratic(int& rtn) {
    std::cout << "\n=== Quadratic: x^2 - 5x + 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = 2, 3" << std::endl;
    sm::vvec<std::complex<double>> roots = PolynomialSolver<double>::solve({6, -5, 1});
    printRoots(roots);
    if (roots.size() != 2) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Quadratic (complex): x^2 + 1 = 0 ===" << std::endl;
    std::cout << "Expected: x = ±i" << std::endl;
    roots = PolynomialSolver<double>::solve({1, 0, 1});
    printRoots(roots);
    if (roots.size() != 2) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Quadratic: x^2 + 4x + 4 = 0 (repeated root) ===" << std::endl;
    std::cout << "Expected: x = -2, -2" << std::endl;
    roots = PolynomialSolver<double>::solve({4, 4, 1});
    printRoots(roots);
    
    std::cout << "\n=== Quadratic: 2x^2 - 8x + 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = 1, 3" << std::endl;
    roots = PolynomialSolver<double>::solve({6, -8, 2});
    printRoots(roots);
    
    std::cout << "\n=== Quadratic: x^2 + 2x + 5 = 0 (complex) ===" << std::endl;
    std::cout << "Expected: x = -1 ± 2i" << std::endl;
    roots = PolynomialSolver<double>::solve({5, 2, 1});
    printRoots(roots);
    
    std::cout << "\n=== Quadratic: x^2 - 2 = 0 ===" << std::endl;
    std::cout << "Expected: x = ±√2 ≈ ±1.414" << std::endl;
    roots = PolynomialSolver<double>::solve({-2, 0, 1});
    printRoots(roots);
    
    std::cout << "\n=== Quadratic: 3x^2 + 6x + 9 = 0 ===" << std::endl;
    std::cout << "Expected: complex conjugate roots" << std::endl;
    roots = PolynomialSolver<double>::solve({9, 6, 3});
    printRoots(roots);
}

void testCubic(int& rtn) {
    std::cout << "\n=== Cubic: x^3 - 6x^2 + 11x - 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = 1, 2, 3" << std::endl;
    sm::vvec<std::complex<double>> roots = PolynomialSolver<double>::solve({-6, 11, -6, 1});
    printRoots(roots);
    if (roots.size() != 3) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Cubic: x^3 - 1 = 0 (cube roots of unity) ===" << std::endl;
    std::cout << "Expected: x = 1, -0.5±0.866i" << std::endl;
    roots = PolynomialSolver<double>::solve({-1, 0, 0, 1});
    printRoots(roots);
    
    std::cout << "\n=== Cubic: x^3 + 8 = 0 ===" << std::endl;
    std::cout << "Expected: x = -2, 1±√3i" << std::endl;
    roots = PolynomialSolver<double>::solve({8, 0, 0, 1});
    printRoots(roots);
    
    std::cout << "\n=== Cubic: x^3 - 3x^2 + 3x - 1 = 0 (repeated root) ===" << std::endl;
    std::cout << "Expected: x = 1, 1, 1 (triple root)" << std::endl;
    roots = PolynomialSolver<double>::solve({-1, 3, -3, 1});
    printRoots(roots);
    
    std::cout << "\n=== Cubic: x^3 + 3x^2 + 3x + 1 = 0 ===" << std::endl;
    std::cout << "Expected: x = -1, -1, -1" << std::endl;
    roots = PolynomialSolver<double>::solve({1, 3, 3, 1});
    printRoots(roots);
    
    std::cout << "\n=== Cubic: 2x^3 - 4x^2 - 22x + 24 = 0 ===" << std::endl;
    std::cout << "Expected: x = -3, 1, 4" << std::endl;
    roots = PolynomialSolver<double>::solve({24, -22, -4, 2});
    printRoots(roots);
    
    std::cout << "\n=== Cubic: x^3 - 7x - 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = -1, -2, 3" << std::endl;
    roots = PolynomialSolver<double>::solve({-6, -7, 0, 1});
    printRoots(roots);
    
    std::cout << "\n=== Cubic: x^3 - 15x - 4 = 0 ===" << std::endl;
    std::cout << "Expected: three real roots" << std::endl;
    roots = PolynomialSolver<double>::solve({-4, -15, 0, 1});
    printRoots(roots);
}

void testQuartic(int& rtn) {
    std::cout << "\n=== Quartic: x^4 - 10x^2 + 9 = 0 (biquadratic) ===" << std::endl;
    std::cout << "Expected: x = ±1, ±3" << std::endl;
    sm::vvec<std::complex<double>> roots = PolynomialSolver<double>::solve({9, 0, -10, 0, 1});
    printRoots(roots);
    if (roots.size() != 4) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Quartic: (x-1)(x-2)(x-3)(x-4) = x^4 - 10x^3 + 35x^2 - 50x + 24 = 0 ===" << std::endl;
    std::cout << "Expected: x = 1, 2, 3, 4" << std::endl;
    roots = PolynomialSolver<double>::solve({24, -50, 35, -10, 1});
    printRoots(roots);
    
    std::cout << "\n=== Quartic: x^4 - 1 = 0 (fourth roots of unity) ===" << std::endl;
    std::cout << "Expected: x = ±1, ±i" << std::endl;
    roots = PolynomialSolver<double>::solve({-1, 0, 0, 0, 1});
    printRoots(roots);
    
    std::cout << "\n=== Quartic: x^4 + 4x^2 + 4 = 0 ===" << std::endl;
    std::cout << "Expected: complex roots" << std::endl;
    roots = PolynomialSolver<double>::solve({4, 0, 4, 0, 1});
    printRoots(roots);
    
    std::cout << "\n=== Quartic: x^4 - 5x^2 + 4 = 0 ===" << std::endl;
    std::cout << "Expected: x = ±1, ±2" << std::endl;
    roots = PolynomialSolver<double>::solve({4, 0, -5, 0, 1});
    printRoots(roots);
    
    std::cout << "\n=== Quartic: x^4 + x^3 - 7x^2 - x + 6 = 0 ===" << std::endl;
    std::cout << "Expected: x = -3, -1, 1, 2" << std::endl;
    roots = PolynomialSolver<double>::solve({6, -1, -7, 1, 1});
    printRoots(roots);
    
    std::cout << "\n=== Quartic: 2x^4 - 8x^3 + 8x^2 - 8x + 6 = 0 ===" << std::endl;
    std::cout << "Expected: two real and two complex roots" << std::endl;
    roots = PolynomialSolver<double>::solve({6, -8, 8, -8, 2});
    printRoots(roots);
}

void testRealRoots(int& rtn) {
    std::cout << "\n=== Real Roots Filter Test 1 ===" << std::endl;
    sm::vvec<double> realRoots = PolynomialSolver<double>::solveReal({-6, 11, -6, 1});
    std::cout << "Cubic x^3 - 6x^2 + 11x - 6 = 0 real roots:" << std::endl;
    for (size_t i = 0; i < realRoots.size(); ++i) {
        std::cout << "  x" << i << " = " << realRoots[i] << std::endl;
    }
    if (realRoots.size() != 3) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Real Roots Filter Test 2 ===" << std::endl;
    realRoots = PolynomialSolver<double>::solveReal({-1, 0, 0, 1});
    std::cout << "Cubic x^3 - 1 = 0 real roots (should be 1 root):" << std::endl;
    for (size_t i = 0; i < realRoots.size(); ++i) {
        std::cout << "  x" << i << " = " << realRoots[i] << std::endl;
    }
    if (realRoots.size() != 1) {
        std::cout << "FAILED: Expected 1 real root, got " << realRoots.size() << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Real Roots Filter Test 3 ===" << std::endl;
    realRoots = PolynomialSolver<double>::solveReal({1, 0, 1});
    std::cout << "Quadratic x^2 + 1 = 0 real roots (should be 0 roots):" << std::endl;
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

void testSpecialCases(int& rtn) {
    std::cout << "\n=== Special Case: Large Coefficients ===" << std::endl;
    std::cout << "Quadratic: 100x^2 - 500x + 600 = 0" << std::endl;
    sm::vvec<std::complex<double>> roots = PolynomialSolver<double>::solve({600, -500, 100});
    printRoots(roots);
    if (roots.size() != 2) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Special Case: Small Coefficients ===" << std::endl;
    std::cout << "Quadratic: 0.001x^2 - 0.003x + 0.002 = 0" << std::endl;
    roots = PolynomialSolver<double>::solve({0.002, -0.003, 0.001});
    printRoots(roots);
    
    std::cout << "\n=== Special Case: Negative Leading Coefficient ===" << std::endl;
    std::cout << "Cubic: -x^3 + 6x^2 - 11x + 6 = 0" << std::endl;
    roots = PolynomialSolver<double>::solve({6, -11, 6, -1});
    printRoots(roots);
    
    std::cout << "\n=== Special Case: Zero Coefficient Terms ===" << std::endl;
    std::cout << "Quartic: x^4 + 0x^3 + 0x^2 + 0x - 16 = 0" << std::endl;
    roots = PolynomialSolver<double>::solve({-16, 0, 0, 0, 1});
    printRoots(roots);
}

void testMixedRoots(int& rtn) {
    std::cout << "\n=== Mixed: Positive and Negative Roots ===" << std::endl;
    std::cout << "Cubic: (x+5)(x-3)(x-7) = x^3 - 5x^2 - 29x + 105 = 0" << std::endl;
    sm::vvec<std::complex<double>> roots = PolynomialSolver<double>::solve({105, -29, -5, 1});
    printRoots(roots);
    if (roots.size() != 3) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Mixed: Fractional Roots ===" << std::endl;
    std::cout << "Cubic: (x-0.5)(x-1.5)(x-2.5) = x^3 - 4.5x^2 + 6.25x - 1.875 = 0" << std::endl;
    roots = PolynomialSolver<double>::solve({-1.875, 6.25, -4.5, 1});
    printRoots(roots);
    
    std::cout << "\n=== Mixed: Complex and Real ===" << std::endl;
    std::cout << "Quartic: (x-1)(x-2)(x^2+1) = x^4 - 3x^3 + 3x^2 - 3x + 2 = 0" << std::endl;
    roots = PolynomialSolver<double>::solve({2, -3, 3, -3, 1});
    printRoots(roots);
}

void testHigherDegree(int& rtn) {
    std::cout << "\n=== Degree 5: (x-1)(x-2)(x-3)(x-4)(x-5) ===" << std::endl;
    sm::vvec<std::complex<double>> roots = PolynomialSolver<double>::solve({-120, 274, -225, 85, -15, 1});
    std::cout << "Expected: x = 1, 2, 3, 4, 5" << std::endl;
    printRoots(roots);
    if (roots.size() != 5) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Degree 6: (x+1)(x-1)(x+2)(x-2)(x+3)(x-3) ===" << std::endl;
    sm::vvec<std::complex<double>> roots6 = PolynomialSolver<double>::solve({-36, 0, 49, 0, -14, 0, 1});
    std::cout << "Expected: x = ±1, ±2, ±3" << std::endl;
    printRoots(roots6);
    if (roots6.size() != 6) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Degree 5: x^5 - 32 = 0 ===" << std::endl;
    roots = PolynomialSolver<double>::solve({-32, 0, 0, 0, 0, 1});
    std::cout << "Expected: fifth roots of 32" << std::endl;
    printRoots(roots);
    if (roots.size() != 5) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
    
    std::cout << "\n=== Degree 7: (x-1)(x-2)...(x-7) ===" << std::endl;
    sm::vvec<double> coeffs7 = {5040, -13068, 13132, -6769, 1960, -322, 28, 1};
    sm::vvec<std::complex<double>> roots7 = PolynomialSolver<double>::solve(coeffs7);
    std::cout << "Expected: x = 1, 2, 3, 4, 5, 6, 7" << std::endl;
    printRoots(roots7);
    if (roots7.size() != 7) {
        std::cout << "FAILED" << std::endl;
        --rtn;
    }
}

void testTemplateTypes(int& rtn) {
    std::cout << "\n=== Template Type Test: float ==" << std::endl;
    std::cout << "Quadratic: x^2 - 5x + 6 = 0" << std::endl;
    sm::vvec<std::complex<float>> rootsFloat = PolynomialSolver<float>::solve({6.0f, -5.0f, 1.0f});
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
    
    std::cout << "\n=== Template Type Test: long double ==" << std::endl;
    std::cout << "Cubic: x^3 - 6x^2 + 11x - 6 = 0" << std::endl;
    sm::vvec<std::complex<long double>> rootsLongDouble = PolynomialSolver<long double>::solve({-6.0L, 11.0L, -6.0L, 1.0L});
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
    
    std::cout << "\n=== Template Type Test: float with complex roots ==" << std::endl;
    std::cout << "Quadratic: x^2 + 1 = 0" << std::endl;
    rootsFloat = PolynomialSolver<float>::solve({1.0f, 0.0f, 1.0f});
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
    
    std::cout << "\n=== Template Type Test: solveReal with float ==" << std::endl;
    std::cout << "Quartic: x^4 - 10x^2 + 9 = 0 (real roots only)" << std::endl;
    sm::vvec<float> realRootsFloat = PolynomialSolver<float>::solveReal({9.0f, 0.0f, -10.0f, 0.0f, 1.0f});
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
        testLinear(rtn);
        testQuadratic(rtn);
        testCubic(rtn);
        testQuartic(rtn);
        testRealRoots(rtn);
        testSpecialCases(rtn);
        testMixedRoots(rtn);
        testHigherDegree(rtn);
        testTemplateTypes(rtn);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        --rtn;
    }
    
    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");
    return rtn;
}
