#include <iostream>

import sm.vec;
import sm.interval;

int main()
{
    int rtn = 0;

    sm::interval<sm::vec<float>> rv1 = sm::interval<sm::vec<float>>{ sm::vec<float>{0,0,0}, sm::vec<float>{1,1,1} };
    sm::interval<sm::vec<float>> rv2 = sm::interval<sm::vec<float>>{ sm::vec<float>{0,0,0}, sm::vec<float>{1,1,1} };

    std::cout << rv1 << ", " << rv2 << std::endl;

    if (rv1 == rv2) { rtn = 0; }

    //if (!(rv1 == rv2)) { --rtn; }

#if 0
    if (rv1 != rv2) { --rtn; }

    sm::interval<sm::vec<float>> rv3 = { sm::vec<float>{0,1,0}, sm::vec<float>{1,1,1} };

    if (rv1 == rv3) { --rtn; }
    if (!(rv1 != rv3)) { --rtn; }

    std::cout << std::endl << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
#endif
    return rtn;
}
