#include <iostream>
#include <cstdint>

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
}
