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
 *
 * \author Alex Blenkinsop (with Claude Code then human sanity check from Seb)
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
     * \brief Stateful 4th order Runge-Kutta integrator
     *
     * Holds the current state (t, x) of a system dx/dt = f(t, x), the derivative
     * function f and a step size h, so that step() and integrate() can be called
     * repeatedly to march the solution forward.
     *
     * Example (single scalar ODE, dx/dt = -x):
     *\code{.cpp}
     * sm::rungekutta4<double> rk ([](const double& t, const double& x) { return -x; }, 1.0, 0.0, 0.01);
     * rk.step();
     *\endcode
     *
     * Example (a fixed-size system of 2 ODEs; simple harmonic motion):
     *\code{.cpp}
     * using X = sm::vec<double, 2>;
     * sm::rungekutta4<double, X> rk (
     *     [](const double& t, const X& x) { return X{ x[1], -x[0] }; },
     *     X{ 1.0, 0.0 }, 0.0, 0.01);
     * rk.step();
     *\endcode
     *
     * Example (a system of an arbitrary number of ODEs):
     *\code{.cpp}
     * using X = sm::vvec<double>;
     * sm::rungekutta4<double, X> rk (
     *     [](const double& t, const X& x) { return x * -1.0; },
     *     X{ 1.0, 2.0, 3.0 }, 0.0, 0.01);
     * rk.step();
     *\endcode
     *
     * \tparam T The scalar type of the independent variable, the step size and the RK4
     * weighting coefficients.
     *
     * \tparam X The type of the dependent variable x; it may be
     * a plain scalar (a single ODE), an sm::vec<T, N> (a fixed-size system of N ODEs)
     * or an sm::vvec<T> (a system of an arbitrary number of ODEs).
     */
    template <typename T, typename X = T>
    struct rungekutta4
    {
        rungekutta4() {}

        rungekutta4 (const std::function<X(const T&, const X&)>& _f,
                     const X& _x0, const T& _t0 = T{0}, const T& _h = T{1})
            : f(_f), t(_t0), x(_x0), h(_h) {}

        // dx/dt = f(t, x)
        std::function<X(const T&, const X&)> f;
        // Independent variable (time)
        T t = T{0};
        // Current state, x(t)
        X x = X{};
        // Step size
        T h = T{1};
        // Keep k1 to k4 as members, in case they are vvecs or vecs. This avoids re-allocating their
        // memory on each call to rungekutta4::step().
        X k1 = X{};
        X k2 = X{};
        X k3 = X{};
        X k4 = X{};

        /*!
         * Advance the solution by one step of size h, updating t and x in place
         */
        void step()
        {
            const T half_h = this->h / T{2};
            const T sixth_h = this->h / T{6};

            this->k1 = this->f (this->t, this->x);
            this->k2 = this->f (this->t + half_h, this->x + k1 * half_h);
            this->k3 = this->f (this->t + half_h, this->x + k2 * half_h);
            this->k4 = this->f (this->t + this->h, this->x + k3 * this->h);

            this->x += (k1 + k2 * T{2} + k3 * T{2} + k4) * sixth_h;
            this->t += this->h;
        }

        // Advance the solution by n_steps steps of size h, returning the trajectory of
        // states (including the initial state) as an sm::vvec<X> of size n_steps + 1
        sm::vvec<X> integrate (const std::uint32_t n_steps)
        {
            sm::vvec<X> traj (n_steps + 1);
            traj[0] = this->x;
            for (std::uint32_t i = 1; i <= n_steps; ++i) {
                this->step();
                traj[i] = this->x;
            }
            return traj;
        }
    };

} // namespace sm
