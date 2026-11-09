#include <iostream>
#include <complex>

import sm.interval;
import sm.vec;

int main()
{
    int rtn = 0;

    sm::interval<std::complex<float>> rc({2.0f, 1.0f}, {4.0f, -1.0f});
    if (rc.has_inf()) { --rtn; }
    rc.max = std::numeric_limits<std::complex<float>>::infinity();
    if (!rc.has_inf()) { --rtn; }

    sm::interval<double> rs(2.0, 6.0);
    if (rs.has_inf()) { --rtn; }
    rs.min = std::numeric_limits<float>::infinity();
    if (!rs.has_inf()) { --rtn; }

    sm::interval<sm::vec<float>> rv { { 0.0f }, { 1.0f, 1.0f, 1.0f } };
    if (rv.has_inf()) { --rtn; }
    rv.min = { 1.0f, 3000.0f, std::numeric_limits<float>::infinity() };
    if (!rv.has_inf()) { --rtn; }
    
    std::cout << "Test " << (rtn == 0 ? "Passed" : "Failed") << std::endl;
    return rtn;
}
