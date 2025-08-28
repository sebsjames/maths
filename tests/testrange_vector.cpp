#include <iostream>
#include <sm/vec>
#include <sm/range>

int main()
{
    int rtn = 0;

    sm::range<sm::vec<float,3>> r(sm::vec<float,3>{1, 2, 3}, sm::vec<float,3>{45, 67, 93});
    std::cout << "A vector range is " << r << std::endl;

    std::vector<sm::vec<float, 2>> vectorvec = { {1,0}, {0,1}, {2,2} };
    sm::range<sm::vec<float, 2>> r2 = sm::range<sm::vec<float, 2>>::get_from (vectorvec);
    std::cout << "Vector range from get_from: " << r2 << std::endl;

    sm::range<sm::vec<float, 3>> r3 (sm::range_init::for_search);
    std::cout << "Vector range after search init: " << r3 << std::endl;

    r3.update (sm::vec<float>{3, 3, 3});
    std::cout << "Vector range after one update: " << r3 << std::endl;

    r3.update (sm::vec<float>{0, 0, 0});
    std::cout << "Vector range after two updates: " << r3 << std::endl;
    std::cout << "Vector range mid after two updates: " << r3.mid() << std::endl;
    sm::vec<float> m = r3.mid();
    if (m[0] != 1.5f || m[1] != 1.5f || m[2] != 1.5f) {
        --rtn;
    }

    r3.update (sm::vec<float>{-3, -3, -3});
    std::cout << "Vector range after three updates: " << r3 << std::endl;
    std::cout << "Vector range mid after three updates: " << r3.mid() << std::endl;
    m = r3.mid();
    if (m[0] != 0.0f || m[1] != 0.0f || m[2] != 0.0f) {
        --rtn;
    }

    std::cout << "Test " << (rtn == 0 ? "Passed" : "Failed") << std::endl;
    return rtn;
}
