#include <iostream>
#include <array>
#include <sm/mat>

void setMatrixSequence (sm::mat<float, 3>& tm)
{
    tm.arr[0] = 0;
    tm.arr[1] = 1;
    tm.arr[2] = 2;
    tm.arr[3] = 3;
    tm.arr[4] = 4;
    tm.arr[5] = 5;
    tm.arr[6] = 6;
    tm.arr[7] = 7;
    tm.arr[8] = 8;
}

int main()
{
    int rtn = 0;

    // Test assignment
    sm::mat<float, 3> tm1;
    setMatrixSequence (tm1);
    sm::mat<float, 3> tm2 = tm1;
    std::cout << "After assignment:\n" << tm2 << std::endl;
    for (unsigned int i = 0; i<9; ++i) {
        if (tm2.arr[i] != (float)i) {
            ++rtn;
        }
    }

    // Test 2x2 determinant
    sm::mat<float, 2> td22 = { 1.0f, 4.0f, 1.0f, 5.0f };
    float det_td = td22.determinant();
    std::cout << "Determinant = " << det_td << " (expect 1)" << std::endl;
    if (det_td != 1.0f) { ++rtn; }

    // Test 3x3 determinant
    sm::mat<float, 3> td;
    sm::vec<float, 9> threethree = { 1.0f, 0.0f, 2.0f, 1.0f, 1.0f, 3.5f, 3.0f, 2.0f, 120.0f };
    float det_td2 = td.determinant (threethree);
    std::cout << "Determinant = " << det_td2 << " (expect 111)" << std::endl;
    if (det_td2 != 111.0f) { ++rtn; }

    // Test matrix inversion
    sm::mat<float, 3> mi;
    mi.arr[0] = -1;
    mi.arr[1] = 2 ;
    mi.arr[2] = 3;
    mi.arr[3] = -2;
    mi.arr[4] = 1;
    mi.arr[5] = 4;
    mi.arr[6] = 2;
    mi.arr[7] = 1;
    mi.arr[8] = 5; // This is Sal's example from Kahn academy!

    sm::mat<float, 3> miinv = mi.inverse();
    std::cout << "mi\n" << mi << std::endl;
    std::cout << "mi.inverse():\n" << miinv << std::endl;

    // Test multiplication
    sm::mat<float, 3> mult1;
    setMatrixSequence (mult1);
    std::cout << "mult1\n" << mult1 << std::endl;

    sm::mat<float, 3> mult2;
    mult2.arr[0] = 15;
    mult2.arr[1] = 14;
    mult2.arr[2] = 13;
    mult2.arr[3] = 12;
    mult2.arr[4] = 11;
    mult2.arr[5] = 10;
    mult2.arr[6] = 9;
    mult2.arr[7] = 8;
    mult2.arr[8] = 7;
    std::cout << "mult2\n" << mult2 << std::endl;

    sm::mat<float, 3> mult3 = mult1 * mult2;
    std::cout << "mult1 * mult2 =\n" << mult3 << std::endl;

    // do we need to support mat * array? It's not well posed.
    //sm::mat<float, 3> mult3alt = mult1 * mult2.arr;
    //std::cout << "mult1 * mult2.arr =\n" << mult3alt << std::endl;

    sm::mat<float, 3> mult2_t = mult2;
    mult2_t.transpose_inplace();
    std::cout << "mult2 transposed =\n" << mult2_t << std::endl;

    if (mult3.arr[0] != 120
        || mult3.arr[1] != 162
        || mult3.arr[2] != 204
        || mult3.arr[3] != 93
        || mult3.arr[4] != 126
        || mult3.arr[5] != 159
        || mult3.arr[6] != 66
        || mult3.arr[7] != 90
        || mult3.arr[8] != 114
        ) {
        ++rtn;
    }
    sm::mat<float, 3> mult1save = mult1;
    mult1 *= mult2;
    std::cout << "mult1 *= mult2 gives\n" << mult1 << std::endl;
    mult1 = mult1save; // tests copy
    mult1 *= mult2;
    std::cout << "mult1 *= mult2 gives\n" << mult1 << std::endl;

    if (mult1.arr[0] != 120
        || mult1.arr[1] != 162
        || mult1.arr[2] != 204
        || mult1.arr[3] != 93
        || mult1.arr[4] != 126
        || mult1.arr[5] != 159
        || mult1.arr[6] != 66
        || mult1.arr[7] != 90
        || mult1.arr[8] != 114
        ) {
        ++rtn;
    }

    // Test copy
    sm::mat<double, 3> md1;
    md1.arr[0] = 0;
    md1.arr[1] = 1;
    md1.arr[2] = 2;
    md1.arr[3] = 3;
    md1.arr[4] = 4;
    md1.arr[5] = 5;
    md1.arr[6] = 6;
    md1.arr[7] = 7;
    md1.arr[8] = 8;

    sm::mat<double, 3> md2 = md1;
    if (md2 != md1) { ++rtn; }

    // test creation
    constexpr sm::mat<double, 3> zmat = {0.0};
    for (int i = 0; i < 9; ++i) {
        if (zmat[i] != 0.0) {
            throw std::runtime_error ("zero mat not zero");
        }
    }
    constexpr sm::mat<double, 3> idmat = {};
    for (int i = 0; i < 9; ++i) {
        if ((i == 0 || i == 4 || i == 8) && idmat[i] != 1.0) {
            throw std::runtime_error ("id mat not id");
        }
        if ((i != 0 && i != 4 && i != 8) && idmat[i] != 0.0) {
            throw std::runtime_error ("id mat not id");
        }
    }

    const sm::mat<double, 3> c1{}; // Yields identity matrix
    if (c1 != idmat) { ++rtn; }

    const sm::mat<double, 3> c2{0.0}; // Yields zero matrix
    if (c2 != zmat) { ++rtn; }

    const sm::mat<double, 3> c3{{}}; // Yields zero matrix
    if (c3 != zmat) { ++rtn; }

    const sm::mat<double, 3> c4({}); // Yields zero matrix?
    if (c4 != zmat) { ++rtn; }

    const sm::mat<double, 3> c5; // Yields id matrix?
    if (c5 != idmat) { ++rtn; }

    return rtn;
}
