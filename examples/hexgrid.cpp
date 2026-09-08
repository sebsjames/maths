/*
 * An example hexgrid
 */

#include <iostream>
#include <vector>
#include <cmath>

import sm.hexgrid;

int main()
{
    std::cout << "Point up...\n";
    {
        sm::hexgrid<float, sm::hexalign::point_up>hg(0.1f, 3.0f, 0.0f);
        hg.set_circular_boundary (0.2f);
        std::cout << "Number of pixels in grid:" << hg.num() << std::endl;
        std::cout << hg.output() <<  std::endl;
    }
    std::cout << "\n\nFlat up...\n";
    {
        sm::hexgrid<float, sm::hexalign::flat_up> hg(0.1f, 3.0f, 0.0f);
        hg.set_circular_boundary (0.2f);
        std::cout << "Number of pixels in grid:" << hg.num() << std::endl;
        std::cout << hg.output() <<  std::endl;
    }
}
