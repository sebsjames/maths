#include <iostream>
#include <morph/random.h>

int main()
{
    morph::rand_string rs;
    std::cout << rs.get() << std::endl;

    morph::rand_string rs1(4);
    std::cout << rs1.get() << std::endl;

    morph::rand_string rs2(20, morph::chargroup::alpha);
    std::cout << rs2.get() << std::endl;

    morph::rand_string rs3(32, morph::chargroup::decimal);
    std::cout << rs3.get() << std::endl;

    morph::rand_string rs4(32, morph::chargroup::binary_truefalse);
    std::cout << rs4.get() << std::endl;

    morph::rand_string rs5(32, morph::chargroup::binary);
    std::cout << rs5.get() << std::endl;

    morph::rand_string rs6(20, morph::chargroup::alphanumericuppercase);
    std::cout << rs6.get() << std::endl;

    morph::rand_string rs7(20, morph::chargroup::alphanumericlowercase);
    std::cout << rs7.get() << std::endl;
    rs7.set_chargroup (morph::chargroup::alphauppercase);
    std::cout << rs7.get() << std::endl;

    morph::rand_string rs8(20, morph::chargroup::alphalowercase);
    std::cout << rs8.get() << std::endl;

    morph::rand_string rs9(20, morph::chargroup::hexuppercase);
    std::cout << rs9.get() << std::endl;

    morph::rand_string rs10(20, morph::chargroup::hexlowercase);
    std::cout << rs10.get(50) << std::endl;

    morph::rand_string rs11(20, morph::chargroup::alphanumeric);
    std::cout << rs11.get() << std::endl;

    return 0;
}
