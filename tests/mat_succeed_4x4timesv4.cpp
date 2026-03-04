#include <iostream>
#include <limits>
#include <cstdint>

import sm.vec;
import sm.mat;

int main()
{
    int32_t rtn = 0;
    sm::mat<float, 4> m;
    m.translate (sm::vec<float>{1,2,3});
    sm::vec<float, 4> v4 = {0.1f, 0.2f, 0.3f, 1.0f};
    std::cout << "m * v4\n" << (m * v4) << std::endl;
    sm::vec<float, 4> res = m * v4;
    sm::vec<float, 4> expected = { 1.1f, 2.2f, 3.3f, 1.0f };
    for (uint32_t i = 0; i < 4; ++i) {
        if (std::abs(res[i] - expected[i]) > std::numeric_limits<float>::epsilon()) {
            --rtn;
        }
    }
    return rtn;
}
