/*
 * An example hexgrid
 */

#include <iostream>
#include <vector>
#include <cmath>

import sm.hexgrid;

int main()
{
    // Create a HexGrid to show in the scene.
    sm::hexgrid hg(0.01f, 3.0f, 0.0f);
    // Hexes outside the circular boundary will all be discarded.
    hg.setCircularBoundary (0.6f);
    std::cout << "Number of pixels in grid:" << hg.num() << std::endl;
}
