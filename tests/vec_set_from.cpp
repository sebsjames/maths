/*
 * Test set_from(string)
 */

#include <iostream>
#include <array>
#include <sm/mathconst>
#include <sm/vec>

int main()
{
    int rtn = 0;

    sm::vec<float, 3> vf = {};
    sm::vec<double, 3> vd = {};
    sm::vec<int32_t, 3> vl = {};
    sm::vec<int64_t, 3> vll = {};
    sm::vec<uint32_t, 3> vul = {};
    sm::vec<uint64_t, 3> vull = {};

    vf.set_from   (" 1,.2,3");
    vd.set_from   (std::string(".1,.2,3"));
    vl.set_from   ("10,-2,3");
    vll.set_from  (std::string(" 1,2,-3"));
    vul.set_from  (" 1,2,3");
    vull.set_from (std::string(" 1,2,-3"));

    std::cout << "vf = " << vf << ", vd = " << vd
              << "\nvl = " << vl << ", vll = " << vll
              << "\nvul = " << vul << ", vull = " << vull << std::endl;

    if (vf[0] != std::stof ("1") || vf[1] != std::stof (".2") || vf[2] != std::stof ("3")) {
        std::cout << "vf: " << vf << std::endl;
        std::cout << "stof: " << std::stof ("1") << ", " << std::stof (".2") << ", " << std::stof ("3") << std::endl;
        --rtn;
    }
    if (vd[0] != std::stod (".1") || vd[1] != std::stod (".2") || vd[2] != std::stod ("3")) {
        std::cout << "vd: " << vd << std::endl;
        std::cout << "stod: " << std::stod (".1") << ", " << std::stod (".2") << ", " << std::stod ("3") << std::endl;
        --rtn;
    }
    if (vl[0] != std::stol ("10") || vl[1] != std::stol ("-2") || vl[2] != std::stol ("3")) {
        std::cout << "vl: " << vl << std::endl;
        std::cout << "stol: " << std::stol ("10") << ", " << std::stol ("-2") << ", " << std::stol ("3") << std::endl;
        --rtn;
    }
    if (vll[0] != std::stoll ("1") || vll[1] != std::stoll ("2") || vll[2] != std::stoll ("-3")) {
        std::cout << "vll: " << vll << std::endl;
        std::cout << "stoll: " << std::stoll ("1") << ", " << std::stoll ("2") << ", " << std::stoll ("-3") << std::endl;
        --rtn;
    }
    if (vul[0] != std::stoul ("1") || vul[1] != std::stoul ("2") || vul[2] != std::stoul ("3")) {
        std::cout << "vul: " << vul << std::endl;
        std::cout << "stoul: " << std::stoul ("1") << ", " << std::stoul ("2") << ", " << std::stoul ("3") << std::endl;
        --rtn;
    }
    if (vull[0] != std::stoull ("1") || vull[1] != std::stoull ("2") || vull[2] != std::stoull ("-3")) {
        std::cout << "vull: " << vull << std::endl;
        std::cout << "stoll: " << std::stoull ("1") << ", " << std::stoull ("2") << ", " << std::stoull ("-3") << std::endl;
        --rtn;
    }

    sm::vec<int, 4> v4;
    v4.set_from ("1,2");
    if (v4 != sm::vec<int, 4>{1,2,0,0}) {
        std::cout << v4 << " != " << sm::vec<int, 4>{1,2,0,0} << std::endl;
        --rtn;
    }

    // Empty fields are ok
    v4.set_from ("1,,3,");
    if (v4 != sm::vec<int, 4>{1,0,3,0}) {
        std::cout << v4 << " != " << sm::vec<int, 4>{1,0,3,0} << std::endl;
        --rtn;
    }

    try {
        v4.set_from ("1,f,3,");
        --rtn;
    } catch (const std::exception& e) {
        std::cout << "Expected exception: " << e.what() << std::endl;
    }

    // You can use other separators if you specify them in the 2nd arg
    v4.set_from ("1 2", " ");
    if (v4 != sm::vec<int, 4>{1,2,0,0}) {
        std::cout << v4 << " != " << sm::vec<int, 4>{1,2,0,0} << std::endl;
        --rtn;
    }

    // Separators can be multi-char
    v4.set_from ("1sep2sep3sep4", "sep");
    if (v4 != sm::vec<int, 4>{1,2,3,4}) {
        std::cout << v4 << " != " << sm::vec<int, 4>{1,2,3,4} << std::endl;
        --rtn;
    }

    sm::vec<unsigned int, 4> u4;
    u4.set_from ("1,2");
    if (u4 != sm::vec<unsigned int, 4>{1,2,0,0}) {
        std::cout << u4 << " != " << sm::vec<unsigned int, 4>{1,2,0,0} << std::endl;
        --rtn;
    }

    u4.set_from ("");
    if (u4 != sm::vec<unsigned int, 4>{0,0,0,0}) {
        std::cout << u4 << " != " << sm::vec<unsigned int, 4>{0,0,0,0} << std::endl;
        --rtn;
    }

    u4.set_from (",,,,,,,");
    if (u4 != sm::vec<unsigned int, 4>{0,0,0,0}) {
        std::cout << u4 << " != " << sm::vec<unsigned int, 4>{0,0,0,0} << std::endl;
        --rtn;
    }

    return rtn;
}
