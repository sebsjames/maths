#include <iostream>
#include <complex>
#include <cstdint>

import sm.quaternion;
import sm.mat;

void setMatrixSequence (sm::mat<std::complex<float>, 4>& tm)
{
    for (std::uint32_t i = 0; i < 16; ++i) {
        tm.arr[i] = std::complex<float>{static_cast<float>(i), 0.0f};
    }
}

int main()
{
    int rtn = 0;

    constexpr std::complex<float> c0 = { 0.0f };
    constexpr std::complex<float> c1 = { 1.0f };
    sm::mat<std::complex<float>, 4> m1 = { c0, c1, c0, c0,  c1, c0, c0, c0,  c0, c0, -c1, c0,  c0, c0, c0, c1 };
    std::cout << "m1:\n" << m1 << std::endl;

    // Test assignment
    sm::mat<std::complex<float>, 4> tm1;
    setMatrixSequence (tm1);
    std::cout << "tm1 after set matrix seq:\n" << tm1 << std::endl;
    sm::mat<std::complex<float>, 4> tm2 = tm1;
    std::cout << "After assignment operator:\n" << tm2 << std::endl;
    for (unsigned int i = 0; i<16; ++i) {
        if (tm2.arr[i] != std::complex<float>{static_cast<float>(i), 0.0f}) {
            ++rtn;
        }
    }
    tm2 = static_cast<sm::mat<std::complex<float>, 4>>(tm1);
    std::cout << "After second assignment:\n" << tm2 << std::endl;
    for (unsigned int i = 0; i<16; ++i) {
        if (tm2.arr[i] != std::complex<float>{static_cast<float>(i), 0.0f}) {
            ++rtn;
        }
    }


    // Test multiplication
    sm::mat<std::complex<float>, 4> mult1;
    setMatrixSequence (mult1);
    std::cout << "mult1\n" << mult1 << std::endl;

    sm::mat<std::complex<float>, 4> mult2;
    for (std::uint32_t i = 0; i < 16; ++i) {
        mult2.arr[i] = std::complex<float>{static_cast<float>(15 - i), 0.0f};
    }
    std::cout << "mult2\n" << mult2 << std::endl;

    sm::mat<std::complex<float>, 4> mult3 = mult1 * mult2;
    std::cout << "mult1 * mult2 =\n" << mult3 << std::endl;

    if (std::real(mult3.arr[0]) != 304
        || std::real(mult3.arr[1]) != 358
        || std::real(mult3.arr[2]) != 412
        || std::real(mult3.arr[3]) != 466
        || std::real(mult3.arr[4]) != 208
        || std::real(mult3.arr[5]) != 246
        || std::real(mult3.arr[6]) != 284
        || std::real(mult3.arr[7]) != 322
        || std::real(mult3.arr[8]) != 112
        || std::real(mult3.arr[9]) != 134
        || std::real(mult3.arr[10]) != 156
        || std::real(mult3.arr[11]) != 178
        || std::real(mult3.arr[12]) != 16
        || std::real(mult3.arr[13]) != 22
        || std::real(mult3.arr[14]) != 28
        || std::real(mult3.arr[15]) != 34
        ) {
        ++rtn;
    }

    mult1 *= mult2; // *= mat44
    std::cout << "mult1 *= mult2 gives\n" << mult1 << std::endl;
    if (std::real(mult1.arr[0]) != 304
        || std::real(mult1.arr[1]) != 358
        || std::real(mult1.arr[2]) != 412
        || std::real(mult1.arr[3]) != 466
        || std::real(mult1.arr[4]) != 208
        || std::real(mult1.arr[5]) != 246
        || std::real(mult1.arr[6]) != 284
        || std::real(mult1.arr[7]) != 322
        || std::real(mult1.arr[8]) != 112
        || std::real(mult1.arr[9]) != 134
        || std::real(mult1.arr[10]) != 156
        || std::real(mult1.arr[11]) != 178
        || std::real(mult1.arr[12]) != 16
        || std::real(mult1.arr[13]) != 22
        || std::real(mult1.arr[14]) != 28
        || std::real(mult1.arr[15]) != 34
        ) {
        ++rtn;
    }

    // Test 4x4 determinant
    sm::vec<std::complex<float>, 16> fourfour = { 2.0f, 7.0f, 5.0f, 6.0f, 8.0f, 1.0f, 3.0f, 6.0f, 2.0f, 8.0f, -1.0f, 7.0f, 7.0f, 0.0f, 1.0f, 7.0f };
    std::complex<float> det_td2 = sm::mat<std::complex<float>, 4>::determinant (fourfour);
    std::cout << "Determinant = " << det_td2 << " (expect 816)" << std::endl;
    if (det_td2 != 816.0f) {
        ++rtn;
    }

    // Test matrix inversion
    sm::mat<std::complex<float>, 4> mult4;
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

    sm::mat<std::complex<float>, 4> mfourfour (fourfour);

    sm::mat<std::complex<float>, 4> m4timesarray = mult4 * mfourfour;
    std::cout << "m4timesarray\n" << m4timesarray << std::endl;


    sm::mat<std::complex<float>, 4> mult4inv = mult4.inverse();
    std::cout << "mult4\n" << mult4 << std::endl;
    std::cout << "mult4.inverse():\n" << mult4inv << std::endl;

    std::array<std::complex<float>, 4> v1 = {1,2,3,4};
    std::array<std::complex<float>, 4> v2;
    std::array<std::complex<float>, 4> v3;
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

    std::complex<float> esum = std::abs(v1[0]-v3[0])
        + std::abs(v1[1]-v3[1])
        + std::abs(v1[2]-v3[2])
        + std::abs(v1[3]-v3[3]);

    if (std::real(esum) > 1e-5f) {
        std::cout << "Inverse failed to re-create the vector" << std::endl;
        ++rtn;
    }

    // test matrix times vec<T,4> multiplication  std::array = mat * sm::vec
    sm::vec<std::complex<float>, 4> v4 = {1,0,0,0};
    std::array<std::complex<float>, 4> r = mult4 * v4;
    std::cout << " mult4 * " << v4 << ": (" << r[0] << "," << r[1] << "," << r[2] << "," << r[3] << ")\n";
    if ((std::real(r[0]) == 15.0f && std::real(r[1]) == 17.0f && std::real(r[2]) == 0.0f && std::real(r[3]) == 0.0f) == false) {
        ++rtn;
    }

    sm::mat<std::complex<float>, 4> mult4inv_copy = mult4inv;
    if (mult4inv_copy != mult4inv) { ++rtn; }

    std::cout << "Test " << (rtn ? "FAILED" : "PASSED") << std::endl;

    return rtn;
}
