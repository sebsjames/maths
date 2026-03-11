#include <utility>
#include <iostream>
#include <fstream>
#include <limits>

import sm.bezcurve;
import sm.vec;
import sm.vvec;

int main()
{
    int rtn = 0;
    sm::vvec<sm::vec<float, 2>> c = {
        {1, 1},
        {2, 8},
        {9, 8},
        {10,1}
    };

    sm::bezcurve<FLT, 3> cv (c);
    std::cout << "Defined a " << cv.getOrder() << " nd/rd/th order curve" << std::endl;

    std::cout << "cv = [" << cv.output(FLT{1}) << "];\n";

    std::pair<sm::mat<FLT, 4, 2>, sm::mat<FLT, 4, 2>> nc = cv.split (FLT{0.5});

    std::cout << "oc=[" << cv.outputControl() << "]\n";
    std::cout << "c1=[" << nc.first << "]\n";
    std::cout << "c2=[" << nc.second << "]\n";

    sm::bezcurve<FLT, 3> cv1 (nc.first);
    sm::bezcurve<FLT, 3> cv2 (nc.second);

    std::cout << "cv1 = [" << cv1.output(FLT{1}) << "];\n";
    std::cout << "cv2 = [" << cv2.output(FLT{1}) << "];\n";

    return rtn;
}
