/*
 * Example: frombasis
 */
#include <iostream>
#include <sm/mathconst>
#include <sm/vec>
#include <sm/mat>

int main()
{
    // All code in same type, so define a using for math consts
    using mc = sm::mathconst<float>;

    // A transformation matrix, which is initialized as the identity matrix
    sm::mat<float, 4> T;

    sm::vec<float> v1 = { 0, mc::one_over_root_2, mc::one_over_root_2 };
    sm::vec<float> v2 = { -1, 0, 0 };
    sm::vec<float> v3 = { mc::one_over_root_2, -mc::one_over_root_2, 0 };

    // Set our transform matrix from the basis vectors
    T.frombasis_inplace (v1, v2, v3);

    std::cout << "T set from the basis vectors\n\t" << v1<< "\n\t" << v2 << "\n\t" << v3 << " is\n" << T << std::endl;

    std::cout << "T * ux = " << T * sm::vec<float>::ux() << std::endl;
    std::cout << "Compare with v1 used to build T: " << v1 << std::endl;
    std::cout << "T * uy = " << T * sm::vec<float>::uy() << std::endl;
    std::cout << "Compare with v2 used to build T: " << v2 << std::endl;
    std::cout << "T * uz = " << T * sm::vec<float>::uz() << std::endl;
    std::cout << "Compare with v3 used to build T: " << v3 << std::endl;

    // static frombasis that returns a mat44
    sm::vec<double> bx = { 0.707f, 0.707f, 0.0f };
    sm::vec<double> by = { -0.707f, 0.707f, 0.0f };
    sm::vec<double> bz = { 0, 0, 1 };
    sm::mat<double, 4> mfb = sm::mat<double, 4>::frombasis (bx, by, bz);

    std::cout << "\nWith matrix\n\n" << mfb << ",\n\n"
              << sm::vec<double>::ux() << " transforms to " << mfb * sm::vec<double>::ux() << std::endl
              << sm::vec<double>::uy() << " transforms to " << mfb * sm::vec<double>::uy() << std::endl
              << sm::vec<double>::uz() << " transforms to " << mfb * sm::vec<double>::uz() << std::endl
              << " and (1,2,3) transforms to " << mfb * sm::vec<double>{1,2,3} << std::endl;
}
