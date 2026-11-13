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

    sm::vvec<double> vv_2 = { 0.1, 0.2, 0.3, 0.2 };
    sm::vvec<double> vv_3 = { 2, 2, 3, 4 };

    std::cout << "Here we go\n";
    sm::vvec<float> r3 = vv.macc (vv_2, vv_3);
    std::cout << vv << " * " << vv_2 << " + " << vv_3 <<"=\n   " << r3 << std::endl;

    sm::vvec<float> r4 = vv.macc (vv_2, 10.0f);
    std::cout << vv << " * " << vv_2 << " + " << 10.0f <<"=\n   " << r4 << std::endl;

    sm::vvec<float> r5 = vv.macc (10.0f, vv_2);
    std::cout << vv << " * " << 10.0f << " + " << vv_2 <<"=\n   " << r5 << std::endl;

    sm::vvec<float> r6 = vv;
    sm::vvec<float> r7 = vv;
    sm::vvec<float> r8 = vv;
    sm::vvec<float> r9 = vv;

    r6.macc_inplace (3.0f, 10.0f);
    r7.macc_inplace (3.0f, vv_2);
    r8.macc_inplace (vv_2, 10.0f);
    r9.macc_inplace (vv_2, vv_2);

    if (rtn == 0) { std::cout << "Success\n"; }
    return rtn;
}
