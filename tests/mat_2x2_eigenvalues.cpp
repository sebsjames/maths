#include <iostream>
#include <iomanip>
#include <cmath>
#include <sm/mat22>

int main()
{
    int rtn = 0;

    // Test 1: Diagonal matrix eigenvalues
    std::cout << "Test 1: Diagonal matrix eigenvalues" << std::endl;
    sm::mat22<float> diag = {3.0f, 0.0f, 0.0f, 7.0f};
    sm::vec<std::complex<float>, 2> lambdas_diag = diag.eigenvalues();
    std::cout << "  Matrix: diag(3, 7)" << std::endl;
    std::cout << "  Eigenvalues: " << lambdas_diag[0].real() << ", "
              << lambdas_diag[1].real() << std::endl;

    // Check if eigenvalues are correct (order may vary)
    bool has_3 = false, has_7 = false;
    for (int i = 0; i < 2; ++i) {
        float val = lambdas_diag[i].real();
        if (std::abs(val - 3.0f) < 1e-5f) has_3 = true;
        if (std::abs(val - 7.0f) < 1e-5f) has_7 = true;
    }
    if (has_3 && has_7) {
        std::cout << "  Diagonal eigenvalues correct\n" << std::endl;
    } else {
        std::cout << "  Diagonal eigenvalues FAILED\n" << std::endl;
        --rtn;
    }

    // Test 2: Identity matrix eigenvalues
    std::cout << "Test 2: Identity matrix eigenvalues" << std::endl;
    sm::mat22<float> identity;
    identity.setToIdentity();
    sm::vec<std::complex<float>, 2> lambdas_id = identity.eigenvalues();
    std::cout << "  Matrix: Identity" << std::endl;
    std::cout << "  Eigenvalues: " << lambdas_id[0].real() << ", "
              << lambdas_id[1].real() << std::endl;

    bool all_ones = true;
    for (int i = 0; i < 2; ++i) {
        if (std::abs(lambdas_id[i].real() - 1.0f) > 1e-5f) {
            all_ones = false;
        }
    }
    if (all_ones) {
        std::cout << "   Identity eigenvalues correct (all 1.0)\n" << std::endl;
    } else {
        std::cout << "  Identity eigenvalues FAILED\n" << std::endl;
        --rtn;
    }

    // Test 3: Symmetric matrix eigenvalues (should be real)
    std::cout << "Test 3: Symmetric matrix eigenvalues" << std::endl;
    sm::mat22<float> sym = {4.0f, 1.0f, 1.0f, 4.0f};
    sm::vec<std::complex<float>, 2> lambdas_sym = sym.eigenvalues();
    std::cout << "  Matrix: Symmetric [[4,1],[1,4]]" << std::endl;
    std::cout << "  Eigenvalues: " << lambdas_sym[0].real() << ", "
              << lambdas_sym[1].real() << std::endl;

    // Check imaginary parts are near zero (real eigenvalues)
    bool all_real = true;
    for (int i = 0; i < 2; ++i) {
        if (std::abs(lambdas_sym[i].imag()) > 1e-5f) {
            all_real = false;
        }
    }
    // Expected eigenvalues: 5, 3
    bool has_5 = false, has_3_sym = false;
    for (int i = 0; i < 2; ++i) {
        float val = lambdas_sym[i].real();
        if (std::abs(val - 5.0f) < 1e-4f) has_5 = true;
        if (std::abs(val - 3.0f) < 1e-4f) has_3_sym = true;
    }
    if (all_real && has_5 && has_3_sym) {
        std::cout << "  Symmetric matrix has correct real eigenvalues (5, 3)\n" << std::endl;
    } else {
        std::cout << "  Symmetric matrix eigenvalues FAILED\n" << std::endl;
        --rtn;
    }

    // Test 4: Rotation matrix (complex eigenvalues)
    std::cout << "Test 4: Rotation matrix (complex eigenvalues)" << std::endl;
    sm::mat22<float> rot;
    rot.rotate(sm::mathconst<float>::pi_over_4);  // 45 degree rotation
    sm::vec<std::complex<float>, 2> lambdas_rot = rot.eigenvalues();
    std::cout << "  Matrix: 45deg rotation" << std::endl;
    std::cout << "  Eigenvalues: " << lambdas_rot[0].real() << " + "
              << lambdas_rot[0].imag() << "i, "
              << lambdas_rot[1].real() << " + " << lambdas_rot[1].imag() << "i" << std::endl;

    // Expected: cos(45) +/- i * sin(45) ~ 0.707 +/- 0.707i
    float cos45 = std::cos(sm::mathconst<float>::pi_over_4);
    float sin45 = std::sin(sm::mathconst<float>::pi_over_4);
    bool complex_correct = false;
    if (std::abs(lambdas_rot[0].real() - cos45) < 1e-5f &&
        std::abs(std::abs(lambdas_rot[0].imag()) - sin45) < 1e-5f &&
        std::abs(lambdas_rot[1].real() - cos45) < 1e-5f &&
        std::abs(std::abs(lambdas_rot[1].imag()) - sin45) < 1e-5f) {
        complex_correct = true;
    }
    if (complex_correct) {
        std::cout << "  Rotation matrix has correct complex eigenvalues\n" << std::endl;
    } else {
        std::cout << "  Rotation matrix eigenvalues FAILED\n" << std::endl;
        --rtn;
    }

    // Test 5: eigenpairs and verification (Av = lambda v)
    std::cout << "Test 5: eigenpair verification (Av = lambda v)" << std::endl;
    sm::mat22<float> A = {1.0f, 2.0f, 2.0f, 1.0f};
    sm::vec<sm::mat22<float>::eigenpair, 2> pairs = A.eigenpairs();
    std::cout << "  Matrix: [[1,2],[2,1]]" << std::endl;

    bool all_verified = true;
    for (int i = 0; i < 2; ++i) {
        std::complex<float> lambda = pairs[i].eigenvalue;
        sm::vec<std::complex<float>, 2> v = pairs[i].eigenvector;

        // Compute Av (column-major: [0,1, 2,3])
        std::complex<float> Av0 = A[0] * v[0] + A[2] * v[1];
        std::complex<float> Av1 = A[1] * v[0] + A[3] * v[1];

        // Compute lambdav
        std::complex<float> lv0 = lambda * v[0];
        std::complex<float> lv1 = lambda * v[1];

        // Compute ||Av - lambdav||
        float error = std::sqrt(std::norm(Av0 - lv0) + std::norm(Av1 - lv1));

        std::cout << "  Pair " << i << ": lambda = " << lambda.real();
        if (std::abs(lambda.imag()) > 1e-5f) {
            std::cout << " + " << lambda.imag() << "i";
        }
        std::cout << ", error = " << error;
        if (error < 1e-4f) {
            std::cout << " " << std::endl;
        } else {
            std::cout << " x" << std::endl;
            all_verified = false;
            --rtn;
        }
    }
    if (all_verified) {
        std::cout << "  All eigenpairs verified\n" << std::endl;
    } else {
        std::cout << "  eigenpair verification FAILED\n" << std::endl;
    }

    // Test 6: Individual eigenvector computation
    std::cout << "Test 6: Individual eigenvector method" << std::endl;
    sm::mat22<float> B = {-2.0f, 1.0f, 1.0f, -2.0f};
    sm::vec<std::complex<float>, 2> lambdas_B = B.eigenvalues();
    sm::vec<std::complex<float>, 2> v_first = B.eigenvector(lambdas_B[0]);

    // Verify this eigenvector
    std::complex<float> Bv0 = B[0] * v_first[0] + B[2] * v_first[1];
    std::complex<float> Bv1 = B[1] * v_first[0] + B[3] * v_first[1];

    std::complex<float> lv0_B = lambdas_B[0] * v_first[0];
    std::complex<float> lv1_B = lambdas_B[0] * v_first[1];

    float error_B = std::sqrt(std::norm(Bv0 - lv0_B) + std::norm(Bv1 - lv1_B));
    std::cout << "  Matrix: [[-2,1],[1,-2]] (expected eigenvalues: -1, -3)" << std::endl;
    std::cout << "  eigenvector(lambda_0) verification error: " << error_B << std::endl;

    if (error_B < 1e-4f) {
        std::cout << "  Individual eigenvector method correct\n" << std::endl;
    } else {
        std::cout << "  Individual eigenvector method FAILED\n" << std::endl;
        --rtn;
    }

    // Test 7: Repeated eigenvalue (shear matrix)
    std::cout << "Test 7: Repeated eigenvalue case" << std::endl;
    sm::mat22<float> shear = {1.0f, 0.0f, 2.0f, 1.0f};
    sm::vec<std::complex<float>, 2> lambdas_shear = shear.eigenvalues();
    std::cout << "  Matrix: Shear [[1,2],[0,1]]" << std::endl;
    std::cout << "  Eigenvalues: " << lambdas_shear[0].real() << ", "
              << lambdas_shear[1].real() << std::endl;

    // Both eigenvalues should be 1.0 (repeated)
    bool both_one = (std::abs(lambdas_shear[0].real() - 1.0f) < 1e-5f &&
                     std::abs(lambdas_shear[1].real() - 1.0f) < 1e-5f);
    if (both_one) {
        std::cout << "  Repeated eigenvalue detected (both = 1.0)\n" << std::endl;
    } else {
        std::cout << "  Repeated eigenvalue test FAILED\n" << std::endl;
        --rtn;
    }

    // Test 8: General matrix with negative eigenvalues
    std::cout << "Test 8: Matrix with negative eigenvalues" << std::endl;
    sm::mat22<float> neg = {0.0f, -1.0f, 1.0f, 0.0f};
    sm::vec<std::complex<float>, 2> lambdas_neg = neg.eigenvalues();
    std::cout << "  Matrix: [[0,-1],[1,0]] (90deg rotation)" << std::endl;
    std::cout << "  Eigenvalues: " << lambdas_neg[0].real() << " + "
              << lambdas_neg[0].imag() << "i, "
              << lambdas_neg[1].real() << " + " << lambdas_neg[1].imag() << "i" << std::endl;

    // Expected: +/-i (purely imaginary)
    bool purely_imaginary = (std::abs(lambdas_neg[0].real()) < 1e-5f &&
                            std::abs(lambdas_neg[1].real()) < 1e-5f &&
                            std::abs(std::abs(lambdas_neg[0].imag()) - 1.0f) < 1e-5f &&
                            std::abs(std::abs(lambdas_neg[1].imag()) - 1.0f) < 1e-5f);
    if (purely_imaginary) {
        std::cout << "  Purely imaginary eigenvalues correct (+/-i)\n" << std::endl;
    } else {
        std::cout << "  Purely imaginary eigenvalues FAILED\n" << std::endl;
        --rtn;
    }

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");
    return rtn;
}
