/*
 * Test Nelder Mead Simplex algorithm on the Rosenbrock banana function.
 *
 * To see a visualized version of this program see
 * https://github.com/sebsjames/mathplot/blob/main/examples/rosenbrock.cpp
 */

#include <iostream>
#include <sm/vvec>
#include <sm/random>
#include <sm/nm_simplex>

// Here's the Rosenbrock banana function
template<typename F>
F banana (F x, F y)
{
    constexpr F a = F{1};
    constexpr F b = F{100};
    F rtn = ((a - x) * (a - x)) + (b * (y - (x * x)) * (y - (x * x)));
    return rtn;
}

int main()
{
    using F = double; // you can try with F = float, too

    // Initialise the vertices arbitrarily (here, it's 3 two-dimensional coordinates making a triangle)
    sm::rand_uniform<F> rng(F{-3}, F{3});
    sm::vvec<sm::vvec<F>> i_vertices = { {rng.get(), rng.get()},  {rng.get(), rng.get()},  {rng.get(), rng.get()} };
    // Set up the nm_simplex:
    sm::nm_simplex<F> simp(i_vertices);
    simp.objective = [](sm::vvec<F> x) { return banana<F>(x[0], x[1]); }; // objective defined as lambda
    simp.termination_threshold = std::numeric_limits<F>::epsilon();
    simp.too_many_operations = 10000;
    // Run the algorithm:
    simp.run();
    // Get the result or determine that there was an error:
    std::vector<F> bv = {};
    if (simp.stopreason == sm::nm_simplex_stop_reason::TerminationThreshold) {
        bv = simp.best_vertex();
        std::cout << "Finished in " << simp.operation_count << " operations. Best approximation at: ("
                  << bv[0] << "," << bv[1] << ") has value " << simp.best_value() << std::endl;
        // Check we got the right result (1e-3 so that this will succeed with F as float or double)
        if (std::abs (bv[0] - F{1}) < F{1e-3} && std::abs (bv[1] - F{1}) < F{1e-3}) {
            std::cout << "Nelder-Mead success!\n";
            return 0;
        } else {
            std::cout << "Nelder-Mead finished, but the result was wrong\n";
        }
    } else {
        std::cout << "Did not terminate successfully" << std::endl;
    }

    return -1;
}
