/*
 * mat33 and mat22 examples
 */
#include <iostream>
#include <sm/mat>

int main()
{
    sm::mat<float, 3> mi = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
    std::cout << "mi =\n" << mi << std::endl;

    sm::mat<float, 3> mi0 = { 1,2,3,4 };
    std::cout << "mi0 =\n" << mi0 << std::endl;

    sm::mat<float, 3> mi2 = std::array<float, 9>{ 1,2,3,4,5,6,7,8,9 };
    std::cout << "mi2 =\n" << mi2 << std::endl;
    sm::mat<float, 3> mi3 = sm::vec<float, 9>{ 2,2,2,2 };
    std::cout << "mi3 =\n" << mi3 << std::endl;

    mi3 = { 4,3,2,1, 4,3,2,1, 4,3,2,1, 4,3,2,1 };
    std::cout << "mi3 reassigned =\n" << mi3 << std::endl;

    std::cout << "create {} initialized list:\n";
    sm::mat<double, 2> two_null = {0.0};
    std::cout << "{0.0} initialized list:\n" << two_null << std::endl;
    sm::mat<double, 2> two_null2 = {{}};
    std::cout << "{{}} initialized list:\n" << two_null2 << std::endl;
    sm::mat<double, 2> two_null3 = {};
    std::cout << "{} initialized list:\n" << two_null3 << std::endl;

    sm::mat<float, 2> two_mi = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
    std::cout << "two_mi =\n" << two_mi << std::endl;

    sm::mat<float, 2> two_bi{};
    std::cout << "sm::mat<float, 2> two_bi{}; gives \n" << two_bi << std::endl;
#if 0
    constexpr sm::mat<float, 2> two_mi0 = { 1,2,3,4 };
    std::cout << "two_mi0 =\n" << two_mi0 << std::endl;

    constexpr sm::mat<float, 2> two_mi2 = std::array<float, 4>{ 1,2,3,4 };
    std::cout << "two_mi2 =\n" << two_mi2 << std::endl;
    sm::mat<float, 2> two_mi3 = sm::vec<float, 4>{ 3,2,2,3 };
    std::cout << "two_mi3 =\n" << two_mi3 << std::endl;

    two_mi3 = { 4,3,2,1, 4,3,2,1, 4,3,2,1, 4,3,2,1 };
    std::cout << "two_mi3 reassigned =\n" << two_mi3 << std::endl;

    constexpr sm::mat<float, 2> ceprod = two_mi0 * two_mi2;
    std::cout << two_mi0 << "\n * \n" << two_mi2 << "\n = \n" << ceprod << std::endl;
#endif
}
