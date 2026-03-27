#include <cstdint>
#include <iostream>
#include <utility>

import sm.hex;

void showflags (const sm::hex& h)
{
    std::cout << "User flags 0-15: ";
    for (std::uint32_t i = 0; i < 16; ++i) {
        std::cout << h.get_user_flag (i) << ",";
    }
    std::cout << std::endl;
}

std::int32_t main()
{
    std::int32_t rtn = 0;

    std::int32_t r = 0;
    std::int32_t g = 0;
    float d = 2.0f;
    std::uint32_t idx = 0;
    sm::hex h(idx, d, r, g);

    showflags (h);
    if (h.get_user_flag(2) == true) {
        rtn -= 1;
    }

    h.set_user_flag(2);
    showflags (h);
    if (h.get_user_flag(2) == false) {
        rtn -= 1;
    }

    h.unset_user_flag(2);
    showflags (h);
    if (h.get_user_flag(2) == true) {
        rtn -= 1;
    }

    h.unset_user_flag(2);
    showflags (h);
    if (h.get_user_flag(2) == true) {
        rtn -= 1;
    }

    h.set_user_flags (sm::HEX_USER_FLAG_0 | sm::HEX_USER_FLAG_3);
    showflags (h);
    if (h.get_user_flag(0) == false || h.get_user_flag(3) == false) {
        rtn -= 1;
    }

    h.reset_user_flags();
    showflags (h);
    if (h.get_user_flag(0) == true || h.get_user_flag(3) == true) {
        rtn -= 1;
    }

    h.set_user_flags (sm::HEX_USER_FLAG_0 | sm::HEX_USER_FLAG_15);
    showflags (h);
    if (h.get_user_flag(0) == false || h.get_user_flag(15) == false) {
        rtn -= 1;
    }

    h.reset_user_flags();
    showflags (h);
    if (h.get_user_flag(0) == true || h.get_user_flag(15) == true) {
        rtn -= 1;
    }

    if (rtn != 0) {
        std::cout << "FAIL" << std::endl;
    }
    return rtn;
}
