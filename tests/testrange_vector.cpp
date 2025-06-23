#include <iostream>
#include <sm/vec>
#include <sm/range>

int main()
{
    int rtn = 0;

    sm::range<sm::vec<float,3>> r(sm::vec<float,3>{1, 2, 3}, sm::vec<float,3>{45, 67, 93});
    std::cout << "A vector range is " << r << std::endl;

    std::vector<sm::vec<float, 2>> vectorvec = { {1,0}, {0,1}, {2,2} };
    sm::range<sm::vec<float, 2>> r2 = sm::range<sm::vec<float, 2>>::get_from (vectorvec);
    std::cout << "Vector range from get_from: " << r2 << std::endl;

    std::cout << "Test " << (rtn == 0 ? "Passed" : "Failed") << std::endl;
    return rtn;
}
