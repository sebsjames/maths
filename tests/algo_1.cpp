#include <iostream>
#include <sm/algo>

int main()
{
    int rtn = 0;

    int NEps = 2;
    float rad_f = 10.0f;
    std::cout << rad_f;
    sm::algo::zero_to_twopi (rad_f);
    std::cout << " radians is equivalent to " << rad_f << std::endl;
    if (std::abs(rad_f - 3.716814693f) > std::numeric_limits<float>::epsilon() * NEps) { --rtn; }

    NEps = 1;
    double rad_d = 10.0;
    std::cout << rad_d;
    sm::algo::zero_to_twopi (rad_d);
    std::cout << " radians is equivalent to " << rad_d << std::endl;
    if (std::abs(rad_d - 3.716814693) > std::numeric_limits<float>::epsilon() * NEps) { --rtn; }

    NEps = 150;
    rad_f = 10.0f + (sm::mathconst<float>::two_pi * 100);
    std::cout << rad_f;
    sm::algo::zero_to_twopi (rad_f);
    std::cout << " radians is equivalent to " << rad_f << std::endl;
    if (std::abs(rad_f - 3.716814693f) > std::numeric_limits<float>::epsilon() * NEps) { --rtn; }

    NEps = 1;
    rad_d = 10.0 + (sm::mathconst<double>::two_pi * 100);
    std::cout << rad_d;
    sm::algo::zero_to_twopi (rad_d);
    std::cout << " radians is equivalent to " << rad_d << std::endl;
    if (std::abs(rad_d - 3.716814693) > std::numeric_limits<float>::epsilon() * NEps) { --rtn; }

    std::cout << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
    return rtn;
}
