#include <iostream>
import sm.vec;
import sm.interval;

int main()
{
    int rtn = 0;

    sm::interval<float> r = { 4, 5 };
    std::cout << "initial interval: " << r << std::endl;
    std::cout << "interval + 1: " << (r+1) << std::endl;
    std::cout << "interval - 1: " << (r-1) << std::endl;
    r += 1;
    std::cout << "interval after +=1: " << r << std::endl;
    r -= 1;
    std::cout << "interval after -=1: " << r << std::endl;
    if (r.min != 4 && r.max != 5) { --rtn; }

    sm::interval<sm::vec<>> rv = { {0,0,0}, {1,1,1} };
    std::cout << "initial vector interval: " << rv << std::endl;
    std::cout << "interval + uz: " << rv + sm::vec<>::uz() << std::endl;
    auto rvn = rv + sm::vec<>::uz();
    if (rvn.min != sm::vec<>::uz() || rvn.max != sm::vec<>{1,1,2}) { --rtn; }

    std::cout << std::endl << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
    return rtn;
}
