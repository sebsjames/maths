#include <iostream>
#include <array>
#include <sm/mathconst>
#include <sm/vec>
#include <sm/mat>

void setMatrixSequence (sm::mat<float, 2>& tm)
{
    tm.arr = { 0, 1, 2, 3 };
}

int main()
{
    int rtn = 0;

    // Test assignment
    sm::mat<float, 2> tm1;
    setMatrixSequence (tm1);
    sm::mat<float, 2> tm2 = tm1;
    std::cout << "After assignment:\n" << tm2 << std::endl;
    for (unsigned int i = 0; i<4; ++i) {
        if (tm2.arr[i] != (float)i) {
            ++rtn;
        }
    }

    // Test determinant
    sm::mat<float, 2> tt;
    tt.arr = { 1.0f, 4.0f, 1.0f, 5.0f };
    float det_td2 = tt.determinant();
    std::cout << "Determinant = " << det_td2 << " (expect 1)" << std::endl;
    if (det_td2 != 1.0f) { ++rtn; }

    // Test matrix inversion
    sm::mat<float, 2> mi;
    mi.arr = { -1, 2, 3, -2 };

    sm::mat<float, 2> miinv = mi.inverse();
    std::cout << "mi\n" << mi << std::endl;
    std::cout << "mi.inverse():\n" << miinv << std::endl;

    // Test multiplication
    sm::mat<float, 2> mult1;
    setMatrixSequence (mult1);
    std::cout << "mult1\n" << mult1 << std::endl;

    sm::mat<float, 2> mult2;
    mult2.arr = { 5, 4, 3, 2 };
    std::cout << "mult2\n" << mult2 << std::endl;

    sm::mat<float, 2> mult3 = mult1 * mult2;
    std::cout << "mult1 * mult2 =\n" << mult3 << std::endl;

    sm::mat<float, 2> mult2_t = mult2;
    mult2_t.transpose_inplace();
    std::cout << "mult2 transposed =\n" << mult2_t << std::endl;

    if (mult3.arr[0] != 8 || mult3.arr[1] != 17
        || mult3.arr[2] != 4|| mult3.arr[3] != 9) {
        ++rtn;
    }

    sm::mat<float, 2> mult1save = mult1;
    mult1 *= mult2;
    std::cout << "mult1 *= mult2 gives\n" << mult1 << std::endl;
    mult1 = mult1save;
    mult1 *= mult2;
    std::cout << "mult1 *= mult2 gives\n" << mult1 << std::endl;

    if (mult1.arr[0] != 8 || mult1.arr[1] != 17
        || mult1.arr[2] != 4 || mult1.arr[3] != 9) {
        ++rtn;
    }

    // vec rotation
    sm::vec<double, 2> v1 = { 0.0, 0.1 };
    sm::mat<double, 2> rotn;
    rotn.rotate (sm::mathconst<double>::pi_over_3);
    sm::vec<double, 2> v1_rot = rotn * v1;
    std::cout << "v1: " << v1 << ", rotated pi/3 is: "  << v1_rot << std::endl;

    rotn.rotate (sm::mathconst<double>::two_pi_over_3);
    v1_rot = rotn * v1;
    std::cout << "v1: " << v1 << ", rotated 2pi/3 is: "  << v1_rot << std::endl;

    // test creation
    constexpr sm::mat<double, 2> zmat = {0.0};
    if constexpr (zmat[0] != 0.0 || zmat[1] != 0.0 || zmat[2] != 0.0 || zmat[3] != 0.0) {
        throw std::runtime_error ("Uh oh");
    }
    constexpr sm::mat<double, 2> idmat = {};
    if constexpr (idmat[0] != 1.0 || idmat[1] != 0.0 || idmat[2] != 0.0 || idmat[3] != 1.0) {
        throw std::runtime_error ("Uh oh");
    }

    const sm::mat<double, 2> c1{}; // Yields identity matrix
    for (int i = 0; i < 4; ++i) { if (idmat[i] != c1[i]) { ++rtn; } }

    const sm::mat<double, 2> c2{0.0}; // Yields zero matrix
    for (int i = 0; i < 4; ++i) { if (zmat[i] != c2[i]) { ++rtn; } }

    const sm::mat<double, 2> c3{{}}; // Yields zero matrix
    for (int i = 0; i < 4; ++i) { if (zmat[i] != c3[i]) { ++rtn; } }

    const sm::mat<double, 2> c4({}); // Yields zero matrix?
    for (int i = 0; i < 4; ++i) { if (zmat[i] != c4[i]) { ++rtn; } }

    const sm::mat<double, 2> c5; // Yields id matrix?
    for (int i = 0; i < 4; ++i) { if (idmat[i] != c5[i]) { ++rtn; } }

    return rtn;
}
