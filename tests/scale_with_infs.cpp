#include <iostream>
#include <limits>
#include <vector>

import sm.scale;
import sm.vvec;

int main ()
{
    int rtn = 0;

    sm::scale<float> s;
    s.do_autoscale = true;

    sm::vvec<float> vf = { 1, 2, 3, 4, 5, std::numeric_limits<float>::infinity(), 9, 18};

    sm::vvec<float> result(vf);
    s.transform (vf, result);
    std::cout << "s output_range: " << s.output_range << std::endl;
    std::cout << "1st data: Unscaled/scaled: ";
    for (unsigned int i = 0; i < vf.size(); ++i) {
        std::cout << vf[i] << "/" << result[i] << ", ";
    }
    std::cout << std::endl;

    if (result[0] != 0.0f || result[7] != 1.0f) { --rtn; }

    sm::scale<float> s1;
    sm::scale<float> s2;
    std::vector<float> input_data = { 1.0f, 2.0f, 3.0f, 6.0f, std::numeric_limits<float>::infinity() };
    s1.compute_scaling_from_data (input_data);
    // equivalent: s1.compute_scaling_from_data<std::vector<float>, false> (input_data);
    s2.compute_scaling_from_data<std::vector<float>, true> (input_data);
    std::cout << "scaled ignoring infinities:\n   " << s1 << std::endl;
    std::cout << "scaled without ignoring infinities:\n   " << s2 << std::endl;

    std::cout << "Test " << (rtn == 0 ? "Passed" : "Failed") << std::endl;
    return rtn;
}
