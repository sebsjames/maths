// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * A Random walk module. Generate a bee-like random walk, following the description given in:
 *
 * "An Anatomically Constrained Model for Path Integration in the Bee
 * Brain", Current Biology 27, 3069-3085, October 23, 2017 (Stone et al.)
 *
 * DOI: http://dx.doi.org/10.1016/j.cub.2017.08.052
 *
 * \author Seb James
 * \date 2026
 */
module;

#include <cstdint>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <format>

export module sm.random_walk;

import sm.spline;
import sm.mathconst;
import sm.random;
import sm.algo;
import sm.vec;
import sm.vvec;

namespace sm
{
    // Make a randomized path to follow
    export template <typename T>
    struct random_walk
    {
        random_walk() {}

        random_walk (const std::uint32_t _n_steps, const std::uint32_t _a_tau)
        {
            this->n_steps = _n_steps;
            this->a_tau = _a_tau;
            this->init();
        }

        random_walk (const std::uint32_t _n_steps, const std::uint32_t _a_tau, const T& _kappa)
        {
            this->n_steps = _n_steps;
            this->a_tau = _a_tau;
            this->kappa = _kappa;
            this->init();
        }

        random_walk (const std::uint32_t _n_steps, const std::uint32_t _a_tau, const T& _kappa, const T& acc_max)
        {
            this->n_steps = _n_steps;
            this->a_tau = _a_tau;
            this->kappa = _kappa;
            this->amm.max = acc_max;
            this->init();
        }

        void init()
        {
            this->rVM = std::make_unique<sm::rand_vonmises<T>> (T{0}, kappa);
            this->a.resize (this->n_steps / this->a_tau);
            this->a.randomize();
            this->a *= (amm.span());
            this->a += amm.min;
            sm::cubic_spline_expansion (this->a, this->a_tau);
        }

        // Reset state
        void reset()
        {
            this->t = 0;
            this->theta = T{0};
            this->omega = T{0};
            this->velocity = {};
            this->speed = T{0};
        }

        void about_turn() { this->theta += sm::mathconst<T>::pi; }

        // Advance the route generation by one timestep
        void step()
        {
            // This is the model as stated in the paper and it should be equivalent to lfilter
            // function.
            T epsilon = this->rVM->get(); // Angular acceleration
            this->omega = this->lambda * this->omega + epsilon;
            this->theta += this->omega;

            T accel = T{0};
            if (t < this->a.size()) { accel = this->a[this->t]; }

            sm::vec<T, 2> thrust = { accel * std::sin (theta), accel * std::cos (theta) };
            this->velocity = (this->velocity + thrust) * one_minus_FD;
            this->speed = (this->speed + accel) * one_minus_FD;

            ++this->t;
            if (this->t > this->n_steps) { this->reset(); }
        }

        // Generate a walk of n_steps with given starting positional/angular state
        sm::vvec<sm::vec<T, 2>> generate (const T theta0 = T{0}, const sm::vec<T, 2> position0 = sm::vec<T, 2>{})
        {
            sm::vvec<sm::vec<T, 2>> coords (this->n_steps);
            T _theta = theta0;                   // rotational state of walking agent
            sm::vec<T, 2> position = position0;  // positional state of walking agent
            for (std::uint32_t i = 0; i < this->n_steps; ++i) {
                this->step();
                // Use omega (angular speed) and speed (linear/fwds) to generate x/y
                _theta += this->omega;
                position += sm::vec<T, 2>{this->speed * std::cos (_theta), this->speed * std::sin (_theta)};
                coords[i] = position;
            }
            return coords;
        }

        // Number of steps total.
        std::uint32_t n_steps = 0;
        // Current time step
        std::uint32_t t = 0;

        // State
        T theta = T{0};              // Heading/theta
        T omega = T{0};              // Angular velocity
        sm::vec<T, 2> velocity = {}; // Cartesian velocity (or can use speed):
        T speed = T{0};              // Linear speed

        // Parameters
        const T lambda = T{0.4};
        T kappa = T{100};                   // Von Mises concentration parameter
        sm::vvec<T> a = {};                 // Acceleration values
        // Uniform RNG range outbound [0, 0.05]
        sm::interval<T> amm = { T{0}, T{0.005} };
        // how often does the acceleration change?
        std::uint32_t a_tau = 50;

        // FD is the drag coefficient
        static constexpr T FD = T{0.15};
        static constexpr T one_minus_FD = (T{1} - FD);

        // Random number generation
        std::unique_ptr<sm::rand_vonmises<T>> rVM;
    };

} // namespace
