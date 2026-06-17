// All the geometry tests in one file.
#include <iostream>

import smtest.geometry_2;
import smtest.geometry_3;
import smtest.geometry_4;
import smtest.geometry_5;
import smtest.geometry_6;
import smtest.geometry_xyz_to_latlong;

int main()
{
    int rtn = 0;

    rtn += smtest::geometry_2::main();
    rtn += smtest::geometry_3::main();
    rtn += smtest::geometry_4::main();
    rtn += smtest::geometry_5::main();
    rtn += smtest::geometry_6::main();
    rtn += smtest::geometry_xyz_to_latlong::main();

    if (rtn) {
        std::cout << "Tests FAILED" << std::endl;
    } else {
        std::cout << "All tests PASSED" << std::endl;
    }

    return rtn;
}
