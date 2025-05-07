#include <iostream>
#include <sm/random>

int main()
{
    sm::rand_string rs;
    std::cout << rs.get() << std::endl;

    sm::rand_string rs1(4);
    std::cout << rs1.get() << std::endl;

    sm::rand_string rs2(20, sm::chargroup::alpha);
    std::cout << rs2.get() << std::endl;

    sm::rand_string rs3(32, sm::chargroup::decimal);
    std::cout << rs3.get() << std::endl;

    sm::rand_string rs4(32, sm::chargroup::binary_truefalse);
    std::cout << rs4.get() << std::endl;

    sm::rand_string rs5(32, sm::chargroup::binary);
    std::cout << rs5.get() << std::endl;

    sm::rand_string rs6(20, sm::chargroup::alphanumericuppercase);
    std::cout << rs6.get() << std::endl;

    sm::rand_string rs7(20, sm::chargroup::alphanumericlowercase);
    std::cout << rs7.get() << std::endl;
    rs7.set_chargroup (sm::chargroup::alphauppercase);
    std::cout << rs7.get() << std::endl;

    sm::rand_string rs8(20, sm::chargroup::alphalowercase);
    std::cout << rs8.get() << std::endl;

    sm::rand_string rs9(20, sm::chargroup::hexuppercase);
    std::cout << rs9.get() << std::endl;

    sm::rand_string rs10(20, sm::chargroup::hexlowercase);
    std::cout << rs10.get(50) << std::endl;

    sm::rand_string rs11(20, sm::chargroup::alphanumeric);
    std::cout << rs11.get() << std::endl;

    return 0;
}
