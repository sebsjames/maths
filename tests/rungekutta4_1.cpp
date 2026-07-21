#include <iostream>
#include <cmath>

import sm.rungekutta4;
import sm.vec;
import sm.vvec;

int main()
{
    int rtn = 0;

    // Scalar ODE: dx/dt = -x, analytic solution x(t) = x0 * exp(-t)
    {
        sm::rungekutta4<double> rk ([](const double&, const double& x) { return -x; }, 1.0, 0.0, 0.001);
        for (int i = 0; i < 1000; ++i) { rk.step(); }
        double expected = std::exp (-rk.t);
        if (std::abs (rk.x - expected) > 1e-6) {
            std::cerr << "Scalar RK4 failed: got " << rk.x << " expected " << expected << std::endl;
            --rtn;
        }
    }

    // Fixed-size system of 2 ODEs (simple harmonic motion): dx/dt = v, dv/dt = -x
    // analytic solution with x(0) = 1, v(0) = 0 is x(t) = cos(t), v(t) = -sin(t)
    {
        using State = sm::vec<double, 2>;
        sm::rungekutta4<double, State> rk (
            [](const double&, const State& x) { return State{ x[1], -x[0] }; },
            State{ 1.0, 0.0 }, 0.0, 0.001);
        for (int i = 0; i < 1000; ++i) { rk.step(); }
        double expected_x = std::cos (rk.t);
        double expected_v = -std::sin (rk.t);
        if (std::abs (rk.x[0] - expected_x) > 1e-6 || std::abs (rk.x[1] - expected_v) > 1e-6) {
            std::cerr << "sm::vec RK4 failed: got " << rk.x << " expected (" << expected_x << ", " << expected_v << ")" << std::endl;
            --rtn;
        }
    }

    // Arbitrary-size system of ODEs (5 independent exponential decays with different rates)
    {
        using State = sm::vvec<double>;
        State rate = { 0.1, 0.5, 1.0, 2.0, 3.0 };
        sm::rungekutta4<double, State> rk (
            [rate](const double&, const State& x) { return x * rate * -1.0; },
            State{ 1.0, 1.0, 1.0, 1.0, 1.0 }, 0.0, 0.001);
        sm::vvec<State> traj = rk.integrate (1000);
        for (std::size_t i = 0; i < rate.size(); ++i) {
            double expected = std::exp (-rate[i] * rk.t);
            if (std::abs (traj[1000][i] - expected) > 1e-6) {
                std::cerr << "sm::vvec RK4 failed on element " << i << ": got " << traj[1000][i] << " expected " << expected << std::endl;
                --rtn;
            }
        }
        if (traj.size() != 1001) { --rtn; }
    }

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");
    return rtn;
}
