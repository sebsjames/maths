#include <iostream>
#include <chrono>
#include <limits>

import sm.vvec;

int main()
{
    using namespace std::chrono;
    using sc = std::chrono::steady_clock;

    int rtn = 0;

    sm::vvec<double> a (1000000, 0.0);
    a.randomize();

    auto t0 = sc::now();    
    a.renormalize();
    auto t1 = sc::now();    
    
    std::cout << "renormalize time: " << duration_cast<microseconds>((t1 - t0)).count() << " us\n";

    std::cout << "renormalized, length is now " << a.length() << std::endl;

    // The length is close to 1, but may be quite a few epsilons away with numeric errors
    if (std::abs(a.length() - 1.0) > (std::numeric_limits<double>::epsilon() * 100.0)) {
        --rtn;
        std::cout << "std::abs(a.length() - 1.0) = " << std::abs(a.length() - 1.0) << std::endl;
    }

    if (rtn == 0) { std::cout << "Success\n"; } else { std::cout << "Test failed\n"; }
    
    return rtn;
}
