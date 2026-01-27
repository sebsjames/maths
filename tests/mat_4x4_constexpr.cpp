#include <iostream>
#include <array>
#include <cmath>
#include <sm/constexpr_math>
#include <sm/vec>
#include <sm/quaternion>
#include <sm/mat>

constexpr void setMatrixSequence (sm::mat<float, 4>& tm)
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
    tm.arr[9] = 9;
    tm.arr[10] = 10;
    tm.arr[11] = 11;
    tm.arr[12] = 12;
    tm.arr[13] = 13;
    tm.arr[14] = 14;
    tm.arr[15] = 15;
}

constexpr int do_test()
{
    int rtn = 0;

    // Test assignment
    sm::mat<float, 4> tm1;
    setMatrixSequence (tm1);

    sm::mat<float, 4> tm2 = tm1;
    for (unsigned int i = 0; i<16; ++i) {
        if (tm2.arr[i] != (float)i) {
            ++rtn;
        }
    }

    tm2 = tm1;
    for (unsigned int i = 0; i<16; ++i) {
        if (tm2.arr[i] != (float)i) {
            ++rtn;
        }
    }

    // Test multiplication
    sm::mat<float, 4> mult1;
    setMatrixSequence (mult1);

    sm::mat<float, 4> mult2;
    mult2.arr[0] = 15;
    mult2.arr[1] = 14;
    mult2.arr[2] = 13;
    mult2.arr[3] = 12;
    mult2.arr[4] = 11;
    mult2.arr[5] = 10;
    mult2.arr[6] = 9;
    mult2.arr[7] = 8;
    mult2.arr[8] = 7;
    mult2.arr[9] = 6;
    mult2.arr[10] = 5;
    mult2.arr[11] = 4;
    mult2.arr[12] = 3;
    mult2.arr[13] = 2;
    mult2.arr[14] = 1;
    mult2.arr[15] = 0;

    sm::mat<float, 4> mult3 = mult1 * mult2;

    if (mult3.arr[0] != 304
        || mult3.arr[1] != 358
        || mult3.arr[2] != 412
        || mult3.arr[3] != 466
        || mult3.arr[4] != 208
        || mult3.arr[5] != 246
        || mult3.arr[6] != 284
        || mult3.arr[7] != 322
        || mult3.arr[8] != 112
        || mult3.arr[9] != 134
        || mult3.arr[10] != 156
        || mult3.arr[11] != 178
        || mult3.arr[12] != 16
        || mult3.arr[13] != 22
        || mult3.arr[14] != 28
        || mult3.arr[15] != 34
        ) {
        ++rtn;
    }

    mult1 *= mult2;

    if (mult1.arr[0] != 304
        || mult1.arr[1] != 358
        || mult1.arr[2] != 412
        || mult1.arr[3] != 466
        || mult1.arr[4] != 208
        || mult1.arr[5] != 246
        || mult1.arr[6] != 284
        || mult1.arr[7] != 322
        || mult1.arr[8] != 112
        || mult1.arr[9] != 134
        || mult1.arr[10] != 156
        || mult1.arr[11] != 178
        || mult1.arr[12] != 16
        || mult1.arr[13] != 22
        || mult1.arr[14] != 28
        || mult1.arr[15] != 34
        ) {
        ++rtn;
    }

    // Test 4x4 determinant
    sm::vec<float, 16> fourfour = { 2.0f, 7.0f, 5.0f, 6.0f, 8.0f, 1.0f, 3.0f, 6.0f, 2.0f, 8.0f, -1.0f, 7.0f, 7.0f, 0.0f, 1.0f, 7.0f };
    float det_td2 = sm::mat<float, 4>::determinant (fourfour);
    if (det_td2 != 816.0f) {
        ++rtn;
    }

    // Test matrix inversion
    sm::mat<float, 4> mult4;
    mult4.arr[0] = 15;
    mult4.arr[1] = 17;
    mult4.arr[2] = 0;
    mult4.arr[3] = 0;
    mult4.arr[4] = 2;
    mult4.arr[5] = 10;
    mult4.arr[6] = 0;
    mult4.arr[7] = 0;
    mult4.arr[8] = 0;
    mult4.arr[9] = 0;
    mult4.arr[10] = 5;
    mult4.arr[11] = 4;
    mult4.arr[12] = 0;
    mult4.arr[13] = 0;
    mult4.arr[14] = 1;
    mult4.arr[15] = 0;

    sm::mat<float, 4> mult4inv = mult4.inverse();

    std::array<float, 4> v1 = {1,2,3,4};
    std::array<float, 4> v2;
    std::array<float, 4> v3;
    v2 = mult4 * v1;
    v3 = mult4inv * v2;

    float esum = sm::cem::abs(v1[0]-v3[0])
        + sm::cem::abs(v1[1]-v3[1])
        + sm::cem::abs(v1[2]-v3[2])
        + sm::cem::abs(v1[3]-v3[3]);

    if (esum > 1e-5) {
        //std::cout << "Inverse failed to re-create the vector" << std::endl;
        ++rtn;
    }

    // test matrix times vec<T,4> multiplication  std::array = mat * sm::vec
    sm::vec<float, 4> v4 = {1,0,0,0};
    std::array<float, 4> r = mult4 * v4;
    if ((r[0]==15 && r[1]==17 && r[2]==0 && r[3]==0) == false) {
        ++rtn;
    }

    sm::mat<float, 4> mult4inv_copy = mult4inv;
    if (mult4inv_copy != mult4inv) { ++rtn; }

    mult4inv_copy.set_identity();
    if (mult4inv_copy[0] != 1.0f) { ++rtn; }
    sm::vec<float, 4> r0 = mult4inv_copy.row(0);
    if (r0[0] != 1.0f) { ++rtn; }
    sm::vec<float, 4> c0 = mult4inv_copy.col(0);
    if (c0[0] != 1.0f) { ++rtn; }

    mult4inv_copy.translate (sm::vec<float, 3>{1, 0, 0});
    mult4inv_copy.translate (std::array<float, 3>{-1, 0, 0});
    mult4inv_copy.translate (0.0f, 0.0f, 0.0f);
    if (mult4inv_copy[0] != 1.0f) { ++rtn; }

    mult4inv_copy.perspective (25.0f, 2.0f, 0.1f, 10.0f);
    const sm::vec<float, 2> lb = { -4, -5 };
    const sm::vec<float, 2>& rt = { 4, 5 };
    mult4inv_copy.orthographic (lb, rt, 0.1f, 10.0f);
    mult4inv_copy.set_identity();
    if (mult4inv_copy[0] != 1.0f) { ++rtn; }

    sm::quaternion<float> q_f;
    sm::quaternion<double> q_d;
    mult4inv_copy.rotate (q_f);
    mult4inv_copy.rotate (q_d);
    mult4inv_copy.set_identity();
    if (mult4inv_copy[0] != 1.0f) { ++rtn; }

    return rtn;
}

int main()
{
    constexpr int rtn = do_test();
    if constexpr (rtn == 0) { std::cout << "Success\n"; }
    return rtn;
}
