#include <iostream>
#include <array>
#include <cmath>
#include <sm/vec>
#include <sm/mat44>

void setMatrixSequence (sm::mat44<float>& tm)
{
    tm.mat[0] = 0;
    tm.mat[1] = 1;
    tm.mat[2] = 2;
    tm.mat[3] = 3;
    tm.mat[4] = 4;
    tm.mat[5] = 5;
    tm.mat[6] = 6;
    tm.mat[7] = 7;
    tm.mat[8] = 8;
    tm.mat[9] = 9;
    tm.mat[10] = 10;
    tm.mat[11] = 11;
    tm.mat[12] = 12;
    tm.mat[13] = 13;
    tm.mat[14] = 14;
    tm.mat[15] = 15;
}

int main()
{
    int rtn = 0;

    // Test assignment
    sm::mat44<float> tm1;
    setMatrixSequence (tm1);
    sm::mat44<float> tm2 = tm1;
    std::cout << "After assignment:\n" << tm2 << std::endl;
    for (unsigned int i = 0; i<16; ++i) {
        if (tm2.mat[i] != (float)i) {
            ++rtn;
        }
    }
    tm2 = static_cast<sm::mat44<float>>(tm1);
    std::cout << "After second assignment:\n" << tm2 << std::endl;
    for (unsigned int i = 0; i<16; ++i) {
        if (tm2.mat[i] != (float)i) {
            ++rtn;
        }
    }
    // Test multiplication
    sm::mat44<float> mult1;
    setMatrixSequence (mult1);
    std::cout << "mult1\n" << mult1 << std::endl;

    sm::mat44<float> mult2;
    mult2.mat[0] = 15;
    mult2.mat[1] = 14;
    mult2.mat[2] = 13;
    mult2.mat[3] = 12;
    mult2.mat[4] = 11;
    mult2.mat[5] = 10;
    mult2.mat[6] = 9;
    mult2.mat[7] = 8;
    mult2.mat[8] = 7;
    mult2.mat[9] = 6;
    mult2.mat[10] = 5;
    mult2.mat[11] = 4;
    mult2.mat[12] = 3;
    mult2.mat[13] = 2;
    mult2.mat[14] = 1;
    mult2.mat[15] = 0;
    std::cout << "mult2\n" << mult2 << std::endl;

    sm::mat44<float> mult3 = mult1 * mult2;
    std::cout << "mult1 * mult2 =\n" << mult3 << std::endl;

    if (mult3.mat[0] != 304
        || mult3.mat[1] != 358
        || mult3.mat[2] != 412
        || mult3.mat[3] != 466
        || mult3.mat[4] != 208
        || mult3.mat[5] != 246
        || mult3.mat[6] != 284
        || mult3.mat[7] != 322
        || mult3.mat[8] != 112
        || mult3.mat[9] != 134
        || mult3.mat[10] != 156
        || mult3.mat[11] != 178
        || mult3.mat[12] != 16
        || mult3.mat[13] != 22
        || mult3.mat[14] != 28
        || mult3.mat[15] != 34
        ) {
        ++rtn;
    }
    auto mult1cpy = mult1;
    mult1 *= mult2; // *= mat44
    std::cout << "mult1 *= mult2 gives\n" << mult1 << std::endl;
    if (mult1.mat[0] != 304
        || mult1.mat[1] != 358
        || mult1.mat[2] != 412
        || mult1.mat[3] != 466
        || mult1.mat[4] != 208
        || mult1.mat[5] != 246
        || mult1.mat[6] != 284
        || mult1.mat[7] != 322
        || mult1.mat[8] != 112
        || mult1.mat[9] != 134
        || mult1.mat[10] != 156
        || mult1.mat[11] != 178
        || mult1.mat[12] != 16
        || mult1.mat[13] != 22
        || mult1.mat[14] != 28
        || mult1.mat[15] != 34
        ) {
        ++rtn;
    }

    mult1cpy *= mult2.mat; // *= array
    if (mult1cpy.mat[0] != 304
        || mult1cpy.mat[1] != 358
        || mult1cpy.mat[2] != 412
        || mult1cpy.mat[3] != 466
        || mult1cpy.mat[4] != 208
        || mult1cpy.mat[5] != 246
        || mult1cpy.mat[6] != 284
        || mult1cpy.mat[7] != 322
        || mult1cpy.mat[8] != 112
        || mult1cpy.mat[9] != 134
        || mult1cpy.mat[10] != 156
        || mult1cpy.mat[11] != 178
        || mult1cpy.mat[12] != 16
        || mult1cpy.mat[13] != 22
        || mult1cpy.mat[14] != 28
        || mult1cpy.mat[15] != 34
        ) {
        ++rtn;
    }


    // Test 4x4 determinant
    std::array<float, 16> fourfour = { 2.0f, 7.0f, 5.0f, 6.0f, 8.0f, 1.0f, 3.0f, 6.0f, 2.0f, 8.0f, -1.0f, 7.0f, 7.0f, 0.0f, 1.0f, 7.0f };
    float det_td2 = sm::mat44<float>::determinant (fourfour);
    std::cout << "Determinant = " << det_td2 << " (expect 816)" << std::endl;
    if (det_td2 != 816.0f) {
        ++rtn;
    }

    // Test matrix inversion
    sm::mat44<float> mult4;
    mult4.mat[0] = 15;
    mult4.mat[1] = 17;
    mult4.mat[2] = 0;
    mult4.mat[3] = 0;
    mult4.mat[4] = 2;
    mult4.mat[5] = 10;
    mult4.mat[6] = 0;
    mult4.mat[7] = 0;
    mult4.mat[8] = 0;
    mult4.mat[9] = 0;
    mult4.mat[10] = 5;
    mult4.mat[11] = 4;
    mult4.mat[12] = 0;
    mult4.mat[13] = 0;
    mult4.mat[14] = 1;
    mult4.mat[15] = 0;

    sm::mat44<float> m4timesarray = mult4 * fourfour;
    std::cout << "m4timesarray\n" << m4timesarray << std::endl;


    sm::mat44<float> mult4inv = mult4.inverse();
    std::cout << "mult4\n" << mult4 << std::endl;
    std::cout << "mult4.inverse():\n" << mult4inv << std::endl;

    std::array<float, 4> v1 = {1,2,3,4};
    std::array<float, 4> v2;
    std::array<float, 4> v3;
    v2 = mult4 * v1;
    v3 = mult4inv * v2;

    std::cout << "v1 = (" << v1[0]
              << "," << v1[1]
              << "," << v1[2]
              << "," << v1[3] << ")" << std::endl;
    std::cout << "v2 = mult4 * v1 = (" << v2[0]
              << "," << v2[1]
              << "," << v2[2]
              << "," << v2[3] << ")" << std::endl;
    std::cout << "v3 = mult4inv * v2 = (" << v3[0]
              << "," << v3[1]
              << "," << v3[2]
              << "," << v3[3] << ") (should be equal to v1)" << std::endl;

    std::cout << "v1-v3 errors: " << std::abs(v1[0]-v3[0]) << ", "
              << std::abs(v1[1]-v3[1]) << ", "
              << std::abs(v1[2]-v3[2]) << ", "
              << std::abs(v1[3]-v3[3]) << std::endl;

    float esum = std::abs(v1[0]-v3[0])
        + std::abs(v1[1]-v3[1])
        + std::abs(v1[2]-v3[2])
        + std::abs(v1[3]-v3[3]);

    if (esum > 1e-5) {
        std::cout << "Inverse failed to re-create the vector" << std::endl;
        ++rtn;
    }

    // test matrix times vec<T,4> multiplication  std::array = mat * sm::vec
    sm::vec<float, 4> v4 = {1,0,0,0};
    std::array<float, 4> r = mult4 * v4;
    std::cout << " mult4 * " << v4 << ": (" << r[0] << "," << r[1] << "," << r[2] << "," << r[3] << ")\n";
    if ((r[0]==15 && r[1]==17 && r[2]==0 && r[3]==0) == false) {
        ++rtn;
    }

    sm::mat44<float> mult4inv_copy = mult4inv;
    if (mult4inv_copy != mult4inv) { ++rtn; }

    // Test scaling
    sm::mat44<double> scaler;
    sm::vec<double, 4> v4d = { 2.0, 3.0, 4.0, 1.0 };
    sm::vec<float, 3> scale_vec = { 2.0f, 2.0f, 2.0f };
    scaler.scale (scale_vec);
    std::cout << v4d << " scaled by " << scale_vec << " = " << (scaler * v4d) << std::endl;

    constexpr float second_scale = 2.0f;
    scaler.scale (second_scale);
    std::cout << v4d << " scaled by " << scale_vec << " and then in all dims by " << second_scale  << "  = " << (scaler * v4d) << std::endl;

    sm::vec<double, 4> v4dres = scaler * v4d;
    std::cout << "v4dres: " << v4dres << std::endl;
    if (v4dres[0] != 8.0 || v4dres[1] != 12.0 || v4dres[2] != 16.0) { ++rtn; }

    scaler.scale (0.025, 0.025, 0.025);
    std::cout << v4d << " scaled by " << scale_vec << " and then in all dims by " << second_scale << " and then by 0.025f, 0.025f, 0.025f = " << (scaler * v4d) << std::endl;

    // Test translate then rotate
    sm::mat44<float> trmat;
    sm::vec<float> trans = { 1, 0, 0 };
    sm::quaternion<float> rotn (sm::vec<float>{0, 0, 1}, sm::mathconst<float>::pi_over_4);

    // these two, applied to the same trmat used to do rotate-then-translate
    trmat.translate (trans);
    trmat.rotate (rotn);

    sm::vec<float> uy = { 0, 1, 0 };
    sm::vec<float, 4> tr_res = trmat * uy;
    std::cout << "translate-then-rotate vector = " << tr_res << std::endl;

    // Recapitulate old behaviour
    sm::mat44<float> rot_then_trans;

    // these two, applied to the same trmat used to do rotate-then-translate
    // NOW they do translate then rotate
    rot_then_trans.translate (trans);
    rot_then_trans.prerotate (rotn);

    tr_res = rot_then_trans * uy;
    std::cout << "rotate-then-translate vector = " << tr_res << std::endl;

    // Test that array indexing of a const mat44 will compile
    const sm::mat44<float> m1id;
    float m1id0 = m1id[0];
    std::cout << "m1id[0] = " << m1id0 << std::endl;

    return rtn;
}
