#include <iostream>
#include <limits>
#include <sj/grid>

int main()
{
    int rtn = 0;

    sj::vec<float, 2> dx = { 1, 1 };
    sj::vec<float, 2> offset = { 0, 0 };
    sj::griddomainwrap wrap = sj::griddomainwrap::none;

    sj::grid<int, float> g_bltr(4, 2, dx, offset, wrap, sj::gridorder::bottomleft_to_topright);
    sj::grid<int, float> g_tlbr(4, 2, dx, offset, wrap, sj::gridorder::topleft_to_bottomright);
    sj::grid<int, float> g_bltrc(4, 2, dx, offset, wrap, sj::gridorder::bottomleft_to_topright_colmaj);
    sj::grid<int, float> g_tlbrc(4, 2, dx, offset, wrap, sj::gridorder::topleft_to_bottomright_colmaj);

    std::cout << "grid g_bltr extents: " << g_bltr.extents() << std::endl;
    std::cout << "grid g_bltrc extents: " << g_bltrc.extents() << std::endl;

    auto ext = g_bltr.extents();
    // Test extents are as wide as width:
    if (ext[1] - ext[0] != g_bltr.width()) {
        --rtn;
    }
    // And as high as height
    if (ext[3] - ext[2] != g_bltr.height()) {
        --rtn;
    }
    // Area of pixels should be 8
    if (g_bltr.area_of_pixels() != 8.0f) {
        --rtn;
    }
    // Area should be 3
    if (g_bltr.area() != 3.0f) {
        --rtn;
    }

    std::cout << "grid g_tlbr extents: " << g_tlbr.extents() << std::endl;
    std::cout << "grid g_tlbrc extents: " << g_tlbrc.extents() << std::endl;

    // BLTR extents should be same whether row or col major
    if (g_bltr.extents() != g_bltrc.extents()
        || g_tlbr.extents() != g_tlbrc.extents()) {
        --rtn;
    }

    std::cout << "grid centre: " << g_bltr.centre() << std::endl;
    std::cout << "grid centre: " << g_bltrc.centre() << std::endl;
    std::cout << "grid centre: " << g_tlbr.centre() << std::endl;
    std::cout << "grid centre: " << g_tlbrc.centre() << std::endl;

    if (g_bltr.centre() != g_bltrc.centre()
        || g_bltr.centre() != g_tlbr.centre()
        || g_bltr.centre() != g_tlbrc.centre()) {
        --rtn;
    }

    // Show coords for bltr columnar (inspect these)
    std::cout << "BLTR colmaj:\n";
    for (int i = 1; i < 8; i+=2) {
        std::cout << g_bltrc[i] << ", ";
    }
    std::cout << std::endl;
    for (int i = 0; i < 8; i+=2) {
        std::cout << g_bltrc[i] << ", ";
    }
    std::cout << std::endl;

    std::cout << "BLTR rowmaj:\n";
    for (int i = 4; i < 8; i+=1) {
        std::cout << g_bltr[i] << ", ";
    }
    std::cout << std::endl;
    for (int i = 0; i < 4; i+=1) {
        std::cout << g_bltr[i] << ", ";
    }
    std::cout << std::endl;

    std::cout << "TLBR rowmaj:\n";
    for (int i = 0; i < 4; i+=1) {
        std::cout << g_tlbr[i] << ", ";
    }
    std::cout << std::endl;
    for (int i = 4; i < 8; i+=1) {
        std::cout << g_tlbr[i] << ", ";
    }
    std::cout << std::endl;

    std::cout << "TLBR colmaj:\n";
    for (int i = 0; i < 8; i+=2) {
        std::cout << g_tlbrc[i] << ", ";
    }
    std::cout << std::endl;
    for (int i = 1; i < 8; i+=2) {
        std::cout << g_tlbrc[i] << ", ";
    }
    std::cout << std::endl;

    return rtn;
}
