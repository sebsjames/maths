#include <iostream>
#include <complex>
import sm.mat;

int main()
{
    std::cout << "float:\n";
    using mc = sm::mathconst<float>;

    sm::mat<float, 4> m1 = {
        mc::pi, 1, 2, 3,
        4, mc::pi, 5, 6,
        7, 8, mc::pi, 9,
        10, 11, 12, 45.5
    };

    std::cout << "cout << m1:\n" << m1;
    std::cout << "cout << m1.str(8):\n" << m1.str(8);
    std::cout << "cout << m1.str(4):\n" << m1.str(4);
    std::cout << "cout << m1.str(2):\n" << m1.str(2);
    std::cout << "cout << m1.str(1):\n" << m1.str(1);

    std::cout << "cout << m1.str_arr():\n" << m1.str_arr();
    std::cout << "cout << m1.str_arr(4):\n" << m1.str_arr(4);
    std::cout << "cout << m1.str_arr(2):\n" << m1.str_arr(2);
    std::cout << "cout << m1.str_arr(1):\n" << m1.str_arr(1);

    std::cout << "\ndouble:\n";
    using mcd = sm::mathconst<double>;

    sm::mat<double, 4> m1d = {
        mcd::pi, 1, 2, 3,
        4, mcd::pi, 5, 6,
        7, 8, mcd::pi, 9,
        10, 11, 12, 45.5
    };

    std::cout << "cout << m1:\n" << m1d;
    std::cout << "cout << m1.str(8):\n" << m1d.str(8);
    std::cout << "cout << m1.str(4):\n" << m1d.str(4);
    std::cout << "cout << m1.str(2):\n" << m1d.str(2);
    std::cout << "cout << m1.str(1):\n" << m1d.str(1);

    std::cout << "cout << m1.str_arr():\n" << m1d.str_arr();
    std::cout << "cout << m1.str_arr(4):\n" << m1d.str_arr(4);
    std::cout << "cout << m1.str_arr(2):\n" << m1d.str_arr(2);
    std::cout << "cout << m1.str_arr(1):\n" << m1d.str_arr(1);

    constexpr std::complex<float> c0 = { 0.0f };
    constexpr std::complex<float> c1 = { 1.0f };
    sm::mat<std::complex<float>, 4> m1c = { c0, c1, c0, c0,  c1, c0, c0, c0,  c0, c0, -c1, c0,  c0, c0, c0, c1 };

    std::cout << "Complex mat:\n" << m1c << std::endl;

    return 0;
}
