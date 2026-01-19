#include <iostream>
#include <cstdint>

#include <sm/vec>
#include <sm/vvec>
#include <sm/pca>

#include "sklearn_iris.h"

int main()
{
    sm::vvec<sm::vec<double, 4>> x;
    x.resize (sklearn::iris.size());
    for (uint32_t i = 0; i < sklearn::iris.size(); ++i) {
        x[i] = sklearn::iris[i].template as<double>();
    }
    [[maybe_unused]] sm::pca::result<double, 4> pca_res = sm::pca::compute<double, 4> (x);
}
