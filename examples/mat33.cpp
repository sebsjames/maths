/*
 * mat33 and mat22 examples
 */
#include <iostream>
#include <sm/mat22>
#include <sm/mat33>

int main()
{
    sm::mat33<float> mi = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
    std::cout << "mi =\n" << mi << std::endl;

    sm::mat33<float> mi0 = { 1,2,3,4 };
    std::cout << "mi0 =\n" << mi0 << std::endl;

    sm::mat33<float> mi2 = std::array<float, 9>{ 1,2,3,4,5,6,7,8,9 };
    std::cout << "mi2 =\n" << mi2 << std::endl;
    sm::mat33<float> mi3 = sm::vec<float, 9>{ 2,2,2,2 };
    std::cout << "mi3 =\n" << mi3 << std::endl;

    mi3 = { 4,3,2,1, 4,3,2,1, 4,3,2,1, 4,3,2,1 };
    std::cout << "mi3 reassigned =\n" << mi3 << std::endl;


    sm::mat22<float> two_mi = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
    std::cout << "two_mi =\n" << two_mi << std::endl;

    sm::mat22<float> two_mi0 = { 1,2,3,4 };
    std::cout << "two_mi0 =\n" << two_mi0 << std::endl;

    sm::mat22<float> two_mi2 = std::array<float, 4>{ 1,2,3,4 };
    std::cout << "two_mi2 =\n" << two_mi2 << std::endl;
    sm::mat22<float> two_mi3 = sm::vec<float, 4>{ 3,2,2,3 };
    std::cout << "two_mi3 =\n" << two_mi3 << std::endl;

    two_mi3 = { 4,3,2,1, 4,3,2,1, 4,3,2,1, 4,3,2,1 };
    std::cout << "two_mi3 reassigned =\n" << two_mi3 << std::endl;
}
