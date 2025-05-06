#include <sj/vec>
#include <sj/vvec>

int main()
{
    int rtn = 0;
    sj::vec<float, 4> v = {1,2,3};

    std::cout << "v: " << v << " v.sq(): " << v.sq() << std::endl;
    std::cout << "v: " << v << " v.log(): " << v.log() << std::endl;
    std::cout << "v: " << v << " v.exp(): " << v.exp() << std::endl;
    v = { -2, 2, 3, -10 };
    std::cout << "v: " << v << " v.abs(): " << v.abs() << std::endl;

    std::cout << "v: " << v;
    v.sq_inplace();
    std::cout << " v.sq (in place): " << v << std::endl;

    sj::vvec<float> vv = { 1, 2, 3, 4 };
    std::cout << "vv before rotate pairs: " << vv;
    vv.rotate_pairs();
    std::cout << " vv after rotate_pairs: " << vv << std::endl;

    sj::vvec<float> vv2 = { 2, 2, 3, 8 };
    std::cout << "vv=" << vv << ", vv2=" << vv2 << std::endl;
    std::cout << "vv/vv2 = " << (vv / vv2) << std::endl;

    std::cout << "vv*vv2 = " << (vv * vv2) << std::endl;

    v = { 1, 2, 3, 4 };
    std::cout << "v before rotate pairs: " << v;
    v.rotate_pairs();
    std::cout << " v after rotate_pairs: " << v << std::endl;

    sj::vec<float, 4> v2 = { 2, 2, 3, 8 };
    std::cout << "v=" << v << ", v2=" << v2 << std::endl;
    std::cout << "v/v2 = " << (v / v2) << std::endl;

    std::cout << "v*v2 = " << (v * v2) << std::endl;

    return rtn;
}
