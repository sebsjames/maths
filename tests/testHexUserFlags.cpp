#include <iostream>
#include <utility>
#include <sm/hex>

int main()
{
    int rtn = 0;

    int r = 0;
    int g = 0;
    float d = 2.0f;
    unsigned int idx = 0;
    sm::hex h(idx, d, r, g);

    std::cout << "User flags 0-3: " << h.getUserFlag(0) << "," << h.getUserFlag(1)
         << "," << h.getUserFlag(2) << "," << h.getUserFlag(3) << " (init)" << std::endl;
    if (h.getUserFlag(2) == true) {
        rtn -= 1;
    }

    h.setUserFlag(2);
    std::cout << "User flags 0-3: " << h.getUserFlag(0) << "," << h.getUserFlag(1)
         << "," << h.getUserFlag(2) << "," << h.getUserFlag(3) << " (set)" << std::endl;
    if (h.getUserFlag(2) == false) {
        rtn -= 1;
    }

    h.unsetUserFlag(2);
    std::cout << "User flags 0-3: " << h.getUserFlag(0) << "," << h.getUserFlag(1)
         << "," << h.getUserFlag(2) << "," << h.getUserFlag(3) << " (unset)" << std::endl;
    if (h.getUserFlag(2) == true) {
        rtn -= 1;
    }

    h.unsetUserFlag(2);
    std::cout << "User flags 0-3: " << h.getUserFlag(0) << "," << h.getUserFlag(1)
         << "," << h.getUserFlag(2) << "," << h.getUserFlag(3)  << " (unset again)" << std::endl;
    if (h.getUserFlag(2) == true) {
        rtn -= 1;
    }

    h.setUserFlags (HEX_USER_FLAG_0 | HEX_USER_FLAG_3);
    std::cout << "User flags 0-3: " << h.getUserFlag(0) << "," << h.getUserFlag(1)
         << "," << h.getUserFlag(2) << "," << h.getUserFlag(3) << std::endl;
    if (h.getUserFlag(0) == false || h.getUserFlag(3) == false) {
        rtn -= 1;
    }

    h.resetUserFlags();
    std::cout << "User flags 0-3: " << h.getUserFlag(0) << "," << h.getUserFlag(1)
         << "," << h.getUserFlag(2) << "," << h.getUserFlag(3) << std::endl;
    if (h.getUserFlag(0) == true || h.getUserFlag(3) == true) {
        rtn -= 1;
    }

    if (rtn != 0) {
        std::cout << "FAIL" << std::endl;
    }
    return rtn;
}
