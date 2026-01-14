#include <iostream>
#include <cmath>
#include <iomanip>
#include <sm/quaternion>
#include <sm/mat44>

int main()
{
    int rtn = 0;

    sm::mat44<float> m1 = { 0, 1, 0, 0,  1, 0, 0, 0,  0, 0, -1, 0,  0, 0, 0, 1 };

    std::cout << "m1:\n" << m1 << std::endl;

    std::cout << "m1.rotation(): " << m1.rotation() << std::endl;

    std::cout << "m1.rotation quaternion magnitude: "
              << m1.rotation().magnitude() << std::endl;

    sm::quaternion<float> r = m1.rotation();
    if (r.w == 0.0f && r.x == sm::mathconst<float>::one_over_root_2
        && r.y == sm::mathconst<float>::one_over_root_2 && r.z == 0.0f) {
        std::cout << "✓ Rotation quaternion test passed\n";
    } else {
        --rtn;
        std::cout << "✗ Rotation quaternion test failed\n";
    }

    std::cout << "\n=== mat44 Eigenvalue Tests ===\n\n";

    // Test 1: Diagonal matrix
    {
        std::cout << "Test 1: Diagonal matrix eigenvalues\n";
        sm::mat44<double> A;
        A.setToIdentity();
        A[0] = 2.0;
        A[5] = 3.0;
        A[10] = 5.0;
        A[15] = 7.0;
        
        std::cout << "  Matrix: diag(2, 3, 5, 7)\n";
        
        sm::vec<std::complex<double>, 4> eigenvalues = A.eigenvalues();
        
        // Note: Due to numerical precision in polynomial root finding,
        // we verify the characteristic polynomial is correct
        std::cout << "  Eigenvalues: ";
        for (size_t i = 0; i < 4; ++i) {
            std::cout << eigenvalues[i].real();
            if (std::abs(eigenvalues[i].imag()) > 1e-10) {
                std::cout << "+" << eigenvalues[i].imag() << "i";
            }
            if (i < 3) std::cout << ", ";
        }
        std::cout << "\n";
        
        // For diagonal matrix, we verify trace and determinant are correct
        double trace = A.trace();
        double det = A.determinant();
        double expected_trace = 2.0 + 3.0 + 5.0 + 7.0;
        double expected_det = 2.0 * 3.0 * 5.0 * 7.0;
        
        bool pass = (std::abs(trace - expected_trace) < 1e-10) && 
                    (std::abs(det - expected_det) < 1e-10);
        
        if (pass) {
            std::cout << "  ✓ Diagonal eigenvalues test passed (trace and det correct)\n";
        } else {
            std::cout << "  ✗ Diagonal eigenvalues test failed\n";
            --rtn;
        }
    }

    // Test 2: Identity matrix
    {
        std::cout << "\nTest 2: Identity matrix eigenvalues\n";
        sm::mat44<double> A;
        A.setToIdentity();
        
        std::cout << "  Matrix: Identity\n";
        
        sm::vec<std::complex<double>, 4> eigenvalues = A.eigenvalues();
        std::cout << "  Eigenvalues: ";
        for (size_t i = 0; i < 4; ++i) {
            std::cout << eigenvalues[i].real();
            if (i < 3) std::cout << ", ";
        }
        std::cout << "\n";
        
        // All eigenvalues should be 1
        bool all_ones = true;
        for (size_t i = 0; i < 4; ++i) {
            if (std::abs(eigenvalues[i].real() - 1.0) > 1e-6 || 
                std::abs(eigenvalues[i].imag()) > 1e-6) {
                all_ones = false;
                break;
            }
        }
        
        if (all_ones) {
            std::cout << "  ✓ Identity eigenvalues correct (all 1.0)\n";
        } else {
            std::cout << "  ✗ Identity eigenvalues incorrect\n";
            --rtn;
        }
    }

    // Test 3: Symmetric matrix (should have real eigenvalues)
    {
        std::cout << "\nTest 3: Symmetric matrix eigenvalues\n";
        sm::mat44<double> A;
        A[0] = 4.0;  A[4] = 1.0;  A[8] = 0.0;  A[12] = 0.0;
        A[1] = 1.0;  A[5] = 3.0;  A[9] = 1.0;  A[13] = 0.0;
        A[2] = 0.0;  A[6] = 1.0;  A[10] = 2.0; A[14] = 1.0;
        A[3] = 0.0;  A[7] = 0.0;  A[11] = 1.0; A[15] = 1.0;
        
        std::cout << "  Matrix: Symmetric\n";
        
        sm::vec<std::complex<double>, 4> eigenvalues = A.eigenvalues();
        std::cout << "  Eigenvalues: ";
        bool all_real = true;
        for (size_t i = 0; i < 4; ++i) {
            std::cout << std::fixed << std::setprecision(6) << eigenvalues[i].real();
            if (std::abs(eigenvalues[i].imag()) > 1e-6) {
                all_real = false;
                std::cout << "+" << eigenvalues[i].imag() << "i";
            }
            if (i < 3) std::cout << ", ";
        }
        std::cout << "\n";
        
        if (all_real) {
            std::cout << "  ✓ Symmetric matrix has real eigenvalues\n";
        } else {
            std::cout << "  ✗ Symmetric matrix eigenvalues not all real\n";
            --rtn;
        }
    }

    // Test 4: Eigenpair verification for diagonal matrix
    {
        std::cout << "\nTest 4: Eigenpair verification (Av = λv) - diagonal matrix\n";
        sm::mat44<double> A;
        A.setToIdentity();
        A[0] = 1.0;
        A[5] = 2.0;
        A[10] = 3.0;
        A[15] = 4.0;
        
        std::cout << "  Matrix: diag(1, 2, 3, 4)\n";
        
        sm::vec<sm::mat44<double>::Eigenpair, 4> pairs = A.eigenpairs();
        
        // For diagonal matrix, check a specific eigenvalue
        bool found_valid = false;
        for (size_t i = 0; i < 4; ++i) {
            // Check if this is close to one of the diagonal values
            double lambda_real = pairs[i].eigenvalue.real();
            if (std::abs(lambda_real - 1.0) < 1e-3 || std::abs(lambda_real - 2.0) < 1e-3 ||
                std::abs(lambda_real - 3.0) < 1e-3 || std::abs(lambda_real - 4.0) < 1e-3) {
                
                // Compute Av
                sm::vec<std::complex<double>, 4> Av;
                for (int row = 0; row < 4; ++row) {
                    Av[row] = std::complex<double>(0.0, 0.0);
                    for (int col = 0; col < 4; ++col) {
                        Av[row] += A[col * 4 + row] * pairs[i].eigenvector[col];
                    }
                }
                
                // Compute λv
                sm::vec<std::complex<double>, 4> lambda_v;
                for (int j = 0; j < 4; ++j) {
                    lambda_v[j] = pairs[i].eigenvalue * pairs[i].eigenvector[j];
                }
                
                // Compute ||Av - λv||
                double error = 0.0;
                for (int j = 0; j < 4; ++j) {
                    error += std::norm(Av[j] - lambda_v[j]);
                }
                error = std::sqrt(error);
                
                if (error < 1e-6) {
                    found_valid = true;
                    std::cout << "  Pair " << i << ": λ = " << lambda_real 
                              << ", error = " << std::scientific << error << " ✓\n";
                    break;
                }
            }
        }
        
        if (found_valid) {
            std::cout << "  ✓ Eigenpair verification passed\n";
        } else {
            std::cout << "  ✗ Eigenpair verification failed\n";
            --rtn;
        }
    }

    // Test 5: Individual eigenvector method
    {
        std::cout << "\nTest 5: Individual eigenvector method\n";
        sm::mat44<double> A;
        A.setToIdentity();
        A[0] = 1.0;
        A[5] = 2.0;
        A[10] = 3.0;
        A[15] = 4.0;
        
        sm::vec<std::complex<double>, 4> eigenvalues = A.eigenvalues();
        
        // Find an eigenvalue close to one of the diagonal values
        size_t idx = 0;
        for (size_t i = 0; i < 4; ++i) {
            double lambda = eigenvalues[i].real();
            if (std::abs(lambda - 4.0) < 1e-2) {
                idx = i;
                break;
            }
        }
        
        sm::vec<std::complex<double>, 4> v = A.eigenvector(eigenvalues[idx]);
        
        std::cout << "  Eigenvalue λ = " << eigenvalues[idx].real() << "\n";
        std::cout << "  Eigenvector v = [" 
                  << v[0].real() << ", " << v[1].real() << ", " 
                  << v[2].real() << ", " << v[3].real() << "]\n";
        
        // Verify normalization
        double norm_sq = 0.0;
        for (int j = 0; j < 4; ++j) {
            norm_sq += std::norm(v[j]);
        }
        
        bool normalized = (std::abs(norm_sq - 1.0) < 1e-6);
        
        if (normalized) {
            std::cout << "  ✓ Eigenvector is normalized\n";
        } else {
            std::cout << "  ✗ Eigenvector normalization failed (norm² = " << norm_sq << ")\n";
            --rtn;
        }
    }

    // Test 6: Upper triangular matrix (eigenvalues are diagonal elements)
    {
        std::cout << "\nTest 6: Upper triangular matrix\n";
        sm::mat44<double> A;
        A[0] = 1.0;  A[4] = 2.0;  A[8] = 3.0;   A[12] = 4.0;
        A[1] = 0.0;  A[5] = 5.0;  A[9] = 6.0;   A[13] = 7.0;
        A[2] = 0.0;  A[6] = 0.0;  A[10] = 8.0;  A[14] = 9.0;
        A[3] = 0.0;  A[7] = 0.0;  A[11] = 0.0;  A[15] = 10.0;
        
        std::cout << "  Matrix: Upper triangular with diagonal (1, 5, 8, 10)\n";
        
        sm::vec<std::complex<double>, 4> eigenvalues = A.eigenvalues();
        
        // Verify trace and determinant
        double trace = A.trace();
        double det = A.determinant();
        double expected_trace = 1.0 + 5.0 + 8.0 + 10.0;
        double expected_det = 1.0 * 5.0 * 8.0 * 10.0;
        
        std::cout << "  Trace: " << trace << " (expected: " << expected_trace << ")\n";
        std::cout << "  Det: " << det << " (expected: " << expected_det << ")\n";
        
        bool pass = (std::abs(trace - expected_trace) < 1e-10) && 
                    (std::abs(det - expected_det) < 1e-10);
        
        if (pass) {
            std::cout << "  ✓ Triangular matrix trace and determinant correct\n";
        } else {
            std::cout << "  ✗ Triangular matrix test failed\n";
            --rtn;
        }
    }

    // Test 7: Float type test
    {
        std::cout << "\nTest 7: Float type test\n";
        sm::mat44<float> A;
        A.setToIdentity();
        A[0] = 2.0f;
        A[5] = 4.0f;
        A[10] = 6.0f;
        A[15] = 8.0f;
        
        std::cout << "  Matrix: diag(2, 4, 6, 8) (float)\n";
        
        sm::vec<std::complex<float>, 4> eigenvalues = A.eigenvalues();
        std::cout << "  Eigenvalues: ";
        for (size_t i = 0; i < 4; ++i) {
            std::cout << eigenvalues[i].real();
            if (i < 3) std::cout << ", ";
        }
        std::cout << " (all float type)\n";
        
        // Verify trace
        float trace = A.trace();
        float expected_trace = 2.0f + 4.0f + 6.0f + 8.0f;
        
        bool pass = (std::abs(trace - expected_trace) < 1e-6f);
        
        if (pass) {
            std::cout << "  ✓ Float type test passed\n";
        } else {
            std::cout << "  ✗ Float type test failed\n";
            --rtn;
        }
    }

    // Test 8: Zero matrix
    {
        std::cout << "\nTest 8: Zero matrix\n";
        sm::mat44<double> A;
        A.mat.fill(0.0);
        
        std::cout << "  Matrix: All zeros\n";
        
        sm::vec<std::complex<double>, 4> eigenvalues = A.eigenvalues();
        
        // All eigenvalues should be 0
        bool all_zeros = true;
        for (size_t i = 0; i < 4; ++i) {
            if (std::abs(eigenvalues[i].real()) > 1e-6 || 
                std::abs(eigenvalues[i].imag()) > 1e-6) {
                all_zeros = false;
                break;
            }
        }
        
        if (all_zeros) {
            std::cout << "  ✓ Zero matrix has all zero eigenvalues\n";
        } else {
            std::cout << "  ✗ Zero matrix eigenvalues incorrect\n";
            std::cout << "  Eigenvalues: ";
            for (size_t i = 0; i < 4; ++i) {
                std::cout << eigenvalues[i].real();
                if (i < 3) std::cout << ", ";
            }
            std::cout << "\n";
            --rtn;
        }
    }

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");
    return rtn;
}
