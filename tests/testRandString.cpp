#include <iostream>
#include <sj/random.h>

int main()
{
    sj::rand_string rs;
    std::cout << rs.get() << std::endl;

    sj::rand_string rs1(4);
    std::cout << rs1.get() << std::endl;

    sj::rand_string rs2(20, sj::chargroup::alpha);
    std::cout << rs2.get() << std::endl;

    sj::rand_string rs3(32, sj::chargroup::decimal);
    std::cout << rs3.get() << std::endl;

    sj::rand_string rs4(32, sj::chargroup::binary_truefalse);
    std::cout << rs4.get() << std::endl;

    sj::rand_string rs5(32, sj::chargroup::binary);
    std::cout << rs5.get() << std::endl;

    sj::rand_string rs6(20, sj::chargroup::alphanumericuppercase);
    std::cout << rs6.get() << std::endl;

    sj::rand_string rs7(20, sj::chargroup::alphanumericlowercase);
    std::cout << rs7.get() << std::endl;
    rs7.set_chargroup (sj::chargroup::alphauppercase);
    std::cout << rs7.get() << std::endl;

    sj::rand_string rs8(20, sj::chargroup::alphalowercase);
    std::cout << rs8.get() << std::endl;

    sj::rand_string rs9(20, sj::chargroup::hexuppercase);
    std::cout << rs9.get() << std::endl;

    sj::rand_string rs10(20, sj::chargroup::hexlowercase);
    std::cout << rs10.get(50) << std::endl;

    sj::rand_string rs11(20, sj::chargroup::alphanumeric);
    std::cout << rs11.get() << std::endl;

    return 0;
}
