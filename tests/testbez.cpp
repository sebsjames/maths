#include <utility>
#include <iostream>
#include <fstream>

#include <morph/bezcurve.h>

int main()
{
    int rtn = -1;

    // Make some control points
    morph::vec<float, 2> i, f, c1, c2;
    i = {1,1};
    c1 = {5,5};
    c2 = {2,-4};
    f = {10,1};

    // Make a cubic curve
    morph::bezcurve<float> cc(i, f, c1, c2);

    // Length of step along curve
    std::ofstream f1;
    f1.open ("tests/curve.csv", std::ios::trunc|std::ios::out);
    f1.precision(12);
    std::vector<morph::bezcoord<float>> a = cc.computePoints ((unsigned int)100);
    typename std::vector<morph::bezcoord<float>>::iterator ai = a.begin();
    int ii = 0;
    while (ai != a.end()) {
        if (ai->getNullCoordinate() == false) {
            f1 << *(ai) << std::endl;

            // Pick a value out of the list and compare to validate this test
            if (ii == 55) {
                if ((ai->x() - 4.24224996567f) < 0.00000001
                    && (ai->y() - 0.294625133276f) < 0.00000001
                    && (ai->t() - 0.55f) < 0.00000001) {
                    rtn = 0; // return success!
                } else {
                    std::cerr << "Failed. ai->x(): " << ai->x() << " ai->y(): " << ai->y() << " ai->t(): " << ai->t() << std::endl;
                }
            }

        } // else remaining == ai->getRemaining()

        ++ai; ++ii;
    }
    f1.close();

    std::ofstream f2;
    f2.open ("tests/ctrl.csv", std::ios::trunc|std::ios::out);
    f2 << i << std::endl;
    f2 << c1 << std::endl;
    f2 << c2 << std::endl;
    f2 << f << std::endl;
    f2.close();

    return rtn;
}
