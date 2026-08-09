#include <iostream>
#include <stdexcept>
#include <cstdint>

import sm.spline;

int main()
{
    int rtn = 0;
    // Test cubic_spline_expansion
    sm::vvec<float> v_orig = { 1.0f, 2.0f, 3.0f, 4.0f }; // must be 4 elements to use sm::cubic_spline_expansion<float, 4>
    sm::vvec<float> v = v_orig;
    std::cout << "vvec is init: " << v << std::endl;
    sm::cubic_spline_expansion<float, 4> (v, 3u); // insert 3 points between each of the original 4
    std::cout << "vvec is now: " << v << std::endl;
    // Last element of v should still be 4
    if (v_orig[0] != v[0] || v_orig[3] != v[v.size() - 1]) { --rtn; }

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");
    return rtn;
}
