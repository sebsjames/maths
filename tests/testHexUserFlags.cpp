#include <iostream>
#include <utility>
#include <sm/hex>

void showflags (const sm::hex& h)
{
    std::cout << "User flags 0-15: ";
    for (unsigned int i = 0; i < 16; ++i) {
        std::cout << h.getUserFlag (i) << ",";
    }
    std::cout << std::endl;
}

int main()
{
    int rtn = 0;

    int r = 0;
    int g = 0;
    float d = 2.0f;
    unsigned int idx = 0;
    sm::hex h(idx, d, r, g);

    showflags (h);
    if (h.getUserFlag(2) == true) {
        rtn -= 1;
    }

    h.setUserFlag(2);
    showflags (h);
    if (h.getUserFlag(2) == false) {
        rtn -= 1;
    }

    h.unsetUserFlag(2);
    showflags (h);
    if (h.getUserFlag(2) == true) {
        rtn -= 1;
    }

    h.unsetUserFlag(2);
    showflags (h);
    if (h.getUserFlag(2) == true) {
        rtn -= 1;
    }

    h.setUserFlags (HEX_USER_FLAG_0 | HEX_USER_FLAG_3);
    showflags (h);
    if (h.getUserFlag(0) == false || h.getUserFlag(3) == false) {
        rtn -= 1;
    }

    h.resetUserFlags();
    showflags (h);
    if (h.getUserFlag(0) == true || h.getUserFlag(3) == true) {
        rtn -= 1;
    }

    h.setUserFlags (HEX_USER_FLAG_0 | HEX_USER_FLAG_15);
    showflags (h);
    if (h.getUserFlag(0) == false || h.getUserFlag(15) == false) {
        rtn -= 1;
    }

    h.resetUserFlags();
    showflags (h);
    if (h.getUserFlag(0) == true || h.getUserFlag(15) == true) {
        rtn -= 1;
    }

    if (rtn != 0) {
        std::cout << "FAIL" << std::endl;
    }
    return rtn;
}
