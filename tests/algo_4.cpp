/*
 * Testing ray intersection functions
 */

#include <iostream>
#include <sm/algo>

int main()
{
    int rtn = 0;

    sm::vec<float> t0 = {0,0,0};
    sm::vec<float> t1 = {1,0,0};

    sm::vec<float> p = {-0.1, 0, 0};

    float d = sm::algo::dist_to_lineseg (t0, t1, p);
    std::cout << "Distance from " << p << " to " << t0 << "-" << t1 << " = " << d << std::endl;
    if (d != 0.1) { --rtn; }

    p = {0.5, 0.0001, 0.002};
    d = sm::algo::dist_to_lineseg (t0, t1, p);
    std::cout << "Distance from " << p << " to " << t0 << "-" << t1 << " = " << d << std::endl;
    if (d != 0.0020025) { --rtn; }


    sm::vec<float> t2 = {0,1,0};
    d = sm::algo::dist_to_tri_edge (t0, t1, t2, p);
    float d2 = sm::algo::dist_to_tri_edge_sq (t0, t1, t2, p);
    std::cout << "d to " << p << " = " << d << " sq: " << d2 << std::endl;
    if (d != 0.0020025f || d2 != 4.01e-6f) { --rtn; }

    p = {0, 0.9, 0};
    d = sm::algo::dist_to_tri_edge (t0, t1, t2, p);
    d2 = sm::algo::dist_to_tri_edge_sq (t0, t1, t2, p);
    std::cout << "d to " << p << " = " << d << " sq: " << d2 << std::endl;
    if (d != 0.0f || d2 != 0.0f) { --rtn; }

    p = {0, 0.8, 0};
    d = sm::algo::dist_to_tri_edge (t0, t1, t2, p);
    d2 = sm::algo::dist_to_tri_edge_sq (t0, t1, t2, p);
    std::cout << "d to " << p << " = " << d << " sq: " << d2 << std::endl;
    if (d != 0.0f || d2 != 0.0f) { --rtn; }

    p = {0, 1.1, 0};
    d = sm::algo::dist_to_tri_edge (t0, t1, t2, p);
    d2 = sm::algo::dist_to_tri_edge_sq (t0, t1, t2, p);
    std::cout << "d to " << p << " = " << d << " sq: " << d2 << std::endl;
    if (d != 0.1f || d2 != 0.01f) { --rtn; }

    std::cout << "Test " << (rtn ? "success" : "failure") << std::endl;

    return rtn;
}
