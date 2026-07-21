// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 * \brief Explicit 4th order Runge-Kutta (RK4) solver for systems of ODEs
 *
 * A single template handles a scalar ODE (State = a floating point type), a fixed-size
 * system of N ODEs (State = sm::vec<T, N>) or a system of an arbitrary (run-time
 * determined) number of ODEs (State = sm::vvec<T>). This works because sm::vec and
 * sm::vvec both implement the arithmetic operators (operator+ for state + state and
 * operator* for state * scalar) required to write the RK4 update in the same form as
 * the classic scalar algorithm.
 */

module;

#include <cstdint>
#include <functional>

export module sm.rungekutta4;

export import sm.vec;
export import sm.vvec;

export namespace sm
{
    /*!
     * \brief Advance dx/dt = f(t, x) by one RK4 step
     *
     * T is the scalar type of the independent variable, the step size and the RK4
     * weighting coefficients. State is the type of the dependent variable x; it may be
     * a plain scalar (a single ODE), an sm::vec<T, N> (a fixed-size system of N ODEs)
     * or an sm::vvec<T> (a system of an arbitrary number of ODEs).
     *
     * Because sm::vec and sm::vvec both define operator+ (state + state) and operator*
     * (state * scalar), this code reads exactly like the classic 1D scalar RK4 update.
     */
    template <typename T, typename State>
    State rk4_step (const std::function<State(const T&, const State&)>& f,
                     const T& t, const State& x, const T& h)
    {
        const T half_h = h / T{2};
        const T sixth_h = h / T{6};

        State k1 = f (t, x);
        State k2 = f (t + half_h, x + k1 * half_h);
        State k3 = f (t + half_h, x + k2 * half_h);
        State k4 = f (t + h, x + k3 * h);

        return x + (k1 + k2 * T{2} + k3 * T{2} + k4) * sixth_h;
    }

    /*!
     * \brief Stateful 4th order Runge-Kutta integrator
     *
     * Holds the current state (t, x) of a system dx/dt = f(t, x), the derivative
     * function f and a step size h, so that step() and integrate() can be called
     * repeatedly to march the solution forward. See rk4_step() for the underlying
     * single-step update.
     *
     * Example (single scalar ODE, dx/dt = -x):
     *\code{.cpp}
     * sm::rungekutta4<double> rk ([](const double& t, const double& x) { return -x; }, 1.0, 0.0, 0.01);
     * rk.step();
     *\endcode
     *
     * Example (a fixed-size system of 2 ODEs; simple harmonic motion):
     *\code{.cpp}
     * using State = sm::vec<double, 2>;
     * sm::rungekutta4<double, State> rk (
     *     [](const double& t, const State& x) { return State{ x[1], -x[0] }; },
     *     State{ 1.0, 0.0 }, 0.0, 0.01);
     * rk.step();
     *\endcode
     *
     * Example (a system of an arbitrary number of ODEs):
     *\code{.cpp}
     * using State = sm::vvec<double>;
     * sm::rungekutta4<double, State> rk (
     *     [](const double& t, const State& x) { return x * -1.0; },
     *     State{ 1.0, 2.0, 3.0 }, 0.0, 0.01);
     * rk.step();
     *\endcode
     */
    template <typename T, typename State = T>
    struct rungekutta4
    {
        rungekutta4() {}

        rungekutta4 (const std::function<State(const T&, const State&)>& _f,
                      const State& _x0, const T& _t0 = T{0}, const T& _h = T{1})
            : f(_f), t(_t0), x(_x0), h(_h) {}

        // dx/dt = f(t, x)
        std::function<State(const T&, const State&)> f;

        // Independent variable (time)
        T t = T{0};
        // Current state, x(t)
        State x = State{};
        // Step size
        T h = T{1};

        // Advance the solution by one step of size h, updating t and x in place
        void step()
        {
            this->x = sm::rk4_step<T, State> (this->f, this->t, this->x, this->h);
            this->t += this->h;
        }

        // Advance the solution by n_steps steps of size h, returning the trajectory of
        // states (including the initial state) as an sm::vvec<State> of size n_steps + 1
        sm::vvec<State> integrate (const std::uint32_t n_steps)
        {
            sm::vvec<State> traj (n_steps + 1);
            traj[0] = this->x;
            for (std::uint32_t i = 1; i <= n_steps; ++i) {
                this->step();
                traj[i] = this->x;
            }
            return traj;
        }
    };

} // namespace sm
