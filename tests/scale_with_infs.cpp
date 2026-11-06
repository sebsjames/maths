#include <iostream>

import sm.vec;
import sm.scale;
//import sm.vvec;  // for clang (scale export imports vvec)
//import sm.interval; // for clang (vec, vvec export imports vvec)

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


    std::cout << "Test " << (rtn == 0 ? "Passed" : "Failed") << std::endl;
    return rtn;
}
