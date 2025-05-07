#include <utility>
#include <iostream>
#include <fstream>
#include <limits>

#include <sm/bezcurve>

int main()
{
    int rtn = 0;
    sm::vvec<sm::vec<float, 2>> c = {
        {1, 1},
        {2, 8},
        {9, 8},
        {10,1}
    };

    sm::bezcurve<FLT> cv (c);
    std::cout << "Defined a " << cv.getOrder() << " nd/rd/th order curve" << std::endl;

    std::cout << "cv = [" << cv.output(FLT{1}) << "];\n";

    std::pair<arma::Mat<FLT>, arma::Mat<FLT>> nc = cv.split (FLT{0.5});

    std::cout << "oc=[" << cv.outputControl() << "]\n";
    std::cout << "c1=[" << nc.first << "]\n";
    std::cout << "c2=[" << nc.second << "]\n";

    sm::bezcurve<FLT> cv1 (nc.first);
    sm::bezcurve<FLT> cv2 (nc.second);

    std::cout << "cv1 = [" << cv1.output(FLT{1}) << "];\n";
    std::cout << "cv2 = [" << cv2.output(FLT{1}) << "];\n";

    return rtn;
}
