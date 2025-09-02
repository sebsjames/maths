#include <iostream>
#include <sm/vec>
#include <sm/range>

int main()
{
    int rtn = 0;

    sm::range<float> r = { 4, 5 };
    std::cout << "initial range: " << r << std::endl;
    std::cout << "range + 1: " << (r+1) << std::endl;
    std::cout << "range - 1: " << (r-1) << std::endl;
    r += 1;
    std::cout << "range after +=1: " << r << std::endl;
    r -= 1;
    std::cout << "range after -=1: " << r << std::endl;
    if (r.min != 4 && r.max != 5) { --rtn; }

    sm::range<sm::vec<>> rv = { {0,0,0}, {1,1,1} };
    std::cout << "initial vector range: " << rv << std::endl;
    std::cout << "range + uz: " << rv + sm::vec<>::uz() << std::endl;
    auto rvn = rv + sm::vec<>::uz();
    if (rvn.min != sm::vec<>::uz() || rvn.max != sm::vec<>{1,1,2}) { --rtn; }

    std::cout << std::endl << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
    return rtn;
}
