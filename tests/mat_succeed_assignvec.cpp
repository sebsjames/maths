#include <iostream>

import sm.vec;
import sm.mat;

int main()
{
    int32_t rtn = 0;
    sm::mat<float, 4> m;
    auto v = sm::vec<float, 16>{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4};
    m = v;
    if (m.arr == v) { std::cout << "Equals, Good\n"; } else { --rtn; }
    if (m.arr != v) { --rtn; } else { std::cout << "Not Not-equals, Good\n"; }
    return rtn;
}
