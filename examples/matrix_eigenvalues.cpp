#include <iostream>
#include <iomanip>

#include <sm/mat>

int main()
{
    constexpr double my_epsilon = 1e-10;

    std::cout << "Matrix Eigenvalue API Demonstration\n";
    std::cout << "====================================\n\n";

    // Example 1: Simple 2x2 matrix
    std::cout << "Example 1: 2x2 General Matrix\n";
    std::cout << "------------------------------\n";
    sm::mat22<double> A = {1.0, 2.0, 2.0, 1.0};
    std::cout << "Matrix A =\n" << A.str() << std::endl;

    // Get Eigenvalues
    sm::vec<std::complex<double>, 2> lambdas = A.eigenvalues();
    std::cout << "A.eigenvalues() returns:\n";
    std::cout << "  lambda_0 = " << lambdas[0].real();
    if (std::abs (lambdas[0].imag()) > my_epsilon) {
        std::cout << " + " << lambdas[0].imag() << "i";
    }
    std::cout << "\n";
    std::cout << "  lambda_1 = " << lambdas[1].real();
    if (std::abs (lambdas[1].imag()) > my_epsilon) {
        std::cout << " + " << lambdas[1].imag() << "i";
    }
    std::cout << "\n\n";

    // Get Eigenvector for first Eigenvalue
    sm::vec<std::complex<double>, 2> v0 = A.eigenvector (lambdas[0]);
    std::cout << "A.eigenvector(lambda_0) returns:\n";
    std::cout << "  v = [" << v0[0].real();
    if (std::abs (v0[0].imag()) > my_epsilon) {
        std::cout << " + " << v0[0].imag() << "i";
    }
    std::cout << ", " << v0[1].real();
    if (std::abs(v0[1].imag()) > my_epsilon) {
        std::cout << " + " << v0[1].imag() << "i";
    }
    std::cout << "]\n\n";

    // Get all Eigenpairs at once
    sm::vec<sm::mat22<double>::eigenpair, 2> pairs = A.eigenpairs();
    std::cout << "A.eigenpairs() returns both at once:\n";
    for (size_t i = 0; i < 2; ++i) {
        std::cout << "  Pair " << i << ": lambda = " << pairs[i].eigenvalue.real();
        if (std::abs (pairs[i].eigenvalue.imag()) > my_epsilon) {
            std::cout << " + " << pairs[i].eigenvalue.imag() << "i";
        }
        std::cout << ", v = [" << pairs[i].eigenvector[0].real();
        if (std::abs (pairs[i].eigenvector[0].imag()) > my_epsilon) {
            std::cout << " + " << pairs[i].eigenvector[0].imag() << "i";
        }
        std::cout << ", " << pairs[i].eigenvector[1].real();
        if (std::abs (pairs[i].eigenvector[1].imag()) > my_epsilon) {
            std::cout << " + " << pairs[i].eigenvector[1].imag() << "i";
        }
        std::cout << "]\n";
    }

    // Example 2: Rotation matrix (complex Eigenvalues)
    std::cout << "\n\nExample 2: 2x2 Rotation Matrix\n";
    std::cout << "-------------------------------\n";
    sm::mat22<double> R;
    R.rotate (sm::mathconst<double>::pi_over_6);  // 30 degree rotation
    std::cout << "Rotation matrix (30deg) =\n" << R.str() << std::endl;

    sm::vec<std::complex<double>, 2> rot_lambdas = R.eigenvalues();
    std::cout << "R.eigenvalues() returns complex conjugates:\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  lambda_0 = " << rot_lambdas[0].real();
    if (rot_lambdas[0].imag() >= 0) {
        std::cout << " + " << rot_lambdas[0].imag() << "i";
    } else {
        std::cout << " - " << -rot_lambdas[0].imag() << "i";
    }
    std::cout << "\n";
    std::cout << "  lambda_1 = " << rot_lambdas[1].real();
    if (rot_lambdas[1].imag() >= 0) {
        std::cout << " + " << rot_lambdas[1].imag() << "i";
    } else {
        std::cout << " - " << -rot_lambdas[1].imag() << "i";
    }
    std::cout << "\n";

    // Example 3: 3x3 symmetric matrix
    std::cout << "\n\nExample 3: 3x3 Symmetric Matrix\n";
    std::cout << "--------------------------------\n";
    sm::mat33<double> S = {4.0, 1.0, 0.0,  1.0, 3.0, 1.0,  0.0, 1.0, 2.0};
    std::cout << "Symmetric matrix S =\n" << S.str() << std::endl;

    sm::vec<std::complex<double>, 3> sym_lambdas = S.eigenvalues();
    std::cout << "S.eigenvalues() returns (real for symmetric):\n";
    for (size_t i = 0; i < 3; ++i) {
        std::cout << "  lambda" << i << " = " << sym_lambdas[i].real() << "\n";
    }

    // Example 4: Using single precision floating point
    std::cout << "\n\nExample 4: Using Single Precision\n";
    std::cout << "---------------------------------\n";
    sm::mat22<float> F = {2.0f, 1.0f, 1.0f, 2.0f};
    std::cout << "Matrix F (float) =\n" << F.str() << std::endl;

    sm::vec<std::complex<float>, 2> float_lambdas = F.eigenvalues();
    std::cout << "F.eigenvalues() with float precision:\n";
    std::cout << "  lambda_0 = " << float_lambdas[0].real() << "\n";
    std::cout << "  lambda_1 = " << float_lambdas[1].real() << "\n";

    // Example 5: Identity matrix
    std::cout << "\n\nExample 5: Identity Matrix\n";
    std::cout << "--------------------------\n";
    sm::mat33<double> I; // is identity on construction
    std::cout << "Identity matrix I =\n" << I.str() << std::endl;

    sm::vec<std::complex<double>, 3> id_lambdas = I.eigenvalues();
    std::cout << "I.eigenvalues() returns:\n";
    for (size_t i = 0; i < 3; ++i) {
        std::cout << "  lambda " << i << " = " << id_lambdas[i].real() << "\n";
    }

    // Example 6: 4x4 Diagonal matrix
    std::cout << "\n\nExample 6: 4x4 Diagonal Matrix\n";
    std::cout << "-------------------------------\n";
    sm::mat44<double> D;
    D[0] = 2.0;
    D[5] = 3.0;
    D[10] = 5.0;
    D[15] = 7.0;

    std::cout << "Diagonal matrix D =\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "  [ ";
        for (int j = 0; j < 4; ++j) {
            std::cout << std::setw(3) << D[j * 4 + i];
            if (j < 3) std::cout << ", ";
        }
        std::cout << " ]\n";
    }

    sm::vec<std::complex<double>, 4> diag_lambdas = D.eigenvalues();
    std::cout << "\nD.eigenvalues() returns:\n";
    for (size_t i = 0; i < 4; ++i) {
        std::cout << "  lambda " << i << " = " << diag_lambdas[i].real();
        if (std::abs (diag_lambdas[i].imag()) > my_epsilon) {
            std::cout << " + " << diag_lambdas[i].imag() << "i";
        }
        std::cout << "\n";
    }

    // Example 7: 4x4 eigenpairs
    std::cout << "\n\nExample 7: 4x4 eigenpairs\n";
    std::cout << "-------------------------\n";
    sm::mat44<double> B;
    B[0] = 1.0;
    B[5] = 2.0;
    B[10] = 3.0;
    B[15] = 4.0;

    std::cout << "Matrix B = diag(1, 2, 3, 4)\n";

    sm::vec<sm::mat44<double>::eigenpair, 4> mat44_pairs = B.eigenpairs();
    std::cout << "\nB.eigenpairs() returns:\n";
    for (size_t i = 0; i < 4; ++i) {
        std::cout << "  Pair " << i << ": lambda = " << mat44_pairs[i].eigenvalue.real();
        if (std::abs (mat44_pairs[i].eigenvalue.imag()) > my_epsilon) {
            std::cout << " + " << mat44_pairs[i].eigenvalue.imag() << "i";
        }
        std::cout << "\n          v = [";
        for (size_t j = 0; j < 4; ++j) {
            std::cout << std::fixed << std::setprecision(3)
                      << mat44_pairs[i].eigenvector[j].real();
            if (j < 3) std::cout << ", ";
        }
        std::cout << "]\n";
    }

    std::cout << "\n\n Eigenvalues and eigenvectors are now directly available\n";
    std::cout << " on mat22, mat33, and mat44 objects\n";
    std::cout << " Returns sm::vec<> vectors \n";
    std::cout << " Works with float, double, and long double types\n";

    return 0;
}
