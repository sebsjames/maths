// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file random
 *
 * Random numbers in the sm namespace, wrapping C++ <random> stuff, with a particular
 * favouring for mt19937 and mt19937_64, the 32 and 64 bit Mersenne Twister
 * algorithms. With these classes, generate random numbers using our choice of
 * algorithms from std::random. In future, I'd like to include a siderand approach to
 * collecting entropy.
 *
 * Note on choice of mt19937 vs mt19937_64 as the 'E' template parameter: the 64 bit
 * version is slower, but is suitable if you want to generate random numbers where the
 * repeat of the sequence occurs only after as long a time as possible.
 *
 * Possible values for E:
 *
 * std::mt19937, std::mt19937_64: 32 and 64 bit Mersenne Twister (MT). 32 bit MT seems
 * to be the fastest engine! std::mt19937_64 takes twice the time.
 *
 * std::minstd_rand: linear congruential engine. 'new minimum standard' recommended by
 * Park, Miller, and Stockmeyer in 1993. The linear congruential engine is moderately
 * fast and has a very small storage requirement for state. I found it no faster than
 * std::mt19937 in one test.
 *
 * std::ranlux24, std::ranlux48: subtract-with-carry engine (aka lagged
 * Fibonacci). Seems very slow - ranlux48 is about 20 times slower than mt19937.
 *
 * std::knuth_b: A shuffle order engine. Quite slow.
 *
 * I've wrapped a selection of distributions, including normal, lognormal, poisson and
 * uniform. Copy the classes here to add additional ones that you might need from the
 * full list: https://en.cppreference.com/w/cpp/numeric/random (such as weibull or
 * exponential).
 *
 * See tests/random1.cpp for a variety of usage examples. Here is a single, simple
 * example:
 *
 * \code
 * import sm.random;
 * sm::randuniform<double, std::mt19937_64> rand_double;
 * double sample = rand_double.get();
 * double sample2 = rand_double.get();
 * \endcode
 *
 * A final note: There are some faster RNG algorithms on the
 * block. Xoroshiro/Xoshiro/Xorshift and SplitMix64. These don't appear to be in the c++
 * standard as yet, but they're short and could probably be implemented easily here,
 * another day.
 */
module;

#include <sm/random.hpp>

export module sm.random;

export namespace sm
{
    using sm::rand_uniform;
    using sm::rand_normal;
    using sm::rand_lognormal;
    using sm::rand_poisson;
    using sm::rand_exponential;
    using sm::rand_pareto;
    using sm::rand_vonmises;
    using sm::chargroup;
    using sm::rand_string;
}
