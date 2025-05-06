/*
 * Test use of vec<F, N> objects in a scale class.
 */

#include <cmath>
#include <vector>
#include <list>
#include <array>
#include <iostream>
#include <sj/vec>
#include <sj/scale>

int main ()
{
    int rtn = 0;

    sj::scale<sj::vec<float,4>> s2;
    s2.do_autoscale = true;
    std::vector<sj::vec<float,4>> vaf;
    vaf.push_back ({1,1,2,1});
    vaf.push_back ({2,2,2,3});
    vaf.push_back ({3,3,4,1});
    vaf.push_back ({4,4,4,4});
    std::vector<sj::vec<float,4>> result2(vaf);
    s2.transform (vaf, result2);

    std::cout << "vector<vec<float,4>> unscaled/scaled vectors:\n";
    for (unsigned int i = 0; i < result2.size(); ++i) {

        std::cout << "(";
        for (auto v : vaf[i]) { std::cout << v << ","; }
        std::cout << ")   ";

        std::cout << "(";
        for (auto v : result2[i]) { std::cout << v << ","; }
        std::cout << ")\n";
    }

    std::cout << "Stream scale<vec<float,4>>: " << s2 << std::endl;

    // Test this scaling:
    std::vector<sj::vec<float, 4>>::const_iterator r2i = result2.end();
    r2i--; // To get to last element in vector
    float r2ilen = std::sqrt ((*r2i)[0] * (*r2i)[0] + (*r2i)[1] * (*r2i)[1] + (*r2i)[2] * (*r2i)[2] + (*r2i)[3] * (*r2i)[3]);
    if (std::abs(r2ilen - 1) > 0.0001) {
        std::cout << "Error" << std::endl;
        rtn--;
    }

    return rtn;
}
