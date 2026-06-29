#include <iostream>
#include <cstdint>
#include <cmath>

import sklearn.iris;
import sm.vec;
import sm.vvec;
import sm.pca;

int main()
{
    sm::vvec<sm::vec<double, 4>> x;
    x.resize (sklearn::iris.size());
    for (std::uint32_t i = 0; i < sklearn::iris.size(); ++i) {
        x[i] = sklearn::iris[i].template as<double>();
    }
    std::cout << "sm::pca::compute...\n";
    sm::pca::result<double, 4> pca_res = sm::pca::compute<double, 4> (x);
    std::cout << "\nMeans and SDs of the input data are " << pca_res.mu_sig_x << std::endl;
    std::cout << "\nThe covariance matrix of the zeroed data is\n\n" << pca_res.covariance.str() << std::endl;

    std::cout << "Principal Components:\n\n";

    for (std::uint32_t i = 0; i < 4; ++i) {
        std::cout << "PC " << (i + 1) << " = " << pca_res.pc_vectors[i] << " which accounts for " << pca_res.pc_proportions[i] << " of the variability\n";
    }

    int rtn = 0;
    constexpr double eps = 0.00000001;

    // These test values come from running sklearn_iris.py. First check the first principal components
    sm::vec<double, 4> pc1_expected = { 0.36138659, -0.08452251, 0.85667061, 0.3582892 };
    sm::vec<double, 4> diffs = (pc1_expected - pca_res.pc_vectors[0]).abs();
    if (diffs > eps) { --rtn; }

    // Now check all the explained variances
    if (std::abs(pca_res.pc_proportions[0] - 0.92461872) > eps) {
        std::cout << std::abs(pca_res.pc_proportions[0] - 0.92461872) << " is > " << eps << std::endl;
        --rtn;
    }
    if (std::abs(pca_res.pc_proportions[1] - 0.05306648) > eps) {
        std::cout << std::abs(pca_res.pc_proportions[1] - 0.05306648) << " is > " << eps << std::endl;
        --rtn;
    }
    if (std::abs(pca_res.pc_proportions[2] - 0.01710261) > eps) {
        std::cout << std::abs(pca_res.pc_proportions[2] - 0.01710261) << " is > " << eps << std::endl;
        --rtn;
    }
    if (std::abs(pca_res.pc_proportions[3] - 0.00521218) > eps) {
        std::cout << std::abs(pca_res.pc_proportions[3] - 0.00521218) << " is > " << eps << std::endl;
        --rtn;
    }

    std::cout << "Test " << (rtn ? "FAILED" : "PASSED") << std::endl;
    return rtn;
}
