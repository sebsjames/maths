#include <iostream>
#include <sm/bezcurve>

int main()
{
    int rtn = -1;

    sm::vvec<sm::vec<float, 2>> c = {
        {-0.28f, 0.0f},
        {0.28f, 0.0f},
        {0.28f, 0.45f},
        {-0.28f, 0.45f}
    };

    sm::bezcurve<FLT> cv;
    cv.fit (c);
    std::cout << "Defined a " << cv.getOrder() << " nd/rd/th order curve" << std::endl;

    // Now get points and output
    std::cout << "f=[\n" << cv.output (static_cast<unsigned int>(40)) << "];\n\n";

    std::cout << "p=[\n";
    for (auto p : c) {
        std::cout << p << std::endl;
    }
    std::cout << "];\n\n";

    std::cout << "c=[\n";
    std::cout << cv.outputControl();
    std::cout << "];\n\n";

    rtn = 0;

    return rtn;
}
