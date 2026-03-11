/*
 * An example that creates a bezier curve
 */

#include <iostream>
#include <sm/bezcurve>

import sm.vec;
import sm.vvec;

int main()
{
    sm::vvec<sm::vec<float, 2>> c = {
        {-0.28f, 0.0f},
        {0.28f, 0.0f},
        {0.28f, 0.45f},
        {-0.28f, 0.45f}
    };

    sm::bezcurve<float, 3> cv;
    cv.fit (c);
    std::cout << "Defined a " << cv.getOrder() << " nd/rd/th order curve" <<  std::endl;

    // Now get points and output
    std::cout << "Plot f in octave with plot (f(:,1), f(:,2))\n";
    std::cout << "f=[\n" << cv.output (40u) << "];\n\n";

    std::cout << "p=[\n";
    for (auto p : c) {
        std::cout << p << std::endl;
    }
    std::cout << "];\n\n";

    std::cout << "c=[\n";
    std::cout << cv.outputControl();
}
