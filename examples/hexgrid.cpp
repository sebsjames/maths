/*
 * An example sj::Visual scene, containing a HexGrid.
 */

#include <iostream>
#include <vector>
#include <cmath>

#include <sj/vec>
#include <sj/hexgrid>

int main()
{
    // Create a HexGrid to show in the scene. Hexes outside the circular boundary will
    // all be discarded.
    sj::hexgrid hg(0.01f, 3.0f, 0.0f);
    hg.setCircularBoundary (0.6f);
    std::cout << "Number of pixels in grid:" << hg.num() << std::endl;

    // Make some dummy data (a sine wave) to make an interesting surface
    std::vector<float> data(hg.num(), 0.0f);
    for (unsigned int ri=0; ri<hg.num(); ++ri) {
        data[ri] = 0.05f + 0.05f * std::sin (20.0f * hg.d_x[ri]) * std::sin (10.0f * hg.d_y[ri]); // Range 0->1
    }
}
