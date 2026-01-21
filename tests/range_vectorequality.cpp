#include <iostream>
#include <sm/vec>
#include <sm/range>

int main()
{
    int rtn = 0;

    sm::range<sm::vec<>> rv1 = { {0,0,0}, {1,1,1} };
    sm::range<sm::vec<>> rv2 = { {0,0,0}, {1,1,1} };

    if (!(rv1 == rv2)) { --rtn; }
    if (rv1 != rv2) { --rtn; }

    sm::range<sm::vec<>> rv3 = { {0,1,0}, {1,1,1} };

    if (rv1 == rv3) { --rtn; }
    if (!(rv1 != rv3)) { --rtn; }

    std::cout << std::endl << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
    return rtn;
}
