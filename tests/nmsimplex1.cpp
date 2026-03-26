/*
 * Test Nelder Mead Simplex algorithm on the Rosenbrock banana function
 */

#include <iostream>
#include <cmath>

import sm.vvec;
import sm.nm_simplex;

int main()
{
    // Initialise the vertices
    sm::vvec<sm::vvec<FLT>> i_vertices = {
        { 0.7, 0.0 },
        { 0.0, 0.6 },
        { -0.6, -1.0 }
    };
    // Create an optimiser object
    sm::nm_simplex<FLT> simp(i_vertices);
    // Define your Rosenbrock Banana objective function
    auto banana = [](const sm::vvec<FLT>& point) {
        FLT x = point[0];
        FLT y = point[1];
        constexpr FLT a = FLT{1};
        constexpr FLT b = FLT{100};
        FLT rtn = ((a-x)*(a-x)) + (b * (y-(x*x)) * (y-(x*x)));
        return rtn;
    };
    simp.objective = banana;
    // Set an optimization parameter
    simp.termination_threshold = std::numeric_limits<FLT>::epsilon();
    // Run the optimization
    if (!simp.run()) { std::cerr << "Objective was not set\n"; return -1; }
    // Check the final state which we expect to be 'TerminationThreshold':
    if (simp.stopreason != sm::nm_simplex_stop_reason::termination_threshold) {
        if (simp.stopreason == sm::nm_simplex_stop_reason::too_many_operations) {
            std::cerr << "The optimization stopped after too_many_operations (" << simp.too_many_operations << ")\n";
        } else {
            std::cerr << "The optimization stopped for an unknown reason\n";
        }
        return -1;
    }
    // Test our optimization end point
    sm::vvec<FLT> thebest = simp.best_vertex();
    std::cout << "FINISHED! Best approximation: (" << thebest << ") has value " << simp.best_value() << std::endl;
    if (std::abs(thebest[0] - FLT{1}) < FLT{1e-3} && std::abs(thebest[1] - FLT{1}) < FLT{1e-3}) {
        std::cout << "Test success" << std::endl;
        return 0;
    }
    std::cout << "Test failure" << std::endl;
    return -1;
}
