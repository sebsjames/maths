#include <iostream>
#include <cmath>
#include <iomanip>
#include <sm/mat44>

int main()
{
    constexpr uint32_t N = 4;
    int rtn = 0;

    std::cout << "=== mat44 Eigenvector Tests ===\n\n";

    // Test 1: An example matrix
    {
        std::cout << "Test 1: An example matrix\n";

        sm::mat44<double> A = {
            1.00671141, -0.11835884,  0.87760447,  0.82343066,
            -0.11835884,  1.00671141, -0.43131554, -0.36858315,
            0.87760447, -0.43131554,  1.00671141,  0.96932762,
            0.82343066, -0.36858315,  0.96932762, 1.00671141
        };

        std::cout << "  Matrix:\n" << A << std::endl;
        sm::vec<typename sm::mat44<double>::eigenpair, N> eps = A.eigenpairs();

        std::cout << "Eigenvalues:\n";
        for (uint32_t i = 0; i < N; ++i) { std::cout << eps[i].eigenvalue << "\n"; }
        std::cout << std::endl;

        for (uint32_t i = 0; i < N; ++i) {
            std::cout << "Eigenvalue " << eps[i].eigenvalue;
            std::cout << " corresponds to Eigenvector ";
            std::cout << "[";
            for (uint32_t j = 0; j < N; ++j) {
                std::cout << eps[i].eigenvector[j] << " ";
            }
            std::cout << "]" << std::endl;
        }
        // sm::vec<double, N> exptd_eigenvalues = {};
    }

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");
    return rtn;
}
