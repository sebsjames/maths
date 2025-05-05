#include "sj/vec.h"
#include "sj/vvec.h"
using std::cout;
using std::endl;
using std::array;

int main() {
    int rtn = 0;
    sj::vec<float, 4> v = {1,2,3};

    cout << "v: " << v << " v.sq(): " << v.sq() << endl;
    cout << "v: " << v << " v.log(): " << v.log() << endl;
    cout << "v: " << v << " v.exp(): " << v.exp() << endl;
    v = { -2, 2, 3, -10 };
    cout << "v: " << v << " v.abs(): " << v.abs() << endl;

    cout << "v: " << v;
    v.sq_inplace();
    cout << " v.sq (in place): " << v << endl;

    sj::vvec<float> vv = { 1, 2, 3, 4 };
    cout << "vv before rotate pairs: " << vv;
    vv.rotate_pairs();
    cout << " vv after rotate_pairs: " << vv << endl;

    sj::vvec<float> vv2 = { 2, 2, 3, 8 };
    cout << "vv=" << vv << ", vv2=" << vv2 << endl;
    cout << "vv/vv2 = " << (vv / vv2) << endl;

    cout << "vv*vv2 = " << (vv * vv2) << endl;

    v = { 1, 2, 3, 4 };
    cout << "v before rotate pairs: " << v;
    v.rotate_pairs();
    cout << " v after rotate_pairs: " << v << endl;

    sj::vec<float, 4> v2 = { 2, 2, 3, 8 };
    cout << "v=" << v << ", v2=" << v2 << endl;
    cout << "v/v2 = " << (v / v2) << endl;

    cout << "v*v2 = " << (v * v2) << endl;

    return rtn;
}
