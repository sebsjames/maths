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
    [[maybe_unused]] sm::pca::result<double, 4> pca_res = sm::pca::compute<double, 4> (x);
    for (std::uint32_t i = 0; i < 4; ++i) {
        std::cout << "PC " << (i + 1) << " = " << pca_res.pc_ev_real[i] << " which accounts for " << pca_res.pc_mags[i] << " of the variability\n";
        std::cout << "\n" << pca_res.x_proj[i] << "\n";
    }
}
