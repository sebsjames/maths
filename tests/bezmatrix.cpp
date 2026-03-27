#include <utility>
#include <iostream>
#include <fstream>
#include <limits>
#include <chrono>

import sm.bezcurve;
import sm.hexgrid;

int main()
{
    using namespace std::chrono;

    int rtn = 0;

    sm::vvec<sm::vec<float, 2>> c = {
        {9.0f,10.0f},
        {19.0f,16.0f},
        {42.0f,33.0f},
        {56.0f,47.0f}
        //{75.0f,52.0f},
        //{94.0f,59.0f},
        //{110.0f,68.0f}
    };

    sm::bezcurve<FLT, 3> cv (c);

    std::cout << "Defined a " << cv.get_order() << " nd/rd/th order curve" << std::endl;

    sm::bezcoord<FLT> bc = cv.compute_point_cubic (0.4);
    sm::bezcoord<FLT> bm = cv.compute_point_matrix (0.4);
    sm::bezcoord<FLT> bg = cv.compute_point_general (0.4);
    std::cout << "cubic method: " << bc << std::endl;
    std::cout << "matrix method: " << bm << std::endl;
    std::cout << "general method: " << bg << std::endl;

    float xdiff = std::abs(bm.x() - bg.x());
    float ydiff = std::abs(bm.y() - bg.y());
    std::cout << "mat/gen x points differ by: " << xdiff << std::endl;
    std::cout << "mat/gen y points differ by: " << ydiff << std::endl;

    float xdiff2 = std::abs(bm.x() - bc.x());
    float ydiff2 = std::abs(bm.y() - bc.y());
    std::cout << "mat/cube x points differ by: " << xdiff2 << std::endl;
    std::cout << "mat/cube y points differ by: " << ydiff2 << std::endl;

    if (xdiff < 1e-5 && ydiff < 1e-5) {
        std::cout << "General & matrix methods compute same point" << std::endl;
    } else {
        ++rtn;
    }

    // Profile matrix and general methods
    float tstep = 0.00001f;
    milliseconds m_b4 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    for (float t = 0.0f; t < 1.0f; t+=tstep) {
        cv.compute_point_matrix (t);
    }
    milliseconds m_af = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    milliseconds matrix_time = m_af - m_b4;
    std::cout << "Computed " << (1.0f/tstep) << " bezier points (matrix method) in " << matrix_time.count() << " ms" << std::endl;

    milliseconds g_b4 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    for (float t = 0.0f; t < 1.0f; t+=tstep) {
        cv.compute_point_general (t);
    }
    milliseconds g_af = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    milliseconds general_time = g_af - g_b4;
    std::cout << "Computed " << (1.0f/tstep) << " bezier points (general method) in " << general_time.count() << " ms" << std::endl;

    if (cv.get_order() < 4) {
        microseconds o_b4 = duration_cast<microseconds>(system_clock::now().time_since_epoch());
        for (float t = 0.0f; t < 1.0f; t+=tstep) {
            cv.compute_point_cubic (t);
        }
        microseconds o_af = duration_cast<microseconds>(system_clock::now().time_since_epoch());
        microseconds opt_time = o_af - o_b4;
        std::cout << "Computed " << (1.0f/tstep) << " bezier points (optimized method) in " << opt_time.count() << " us" << std::endl;
    }

    return rtn;
}
