#include <iostream>
#include <sm/cartgrid>
#include <sm/vvec>

int main()
{
    int rtn = 0;

    // A symmetric, zero-centered CartGrid
    sm::cartgrid cg(1.0f, 1.0f, 4.0f, 4.0f);     // dx, dy, span x, span y
    cg.setBoundaryOnOuterEdge();
    std::cout << "Set up " << cg.num() << " pixels in a Cartesian grid of width/depth: " << cg.width() << "/" << cg.depth() << std::endl;

    for (auto a : cg.rects) {
        std::cout << a.outputCart() << std::endl;
    }
    if (cg.num() != 25) { --rtn; }

    sm::vvec<float> vals = { 0, 1, 2, 3, 4,   5, 6, 7, 8, 9,   10, 11, 12, 13, 14,   15, 16, 17, 18, 19,   20, 21, 22, 23, 24  };
    sm::vvec<sm::vec<float, 2>> orig = {{1, 0}, {2, 0}, {1, -1}, {2, -1}};

    {
    // Move so that all destinations are within the cartgrid
    sm::vvec<sm::vec<float, 2>> actual_result = cg.shiftCoords(orig, -2, 1);       // -2 in the x direction.  Plus 1 in the y direction.
    sm::vvec<sm::vec<float, 2>> expected_result = {{-1, 1}, {0, 1}, {-1, 0}, {0, 0}};

    if (expected_result != actual_result){
        rtn -= 1;
        std::cout << "All destinations within cartgrid test FAILED." << std::endl;
        std::cout << "Expected result " << expected_result << " not equal to actual " << actual_result  << std::endl;
    }
    }
    {
    // Move so that all destinations are within the cartgrid, non-exact number of rects.
    sm::vvec<sm::vec<float, 2>> actual_result = cg.shiftCoords(orig, -2.1, 0.8);       // rounds to -2 in the x direction.  Plus 1 in the y direction.
    sm::vvec<sm::vec<float, 2>> expected_result = {{-1, 1}, {0, 1}, {-1, 0}, {0, 0}};

    if (expected_result != actual_result){
        rtn -= 1;
        std::cout << "All destinations within cartgrid (non-exact no of rects to move) test FAILED." << std::endl;
        std::cout << "Expected result " << expected_result << " not equal to actual " << actual_result  << std::endl;
    }
    }
    {
    // Move so that some destinations are outside right boundary
    sm::vvec<sm::vec<float, 2>> actual_result = cg.shiftCoords(orig, 1, 2);
    sm::vvec<sm::vec<float, 2>> expected_result = {{2, 2}, {2, 1}};

    if (expected_result != actual_result){
        rtn -= 1;
        std::cout << "Some destinations outside right boundary test FAILED." << std::endl;
        std::cout << "Expected result " << expected_result << " not equal to actual " << actual_result  << std::endl;
    }
    }
    {
    // Move so that some destinations are outside left boundary
    sm::vvec<sm::vec<float, 2>> actual_result = cg.shiftCoords(orig, -4, -1);
    sm::vvec<sm::vec<float, 2>> expected_result = {{-2, -1}, {-2, -2}};

    if (expected_result != actual_result){
        rtn -= 1;
        std::cout << "Some destinations outside left boundary test FAILED." << std::endl;
        std::cout << "Expected result " << expected_result << " not equal to actual " << actual_result  << std::endl;
    }
    }
    {
    // Move so that some destinations are outside top boundary
    sm::vvec<sm::vec<float, 2>> actual_result = cg.shiftCoords(orig, 0, 3);
    sm::vvec<sm::vec<float, 2>> expected_result = {{1, 2}, {2, 2}};

    if (expected_result != actual_result){
        rtn -= 1;
        std::cout << "Some destinations outside top boundary test FAILED." << std::endl;
        std::cout << "Expected result " << expected_result << " not equal to actual " << actual_result  << std::endl;
    }
    }
    {
    // Move so that some destinations are outside bottom boundary
    sm::vvec<sm::vec<float, 2>> actual_result = cg.shiftCoords(orig, -2, -2);
    sm::vvec<sm::vec<float, 2>> expected_result = {{-1, -2}, {0, -2}};

    if (expected_result != actual_result){
        rtn -= 1;
        std::cout << "Some destinations outside bottom boundary test FAILED." << std::endl;
        std::cout << "Expected result " << expected_result << " not equal to actual " << actual_result  << std::endl;
    }
    }
    {
    // Move so that some destinations are outside bottom corner
    sm::vvec<sm::vec<float, 2>> actual_result = cg.shiftCoords(orig, 1, -2);
    sm::vvec<sm::vec<float, 2>> expected_result = {{2, -2}};

    if (expected_result != actual_result){
        rtn -= 1;
        std::cout << "Some destinations outside bottom corner test FAILED." << std::endl;
        std::cout << "Expected result " << expected_result << " not equal to actual " << actual_result  << std::endl;
    }
    }
    {
    // Move so that all destinations are outside the cartgrid
    sm::vvec<sm::vec<float, 2>> actual_result = cg.shiftCoords(orig, 2, 1);
    sm::vvec<sm::vec<float, 2>> expected_result = {};

    if (expected_result != actual_result){
        rtn -= 1;
        std::cout << "All destinations outside the cartgrid test FAILED." << std::endl;
        std::cout << "Expected result " << expected_result << " not equal to actual " << actual_result  << std::endl;
    }
    }
    std::cout << "At end rtn is " << rtn << std::endl;
    return rtn;
}
