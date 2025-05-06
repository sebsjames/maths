#include <set>
#include <algorithm>
#include <complex>

#include <sj/mathconst>
#include <sj/vec>

int main()
{
    int rtn = 0;

    using namespace std::complex_literals;
    sj::vec<std::complex<double>, 4> cplx;
    cplx.set_from (std::pow(1i, 2));
    std::cout << "Complex*2: " << cplx*2.0 << std::endl;

    sj::vec<float, 4> v = {1,2,3};
    // Test x(), y() etc members
    std::cout << "x: " << v.x() << std::endl;
    std::cout << "z: " << v.z() << std::endl;
    std::cout << "w: " << v.w() << std::endl;
    // Test renormalize
    v.renormalize();
    std::cout << "After renormalize: " << v << std::endl;
    // Test checkunit
    std::cout << "is it unit? " << v.checkunit() << std::endl;
    // Test randomize
    v.randomize(3,4);
    std::cout << "After randomize: " << v << std::endl;
    // Check ints are ok, too
    sj::vec<int, 2> vi;
    vi.randomize(0,200);
    std::cout << "After randomize of int sj::vec: " << vi << std::endl;
    std::cout << "Length: " << vi.length() << std::endl;
    sj::vec<float, 3> vfl;
    vfl.randomize();
    std::cout << "Length of a float vector: " << vfl.length() << std::endl;
    // Test assignment
    sj::vec<int, 2> vi2 = vi;
    std::cout << "Copy of int sj::vec: " << vi2 << std::endl;
    // Test comparison
    std::cout << "vi == vi2? " << (vi == vi2 ? "yes" : "no") << std::endl;
    // Test negate
    sj::vec<int, 2> vi3 = -vi;
    std::cout << "-ve Copy of int sj::vec: " << vi3 << std::endl;
    // Test comparison
    std::cout << "vi == vi3? " << (vi == vi3 ? "yes" : "no") << std::endl;
    // Test cross product (3D only
    sj::vec<double, 3> a = {1,0,0};
    sj::vec<double, 3> b = {0,1,0};
    sj::vec<double, 3> c = a.cross(b);
    std::cout << a << " cross " << b << "=" << c << std::endl;
    // Test dot product
    sj::vec<int, 2> vv1 = {1,1};
    sj::vec<int, 2> vv2 = {2,2};
    int dp = vv1.dot (vv2);
    std::cout << vv1 << "." << vv2 << " = " << dp << std::endl;

    // Assignment
#if 0 //  Copy FROM std::array to vec is NO good
    // Test init from array
    std::array<float, 3> arr = { 2,3,4 };
    sj::vec<float, 3> varr = arr; // Tried overloading operator= to no avail.
    std::cout << "sj::vec from array: " << varr << std::endl;
#endif

#if 1 // copy FROM vec TO std::array is OK
    // Test init from array
    sj::vec<float, 3> varr = { 2, 3, 4 };
    std::array<float, 3> arr = varr; // copy TO std::array
    std::cout << "std::array assigned to from sj::vec: ";
    for (auto a : arr) { std::cout << a << ", "; }
    std::cout << std::endl;
#endif

    // Test scalar multiply
    vv2 *= 2UL;
    std::cout << "vv2 after *2:" << vv2 << std::endl;
    sj::vec<int, 2> vv4 = vv1 * 98;
    std::cout << "vv1 * 98:" << vv4 << std::endl;
    // Scalar division
    sj::vec<double, 3> d = a/3.0;
    std::cout << "a/3.0:" << d << std::endl;
    // sj::vec addition
    sj::vec<double, 3> e = a+b;
    std::cout << "a+b:" << e << std::endl;
    // sj::vec subtraction
    sj::vec<double, 3> f = a-b;
    std::cout << "a-b:" << f << std::endl;
    // Test default template args
    sj::vec<double> vd_def;
    vd_def.randomize();
    std::cout << vd_def << std::endl;
    sj::vec<> v_def;
    v_def.randomize();
    std::cout << v_def << std::endl;

    // So you want to do the dot product of a 1000000 D sj::vec? Easy
    sj::vec<float, 1000> big1;
    sj::vec<float, 1000> big2;
    big1.randomize(0,100);
    big2.randomize(0,20);
    std::cout << "DP..." << std::endl;
    float bdp = big1.dot(big2);
    std::cout << "big1.big2=" << bdp << std::endl;

    // Test setFrom
    sj::vec<double, 3> d1;
    std::array<double, 3> a1 = { 5,6,7 };
    d1.set_from (a1);
    std::cout << "d1 should be 5,6,7: " << d1 << std::endl;
    std::array<double, 4> a2 = { 5,6,8,8 };
    d1.set_from (a2);
    std::cout << "d1 should be 5,6,8: " << d1 << std::endl;
    d1.set_from (5.6);
    std::cout << "d1 should be 5.6 for all elements: " << d1 << std::endl;

    // Test hadamard operator* (elementwise multiplication)
    sj::vec<double, 3> h1 = {1.0, 2.0, 3.0};
    sj::vec<double, 3> h2 = {7.0, 6.0, 5.0};
    sj::vec<double, 3> h3 = h1 * h2;
    std::cout << h1 << "(o)" << h2 << " = " << h3 << std::endl;

    h1 *= h2;
    std::cout << "After h1 *= h2, h1: " << h1 << std::endl;

    // Test operator *= with different types. Ok if lhs is same type as result.
    sj::vec<int, 3> h4 = {2, 2, 2};
    //sj::vec<int, 3> h5 = h2 * h4; // Not ok
    sj::vec<int, 3> h6 = h4 * h2; // sj::vec<int, N> * sj::vec<double, N> implies expected loss of precision.
    sj::vec<double, 3> h7 = h2 * h4;
    //sj::vec<double, 3> h8 = h4 * h2; // Not ok
    std::cout << h2 << "(o)" << h4 << " = " << h6 << " or " << h7 << std::endl;

    // Will VS be happy?
    h4 *= h2;

    sj::vec<float, 4> maxlongest = {-1.1f, -7.0f, 3.0f, 6.0f };
    std::cout << "For sj::vec " << maxlongest
         << ", max: " << maxlongest.max() << " (at index "<< maxlongest.argmax()
         << "), longest component: " << maxlongest.longest() << " (at index "
         << maxlongest.arglongest() << ")\n";

    sj::vec<float, 4> totimes = { 1.0f, 2.0f, 3.0f, 4.0f };
    std::cout << "Cumulative product of " << totimes << " is " << totimes.product() << std::endl;

    sj::vec<float, 4> fordivision = { 1.0f, 2.0f, 3.0f, 4.0f };
    sj::vec<float, 4> divresult = 1.0f / fordivision;
    std::cout << 1.0f << " / " << fordivision << " = " << divresult << std::endl;

    sj::vec<float, 3> compare1 = { 1, 2, 3 };
    sj::vec<float, 3> compare2 = { 2, 1, 3 };

    std::cout << "compare1 < compare2: " << (compare1 < compare2) << std::endl;
    std::cout << "compare2 < compare1: " << (compare2 < compare1) << std::endl;

    auto _cmp = [](sj::vec<float, 3> a, sj::vec<float, 3> b) { return a.lexical_lessthan(b); };
    std::set<sj::vec<float, 3>, decltype(_cmp)> aset(_cmp);
    aset.insert (compare1);
    aset.insert (compare2);
    std::cout << "aset size " << aset.size() << std::endl;

    sj::vec<double,2> VV1 = {1.0, 2.0};
    sj::vec<double,2> VV2 = {2.0, 3.0};
    sj::vec<double,2> VV3 = {1.0, 30.0};
    sj::vec<sj::vec<double, 2>, 3> VdV = { VV1, VV2, VV3 };
    std::cout<< "VdV.mean() = " << VdV.mean() << std::endl;

    sj::vec<float, 2> v_continuous = { 0.5f, 0.6f };
    sj::vec<size_t , 2> v_discrete = { 1, 2 };
    sj::vec<float, 2> v_cd = v_continuous * v_discrete;
    std::cout << "You can do sj::vec<floattype,N> = sj::vec<floattype,N> * sj::vec<inttype,N>: " << v_cd << std::endl;
    // Can't do this though:
    //  sj::vec<float, 2> v_dc = v_discrete * v_continuous; // will throw compiler error

    sj::vec<int, 4> vr = { 0, 1, 2, 3 };
    sj::vec<int, 4> vr2 = vr;

    sj::vec<int, 7> rot_size_t_correct = { 0, 1, 2, 3, 0, 1, 2 };
    for (size_t i = 0; i < 7; ++i) {
        vr2 = vr;
        vr2.rotate (i);
        std::cout << vr << " rotate("<<i<<") is " << vr2 << std::endl;
        if (vr2[0] != rot_size_t_correct[i]) { --rtn; }
    }

    sj::vec<int, 14> rot_int_correct = { 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2 };
    for (int i = -7; i < 7; ++i) {
        vr2 = vr;
        vr2.rotate (i);
        std::cout << vr << " rotate("<<i<<") is " << vr2 << std::endl;
        if (vr2[0] != rot_int_correct[i+7]) { --rtn; }
    }

    sj::vec<float, 3> formax;
    formax.set_max();
    std::cout << "vec<float, 3>::set_max gives: " << formax << std::endl;
    formax.set_lowest();
    std::cout << "vec<float, 3>::set_lowest gives: " << formax << std::endl;

    // Test shorten
    sj::vec<float, 2> lv = { 6.0f, 8.0f }; // a 3,4,5 vector
    sj::vec<float, 2> sv = lv.shorten (5.0f);
    std::cout << "lv: " << lv << " lv.shorten(5.0f) returns the vector: " << sv << std::endl;
    if (sv != sj::vec<float, 2>({ 3.0f, 4.0f })) { --rtn; }

    lv = { 6.0f, 8.0f }; // a 3,4,5 vector
    sv = lv.shorten (10.0f);
    std::cout << "lv: " << lv << " lv.shorten(10.0f) returns the vector: " << sv << std::endl;
    if (sv != sj::vec<float, 2>({ 0.0f, 0.0f })) { --rtn; }

    lv = { 6.0f, 8.0f }; // a 3,4,5 vector
    sv = lv.shorten (12.0f);
    std::cout << "lv: " << lv << " lv.shorten(12.0f) returns the vector: " << sv << std::endl;
    if (sv != sj::vec<float, 2>({ 0.0f, 0.0f })) { --rtn; }

    lv = { 6.0f, 8.0f }; // a 3,4,5 vector
    sv = lv.shorten (-5.0f); // shorten -ve lengthens
    std::cout << "lv: " << lv << " lv.shorten(-5.0f) returns the vector: " << sv << std::endl;
    if (sv != sj::vec<float, 2>({ 9.0f, 12.0f })) { --rtn; }

    lv = { 6.0f, 8.0f }; // a 3,4,5 vector
    sv = lv.lengthen (-5.0f); // lengthen -ve shortens
    std::cout << "lv: " << lv << " lv.lengthen(-5.0f) returns the vector: " << sv << std::endl;
    if (sv != sj::vec<float, 2>({ 3.0f, 4.0f })) { --rtn; }

    lv = { 6.0f, 8.0f }; // a 3,4,5 vector
    sv = lv.lengthen (-10.0f);
    std::cout << "lv: " << lv << " lv.lengthen(-10.0f) returns the vector: " << sv << std::endl;
    if (sv != sj::vec<float, 2>({ 0.0f, 0.0f })) { --rtn; }

    lv = { 6.0f, 8.0f }; // a 3,4,5 vector
    sv = lv.lengthen (-12.0f);
    std::cout << "lv: " << lv << " lv.lengthen(-12.0f) returns the vector: " << sv << std::endl;
    if (sv != sj::vec<float, 2>({ 0.0f, 0.0f })) { --rtn; }

    lv = { 6.0f, 8.0f }; // a 3,4,5 vector
    sv = lv.lengthen (5.0f);
    std::cout << "lv: " << lv << " lv.lengthen(5.0f) returns the vector: " << sv << std::endl;
    if (sv != sj::vec<float, 2>({ 9.0f, 12.0f })) { --rtn; }

    lv = { 6.0f, 8.0f }; // a 3,4,5 vector
    sv = lv.lengthen (15.0f);
    std::cout << "lv: " << lv << " lv.lengthen(15.0f) returns the vector: " << sv << std::endl;
    if (sv != sj::vec<float, 2>({ 15.0f, 20.0f })) { --rtn; }

    sj::vec<float> fv1 = { 0.0f,0.0f,0.0f };
    float vmult = 0.045f;
    std::cout << "Before multiply 0 vector by " << vmult << " it is " << fv1;
    fv1 *= vmult;
    std::cout << " and after: " << fv1 << std::endl;
    if (fv1 != sj::vec<float>({ 0.0f,0.0f,0.0f })) { --rtn; }

    sj::vec<float, 4> avec1 = { 1, 0, 0, 0 };
    sj::vec<float, 4> avec2 = { 0, 0, 1, 0 };
    std::cout << "angle (degrees) between " << avec1 << " and " << avec2 << " is "
              << (avec1.angle(avec2) * sj::mathconst<float>::rad2deg)
              << " or " << (avec2.angle(avec1) * sj::mathconst<float>::rad2deg) << std::endl;

    sj::vec<float, 3> testvec = {1, 0, 0};
    sj::vec<float, 3> othervec = {0.707, 0.707, 0};
    float tv_ov = testvec.angle(othervec, sj::vec<float>{0, 0, 1});
    std::cout << othervec << " is at angle " << tv_ov << " wrt " << testvec << " around the uz axis" << std::endl;
    if (tv_ov < 0.0f) { --rtn; }
    float ov_tv = othervec.angle(testvec, sj::vec<float>{0, 0, 1});
    std::cout << testvec << " is at angle " << ov_tv << " wrt " << othervec << " around the uz axis" << std::endl;
    if (ov_tv > 0.0f) { --rtn; }

    return rtn;
}
