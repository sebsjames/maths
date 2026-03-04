#include <iostream>

import sm.vec;
import sm.mat;

int main()
{
    sm::mat<float, 4> m;
    m.translate (sm::vec<float>{1,2,3});
    sm::vec<float, 16> v16 = {0.1f, 0.2f, 0.3f, 0.0f};
    std::cout << "m + v16\n" << (m + v16) << std::endl;    // Should not compile
    return 0;
}
