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

export namespace sm
{
    // Make a randomized path to follow
    template <typename T>
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
            cubic_spline_expansion (this->a, this->a_tau);
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

        void about_turn() { this->theta += sm::mathconst<float>::pi; }

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
            sm::vvec<sm::vec<T, 2>> coords;
            T _theta = theta0;                   // rotational state of walking agent
            sm::vec<T, 2> position = position0;  // positional state of walking agent
            for (std::uint32_t i = 0; i < this->n_steps; ++i) {
                this->step();
                // Use walk.omega (angular speed) and walk.speed (linear/fwds) to generate x/y
                _theta += this->omega;
                position += sm::vec<T, 2>{this->speed * std::cos (_theta), this->speed * std::sin (_theta)};
                coords.push_back (position);
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
        sm::range<T> amm = { T{0}, T{0.005} };
        // how often does the acceleration change?
        std::uint32_t a_tau = 50;

        // FD is the drag coefficient
        static constexpr T FD = T{0.15};
        static constexpr T one_minus_FD = (T{1} - FD);

        // Random number generation
        std::unique_ptr<sm::rand_vonmises<T>> rVM;
    };

    template <typename T, std::uint32_t N>
    void cubic_spline_expansion (sm::vvec<T>& v, std::uint32_t n)
    {
        if (v.size() != N) { throw std::runtime_error ("v.size != N!"); }
        sm::vec<sm::vec<T, 2>, N> p;
        for (std::uint32_t i = 0; i < N; ++i) { p[i] = { static_cast<T>(n) * i, v[i] }; }
        sm::spline<T, N> spl (p);
        v.resize (v.size() * (n + 1), T{0});
        T ti = T{0};
        for (std::uint32_t i = 0; i < v.size(); ++i, ti += T{1}) { v[i] = spl.compute (ti); }
    }

    constexpr sm::vec<int, 28> supported_N()
    {
        sm::vec<int, 28> sn = {};
        int j = 0;
        for (int i = 2; i < 21; ++i) { sn[j++] = i; } // 2 - 20
        sn[j++] = 25;
        for (int i = 30; i < 110; i += 10) { sn[j++] = i; } // 30 - 100
        return sn;
    }

    // Place n elements between each element in v, computing a cubic spline interpolation, assuming
    // that the x in f(x) increases by the value 1 with each step. Think of x as t, and there is 1
    // timestep between each element in the expanded vvec v. This calls fixed-size versions of
    // cubic_spline_expansion due to the limitation from our fixed size sm::mat.
    template <typename T>
    void cubic_spline_expansion (sm::vvec<T>& v, std::uint32_t n)
    {
        constexpr sm::vec<int, 28> sn = supported_N();

        // sm::mat is a fixed size matrix template class, so we have to have a switch statement
        switch (v.size()) {
        case 2:
            cubic_spline_expansion<T, 2> (v, n);
            break;
        case 3:
            cubic_spline_expansion<T, 3> (v, n);
            break;
        case 4:
            cubic_spline_expansion<T, 4> (v, n);
            break;
        case 5:
            cubic_spline_expansion<T, 5> (v, n);
            break;
        case 6:
            cubic_spline_expansion<T, 6> (v, n);
            break;
        case 7:
            cubic_spline_expansion<T, 7> (v, n);
            break;
        case 8:
            cubic_spline_expansion<T, 8> (v, n);
            break;
        case 9:
            cubic_spline_expansion<T, 9> (v, n);
            break;
        case 10:
            cubic_spline_expansion<T, 10> (v, n);
            break;
        case 11:
            cubic_spline_expansion<T, 11> (v, n);
            break;
        case 12:
            cubic_spline_expansion<T, 12> (v, n);
            break;
        case 13:
            cubic_spline_expansion<T, 13> (v, n);
            break;
        case 14:
            cubic_spline_expansion<T, 14> (v, n);
            break;
        case 15:
            cubic_spline_expansion<T, 15> (v, n);
            break;
        case 16:
            cubic_spline_expansion<T, 16> (v, n);
            break;
        case 17:
            cubic_spline_expansion<T, 17> (v, n);
            break;
        case 18:
            cubic_spline_expansion<T, 18> (v, n);
            break;
        case 19:
            cubic_spline_expansion<T, 19> (v, n);
            break;
        case 20:
            cubic_spline_expansion<T, 20> (v, n);
            break;
        case 25:
            cubic_spline_expansion<T, 25> (v, n);
            break;
        case 30:
            cubic_spline_expansion<T, 30> (v, n);
            break;
        case 40:
            cubic_spline_expansion<T, 40> (v, n);
            break;
        case 50:
            cubic_spline_expansion<T, 50> (v, n);
            break;
        case 60:
            cubic_spline_expansion<T, 60> (v, n);
            break;
        case 70:
            cubic_spline_expansion<T, 70> (v, n);
            break;
        case 80:
            cubic_spline_expansion<T, 80> (v, n);
            break;
        case 90:
            cubic_spline_expansion<T, 90> (v, n);
            break;
        case 100:
            cubic_spline_expansion<T, 100> (v, n);
            break;
        case 0:
        case 1:
        default:
            std::cerr << "Make sure n_steps / a_tau gives a result in the list " << sn << std::endl;
            throw std::runtime_error (std::format("sm::cubic_spline_expansion cannot handle a vvec of size {} with n = {}.", v.size(), n));
            break;
        }
    }

} // namespace
