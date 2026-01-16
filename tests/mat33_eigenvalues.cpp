#include <iostream>
#include <iomanip>
#include <cmath>
#include <sm/mat33>

int main()
{
    int rtn = 0;

    // Test 1: Diagonal matrix eigenvalues
    std::cout << "Test 1: Diagonal matrix eigenvalues" << std::endl;
    sm::mat33<float> diag = {5.0f, 0.0f, 0.0f,  0.0f, -2.0f, 0.0f,  0.0f, 0.0f, 8.0f};
    sm::vec<std::complex<float>, 3> lambdas_diag = diag.eigenvalues();
    std::cout << "  Matrix: diag(5, -2, 8)" << std::endl;
    std::cout << "  Eigenvalues: " << lambdas_diag[0].real() << ", "
              << lambdas_diag[1].real() << ", " << lambdas_diag[2].real() << std::endl;

    // Check if eigenvalues are correct (order may vary)
    bool has_5 = false, has_neg2 = false, has_8 = false;
    for (int i = 0; i < 3; ++i) {
        float val = lambdas_diag[i].real();
        if (std::abs(val - 5.0f) < 1e-5f) has_5 = true;
        if (std::abs(val + 2.0f) < 1e-5f) has_neg2 = true;
        if (std::abs(val - 8.0f) < 1e-5f) has_8 = true;
    }
    if (has_5 && has_neg2 && has_8) {
        std::cout << "  Diagonal eigenvalues correct\n" << std::endl;
    } else {
        std::cout << "  Diagonal eigenvalues FAILED\n" << std::endl;
        --rtn;
    }

    // Test 2: Identity matrix eigenvalues
    std::cout << "Test 2: Identity matrix eigenvalues" << std::endl;
    sm::mat33<float> identity;
    identity.setToIdentity();
    sm::vec<std::complex<float>, 3> lambdas_id = identity.eigenvalues();
    std::cout << "  Matrix: Identity" << std::endl;
    std::cout << "  Eigenvalues: " << lambdas_id[0].real() << ", "
              << lambdas_id[1].real() << ", " << lambdas_id[2].real() << std::endl;

    bool all_ones = true;
    for (int i = 0; i < 3; ++i) {
        if (std::abs(lambdas_id[i].real() - 1.0f) > 1e-5f) {
            all_ones = false;
        }
    }
    if (all_ones) {
        std::cout << "  Identity eigenvalues correct (all 1.0)\n" << std::endl;
    } else {
        std::cout << "  Identity eigenvalues FAILED\n" << std::endl;
        --rtn;
    }

    // Test 3: Symmetric matrix eigenvalues (should be real)
    std::cout << "Test 3: Symmetric matrix eigenvalues" << std::endl;
    sm::mat33<float> sym = {2.0f, 1.0f, 0.0f,  1.0f, 3.0f, 1.0f,  0.0f, 1.0f, 2.0f};
    sm::vec<std::complex<float>, 3> lambdas_sym = sym.eigenvalues();
    std::cout << "  Matrix: Symmetric" << std::endl;
    std::cout << "  Eigenvalues: " << lambdas_sym[0].real() << ", "
              << lambdas_sym[1].real() << ", " << lambdas_sym[2].real() << std::endl;

    // Check imaginary parts are near zero (real eigenvalues)
    bool all_real = true;
    for (int i = 0; i < 3; ++i) {
        if (std::abs(lambdas_sym[i].imag()) > 1e-5f) {
            all_real = false;
        }
    }
    if (all_real) {
        std::cout << "  Symmetric matrix has real eigenvalues\n" << std::endl;
    } else {
        std::cout << "  Symmetric matrix eigenvalues FAILED\n" << std::endl;
        --rtn;
    }

    // Test 4: Eigenpairs and verification (Av = lambdav)
    std::cout << "Test 4: Eigenpair verification (Av = lambdav)" << std::endl;
    sm::mat33<float> A = {6.0f, -1.0f, 0.0f,  -1.0f, 5.0f, -1.0f,  0.0f, -1.0f, 4.0f};
    sm::vec<sm::mat33<float>::Eigenpair, 3> pairs = A.eigenpairs();
    std::cout << "  Matrix: Tridiagonal" << std::endl;

    bool all_verified = true;
    for (int i = 0; i < 3; ++i) {
        std::complex<float> lambda = pairs[i].eigenvalue;
        sm::vec<std::complex<float>, 3> v = pairs[i].eigenvector;

        // Compute Av (column-major: [0,1,2, 3,4,5, 6,7,8])
        std::complex<float> Av0 = A[0] * v[0] + A[3] * v[1] + A[6] * v[2];
        std::complex<float> Av1 = A[1] * v[0] + A[4] * v[1] + A[7] * v[2];
        std::complex<float> Av2 = A[2] * v[0] + A[5] * v[1] + A[8] * v[2];

        // Compute lambdav
        std::complex<float> lv0 = lambda * v[0];
        std::complex<float> lv1 = lambda * v[1];
        std::complex<float> lv2 = lambda * v[2];

        // Compute ||Av - lambdav||
        float error = std::sqrt(std::norm(Av0 - lv0) + std::norm(Av1 - lv1) + std::norm(Av2 - lv2));

        std::cout << "  Pair " << i << ": lambda = " << lambda.real() << ", error = " << error;
        if (error < 1e-4f) {
            std::cout << " Good" << std::endl;
        } else {
            std::cout << " Bad" << std::endl;
            all_verified = false;
            --rtn;
        }
    }
    if (all_verified) {
        std::cout << "  All eigenpairs verified\n" << std::endl;
    } else {
        std::cout << "  Eigenpair verification FAILED\n" << std::endl;
    }

    // Test 5: Individual eigenvector computation
    std::cout << "Test 5: Individual eigenvector method" << std::endl;
    sm::mat33<float> B = {4.0f, 1.0f, 0.0f,  1.0f, 3.0f, 1.0f,  0.0f, 1.0f, 2.0f};
    sm::vec<std::complex<float>, 3> lambdas_B = B.eigenvalues();
    sm::vec<std::complex<float>, 3> v_first = B.eigenvector(lambdas_B[0]);

    // Verify this eigenvector
    std::complex<float> Bv0 = B[0] * v_first[0] + B[3] * v_first[1] + B[6] * v_first[2];
    std::complex<float> Bv1 = B[1] * v_first[0] + B[4] * v_first[1] + B[7] * v_first[2];
    std::complex<float> Bv2 = B[2] * v_first[0] + B[5] * v_first[1] + B[8] * v_first[2];

    std::complex<float> lv0_B = lambdas_B[0] * v_first[0];
    std::complex<float> lv1_B = lambdas_B[0] * v_first[1];
    std::complex<float> lv2_B = lambdas_B[0] * v_first[2];

    float error_B = std::sqrt(std::norm(Bv0 - lv0_B) + std::norm(Bv1 - lv1_B) + std::norm(Bv2 - lv2_B));
    std::cout << "  eigenvector(lambda_0) verification error: " << error_B << std::endl;

    if (error_B < 1e-4f) {
        std::cout << "  Individual eigenvector method correct\n" << std::endl;
    } else {
        std::cout << "  Individual eigenvector method FAILED\n" << std::endl;
        --rtn;
    }

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");
    return rtn;
}
