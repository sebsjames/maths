#include <iostream>
#include <vector>
#include <sm/range>

int main()
{
    int rtn = 0;

    sm::range<float> r(2.0f, 4.0f);
    if (r.update (1.0f) == false) { --rtn; } // Update with 1 should change the range and return true
    if (r.update (5.0f) == false) { --rtn; } // Update with 5 should change the range and return true
    if (r.update (3.0f) == true) { --rtn; } // Update with 3 should not change the range

    sm::range<int> r1 = { 1, 100 };
    sm::range<int> r2 = { 10, 90 };
    sm::range<int> r3 = { -1, 2 };
    sm::range<int> r4 = { 90, 100 };
    sm::range<int> r5 = { 90, 101 };
    sm::range<int> r6 = { 101, 102 };
    std::cout << "range " << r1 << (r1.contains(r2) ? " contains " : " doesn't contain ") << r2 << std::endl;
    std::cout << "range " << r1 << (r1.contains(r3) ? " contains " : " doesn't contain ") << r3 << std::endl;

    if (r1.contains(r2) == false) { --rtn; }
    if (r1.contains(r3) == true) { --rtn; }
    if (r1.contains(r4) == false) { --rtn; }
    if (r1.contains(r5) == true) { --rtn; }
    if (r1.contains(r6) == true) { --rtn; }

    std::vector<int> vint = { 1, -2, 5, 19 };
    sm::range<int> r_fromvec = sm::range<int>::get_from (vint);
    std::cout << "range from vector: " << r_fromvec << std::endl;
    if (r_fromvec.min != -2 || r_fromvec.max != 19) { --rtn; }

    std::cout << "Test " << (rtn == 0 ? "Passed" : "Failed") << std::endl;
    return rtn;
}
