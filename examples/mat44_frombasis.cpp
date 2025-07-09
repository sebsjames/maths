/*
 * Example: frombasis
 */
#include <iostream>
#include <sm/mathconst>
#include <sm/vec>
#include <sm/mat44>

int main()
{
    // All code in same type, so define a using for math consts
    using mc = sm::mathconst<float>;

    // A transformation matrix, which is initialized as the identity matrix
    sm::mat44<float> T;

    sm::vec<float> v1 = { 0, mc::one_over_root_2, mc::one_over_root_2 };
    sm::vec<float> v2 = { -1, 0, 0 };
    sm::vec<float> v3 = { mc::one_over_root_2, -mc::one_over_root_2, 0 };

    // Set our transform matrix from the basis vectors
    T.frombasis (v1, v2, v3);

    std::cout << "T set from the basis vectors\n\t" << v1<< "\n\t" << v2 << "\n\t" << v3 << " is\n" << T << std::endl;

    sm::vec<float> ux = {1,0,0};
    sm::vec<float> uy = {0,1,0};
    sm::vec<float> uz = {0,0,1};

    std::cout << "T * ux = " << T * ux << std::endl;
    std::cout << "Compare with v1 used to build T: " << v1 << std::endl;
    std::cout << "T * uy = " << T * uy << std::endl;
    std::cout << "Compare with v2 used to build T: " << v2 << std::endl;
    std::cout << "T * uz = " << T * uz << std::endl;
    std::cout << "Compare with v3 used to build T: " << v3 << std::endl;
}
