#include <iostream>

import sm.vvec;

int main()
{
    int rtn = 0;

    sm::vvec<float> vv = { 1,2,3,4 };

    // Multiply and accumulate
    sm::vvec<float> r1 = vv.macc (2.0f, 2.0f);

    if (r1[0] != 4.0f || r1[1] != 6.0f || r1[2] != 8.0f || r1[3] != 10.0f) { --rtn; }

    std::cout << vv << " * 2 + 2 = " << r1 << std::endl;

    sm::vvec<float> r2 = vv;

    r2.macc_inplace (3.0f, 10.0f);

    std::cout << vv << " * 3 + 10 = " << r2 << std::endl;

    if (r2[0] != 13.0f || r2[1] != 16.0f || r2[2] != 19.0f || r2[3] != 22.0f) { --rtn; }

    if (rtn == 0) { std::cout << "Success\n"; }
    return rtn;
}
