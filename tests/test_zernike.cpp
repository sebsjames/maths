#include <iostream>
#include <sm/algo>

int main()
{
    // Zernike radial polynomial, constexpr
    constexpr double res = sm::algo::zern_radial_poly (4u, 2, 0.4);
    std::cout << "result = " << res << std::endl;

    return 0;
}
