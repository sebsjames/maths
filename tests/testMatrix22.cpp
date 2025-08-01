#include <iostream>
#include <array>
#include <sm/mathconst>
#include <sm/vec>
#include <sm/mat22>

void setMatrixSequence (sm::mat22<float>& tm)
{
    tm.mat = { 0, 1, 2, 3 };
}

int main()
{
    int rtn = 0;

    // Test assignment
    sm::mat22<float> tm1;
    setMatrixSequence (tm1);
    sm::mat22<float> tm2 = tm1;
    std::cout << "After assignment:\n" << tm2 << std::endl;
    for (unsigned int i = 0; i<4; ++i) {
        if (tm2.mat[i] != (float)i) {
            ++rtn;
        }
    }

    // Test determinant
    sm::mat22<float> tt;
    tt.mat = { 1.0f, 4.0f, 1.0f, 5.0f };
    float det_td2 = tt.determinant();
    std::cout << "Determinant = " << det_td2 << " (expect 1)" << std::endl;
    if (det_td2 != 1.0f) { ++rtn; }

    // Test matrix inversion
    sm::mat22<float> mi;
    mi.mat = { -1, 2, 3, -2 };

    sm::mat22<float> miinv = mi.inverse();
    std::cout << "mi\n" << mi << std::endl;
    std::cout << "mi.inverse():\n" << miinv << std::endl;

    // Test multiplication
    sm::mat22<float> mult1;
    setMatrixSequence (mult1);
    std::cout << "mult1\n" << mult1 << std::endl;

    sm::mat22<float> mult2;
    mult2.mat = { 5, 4, 3, 2 };
    std::cout << "mult2\n" << mult2 << std::endl;

    sm::mat22<float> mult3 = mult1 * mult2;
    std::cout << "mult1 * mult2 =\n" << mult3 << std::endl;

    sm::mat22<float> mult3alt = mult1 * mult2.mat;
    std::cout << "mult1 * mult2.mat =\n" << mult3alt << std::endl;

    sm::mat22<float> mult2_t = mult2;
    mult2_t.transpose_inplace();
    std::cout << "mult2 transposed =\n" << mult2_t << std::endl;

    if (mult3.mat[0] != 8 || mult3.mat[1] != 17
        || mult3.mat[2] != 4|| mult3.mat[3] != 9) {
        ++rtn;
    }

    sm::mat22<float> mult1save = mult1;
    mult1 *= mult2;
    std::cout << "mult1 *= mult2 gives\n" << mult1 << std::endl;
    mult1 = mult1save;
    mult1 *= mult2.mat;
    std::cout << "mult1 *= mult2.mat gives\n" << mult1 << std::endl;

    if (mult1.mat[0] != 8 || mult1.mat[1] != 17
        || mult1.mat[2] != 4 || mult1.mat[3] != 9) {
        ++rtn;
    }

    // vec rotation
    sm::vec<double, 2> v1 = { 0.0, 0.1 };
    sm::mat22<double> rotn;
    rotn.rotate (sm::mathconst<double>::pi_over_3);
    sm::vec<double, 2> v1_rot = rotn * v1;
    std::cout << "v1: " << v1 << ", rotated pi/3 is: "  << v1_rot << std::endl;

    rotn.rotate (sm::mathconst<double>::two_pi_over_3);
    v1_rot = rotn * v1;
    std::cout << "v1: " << v1 << ", rotated 2pi/3 is: "  << v1_rot << std::endl;

    // test creation
    constexpr sm::mat22<double> zmat = {0.0};
    if constexpr (zmat[0] != 0.0 || zmat[1] != 0.0 || zmat[2] != 0.0 || zmat[3] != 0.0) {
        throw std::runtime_error ("Uh oh");
    }
    constexpr sm::mat22<double> idmat = {};
    if constexpr (idmat[0] != 1.0 || idmat[1] != 0.0 || idmat[2] != 0.0 || idmat[3] != 1.0) {
        throw std::runtime_error ("Uh oh");
    }

    const sm::mat22<double> c1{}; // Yields identity matrix
    for (int i = 0; i < 4; ++i) { if (idmat[i] != c1[i]) { ++rtn; } }

    const sm::mat22<double> c2{0.0}; // Yields zero matrix
    for (int i = 0; i < 4; ++i) { if (zmat[i] != c2[i]) { ++rtn; } }

    const sm::mat22<double> c3{{}}; // Yields zero matrix
    for (int i = 0; i < 4; ++i) { if (zmat[i] != c3[i]) { ++rtn; } }

    const sm::mat22<double> c4({}); // Yields zero matrix?
    for (int i = 0; i < 4; ++i) { if (zmat[i] != c4[i]) { ++rtn; } }

    const sm::mat22<double> c5; // Yields id matrix?
    for (int i = 0; i < 4; ++i) { if (idmat[i] != c5[i]) { ++rtn; } }

    return rtn;
}
