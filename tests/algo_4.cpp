/*
 * Testing ray intersection functions
 */

#include <iostream>
#include <sm/algo>

int main()
{
    int rtn = 0;

    sm::vec<float> t0 = {0,0,0};
    sm::vec<float> t1 = {0,0,1};
    sm::vec<float> t2 = {0,1,0};

    sm::vec<float> p = {0, 0.5, 0.1};
    std::cout << "d to " << p << " = " << sm::algo::dist_to_tri_edge (t0, t1, t2, p) << std::endl;
    p = {0, 0.7, 0.1};
    std::cout << "d to " << p << " = " << sm::algo::dist_to_tri_edge (t0, t1, t2, p) << std::endl;
    p = {0, 0.9, 0.1};
    std::cout << "d to " << p << " = " << sm::algo::dist_to_tri_edge (t0, t1, t2, p) << std::endl;
    p = {0, 0.9, 0.01};
    std::cout << "d to " << p << " = " << sm::algo::dist_to_tri_edge (t0, t1, t2, p) << std::endl;

    return rtn;
}
