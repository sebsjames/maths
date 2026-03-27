/*
 * Test ops on non-square mats
 */

#include <iostream>
import sm.mat;

int main()
{
    int rtn = 0;

    sm::mat<float, 2, 4> twobyfour;
    twobyfour = { 1, 2, 3, 4, 5, 6, 7, 8 };

    sm::mat<float, 4, 6> fourbysix;
    fourbysix = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };

    sm::mat<float, 2, 6> res1 =  twobyfour * fourbysix;
    std::cout << "res1\n"  << res1 << "\n";
    if (res1(0, 0) != 50.0f) { --rtn; std::cout << res1(0,0) << " 0a\n"; }
    if (res1(0, 1) != 114.0f) { --rtn; std::cout << res1(0,1) << " 0b\n"; }
    if (res1(1, 0) != 60.0f) { --rtn; std::cout << res1(1,0) << " 0c\n"; }
    if (res1(1, 1) != 140.0f) { --rtn; std::cout << res1(1,1) << " 0d\n"; }
    if (res1(1, 2) != 220.0f) { --rtn; std::cout << res1(1,2) << " 1\n"; }
    if (res1(0, 3) != 242.0f) { --rtn; std::cout << res1(0,3) << " 2\n"; }
    if (res1(0, 5) != 370.0f) { --rtn; std::cout << res1(0,5) << " 3\n"; }

    sm::mat<float, 4, 2> fourbytwo;
    fourbytwo = { 1, 2, 3, 4, 5, 6, 7, 8 };

    float el01 = fourbytwo(0,1);
    float el30 = fourbytwo(3,0);

    if (el01 != 5.0f) { --rtn; std::cout << "4\n"; }
    if (el30 != 4.0f) { --rtn;  std::cout << "5\n"; }

    fourbytwo(3,0) = 30.0f;
    el30 = fourbytwo(3,0);
    if (el30 != 30.0f) { --rtn; std::cout << "6\n"; }

    sm::vec<float, 2> r0 = fourbytwo.row (0);
    sm::vec<float, 2> r1 = fourbytwo.row (1);
    sm::vec<float, 2> r2 = fourbytwo.row (2);
    sm::vec<float, 2> r3 = fourbytwo.row (3);

    if (r0 != sm::vec<float, 2>{1,5}) { --rtn; std::cout << "7\n"; }
    if (r1 != sm::vec<float, 2>{2,6}) { --rtn; std::cout << "8\n"; }
    if (r2 != sm::vec<float, 2>{3,7}) { --rtn; std::cout << "9\n"; }
    if (r3 != sm::vec<float, 2>{30,8}) { --rtn; std::cout << "10\n"; }

    sm::vec<float, 4> c0 = fourbytwo.col (0);
    if (c0 != sm::vec<float, 4>{1,2,3,30}) { --rtn; std::cout << "11\n";  }
    sm::vec<float, 4> c1 = fourbytwo.col (1);
    if (c1 != sm::vec<float, 4>{5,6,7,8}) { --rtn; std::cout << c1 << " 12\n";  }

    fourbytwo.set_col(0, sm::vec<float, 4>{7,8,9,10});
    c0 = fourbytwo.col (0);
    if (c0 != sm::vec<float, 4>{7,8,9,10}) { --rtn; std::cout << "13\n";  }

    sm::mat<float, 4, 4> fourbyfour;
    fourbyfour = { 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8  };
    std::cout << "\n" << fourbyfour << "\n *\n" << fourbytwo << std::endl;
    sm::mat<float, 4, 2> res2 = (fourbyfour * fourbytwo);
    std::cout << " =\n" << res2 << std::endl;
    if (res2(1,3) != 160.0f) { --rtn; std::cout << "14\n"; }
    if (res2(0,0) != 106.0f) { --rtn; std::cout << "15\n"; }

    sm::mat<float, 2, 2> A;
    A = {1, 2, 3, 4};
    sm::mat<float, 2, 1> B;
    B = {5, 6};
    sm::mat<float, 2, 1> C = A * B;

    std::cout << "\n" << A << "\n *\n" << B << " =\n" << C << std::endl;

    if (C[0] != 23.0f || C[1] != 34.0f) { --rtn; }



    std::cout << (rtn == 0 ? "SUCCESS\n" : "FAILURE\n");
    return rtn;
}
