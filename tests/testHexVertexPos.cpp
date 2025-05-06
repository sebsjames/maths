#include <iostream>
#include <sj/hex>

int main()
{
    int r = 0;
    int g = 0;
    float d = 2.0f;
    unsigned int idx = 0;
    sj::hex h(idx, d, r, g);

    sj::vec<float, 2> vN = h.get_vertex_coord (HEX_VERTEX_POS_N);
    sj::vec<float, 2> vNE = h.get_vertex_coord (HEX_VERTEX_POS_NE);
    sj::vec<float, 2> vSE = h.get_vertex_coord (HEX_VERTEX_POS_SE);
    sj::vec<float, 2> vS = h.get_vertex_coord (HEX_VERTEX_POS_S);
    sj::vec<float, 2> vSW = h.get_vertex_coord (HEX_VERTEX_POS_SW);
    sj::vec<float, 2> vNW = h.get_vertex_coord (HEX_VERTEX_POS_NW);

    // Output for user
    std::cout << "hex centre: (" << h.x << "," << h.y << ")" << std::endl;
    std::cout << "hex vertex N : " << vN << std::endl;
    std::cout << "hex vertex NE: " << vNE << std::endl;
    std::cout << "hex vertex SE: " << vSE << std::endl;
    std::cout << "hex vertex S : " << vS << std::endl;
    std::cout << "hex vertex SW: " << vSW << std::endl;
    std::cout << "hex vertex NW: " << vNW << std::endl;


    float vto_ne = d/(2.0f * sj::mathconst<float>::sqrt_of_3);
    // Test the numbers (non-exhaustive)
    if (vN[0] == 0.0f && vNE[1] == vto_ne && vSW[1] == -vto_ne) {
        std::cout << "PASS" << std::endl;
        return 0;
    }

    std::cout << "FAIL" << std::endl;
    return 1;
}
