#include <iostream>

import sm.vec;
import sm.mat;

int main()
{
    sm::mat<float, 4> m;
    m.translate (sm::vec<float>{1,2,3});
    sm::vec<float, 3> v3 = {0.1f, 0.2f, 0.3f};
    m -= v3; // Should not compile
    std::cout << "m after -= v3\n" << m << std::endl;
    return 0;
}
