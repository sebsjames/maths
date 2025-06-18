---
layout: page
title: sm::rand*
parent: Reference
permalink: /ref/random/
nav_order: 3
---
# sm::rand*
{: .no_toc}
## Random number (and string) generation
{: .no_toc}

```c++
#include <sm/random>
```
Header file: [<sm/random>](https://github.com/sebsjames/maths/blob/main/sm/random). Test and example code:  [tests/testRandom](https://github.com/sebsjames/maths/blob/main/tests/testRandom.cpp) [tests/testRandString](https://github.com/sebsjames/maths/blob/main/tests/testRandString.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary

High quality random number generation (RNG) is important in many scientific simulation projects. `<sm/random>` provides a set of random number generation classes. These classes wrap the standard random number generation in the C++ `<random>` header to give a convenient interface for the programmer. While it is possible to select different pseudo random number generators, the classes all default to the very acceptable mt19937 [Mersenne Twister algorithm](https://en.wikipedia.org/wiki/Mersenne_Twister) (using the 32 bit version for `rand_uniform`/`rand_poisson` and the 64 bit version for `rand_normal` and `rand_lognormal`).

## Intervals

This page uses the common mathematical notation for intervals:

* **[a,b)** for a *semi-closed interval* which includes values from and including **a** up to, but not including **b**.
* **[a,b]** for a *closed interval* which includes both **a** and **b** in the interval.

## sm::rand_uniform

Random number generation from a uniform distribution. Any possible value in the interval is equally likely to be returned. The `rand_uniform` class is templated and has integer and floating point specializations:

```c++
namespace sm {
    template <typename T = float, typename E = std::mt19937, bool = std::is_integral<std::decay_t<T>>::value>
    class rand_uniform {};
    // Both integer and floating point specializations are implemented
```
`T` is the type for the generated random numbers and `E` is the generator algorithm/engine.

### Simplest usage
```c++
// Floating point rand_uniform defaults to the semi-closed interval [0,1)
sm::rand_uniform<float> rng1; // Generate random numbers in float type output in interval [0,1)
float f = rng1.get();           // 0 <= f < 1

// By default, an integer rand_uniform provides a number from the closed interval [min, max] for the type
sm::rand_uniform<unsigned short> rng2;
unsigned short us = rng2.get(); // 0 <= us <= 65535
// Find the end points of the RNG interval with min() and max():
std::cout << "Integer RNG generates numbers in range " << rng2.min() << " to " << rng2.max() << std::endl;
```

### Setting the interval

You can choose an interval over which random numbers should be generated in the constructor
```c++
// Create a generator of integer values in interval [0,10]. The interval type should match
// the rand_uniform template type (unsigned int here)
sm::rand_uniform<unsigned int> rng (0, 10);

// Create a floating point RNG for the interval [-100, 100)
sm::rand_uniform<double> rng (-100.0, 100.0);
```

### Using a fixed seed

To use a fixed seed with the default interval, use a single `unsigned int` argument to the constructor:
```c++
sm::rand_uniform<int> rng (2303); // The first .get() should always return the same number now
```

To use a fixed seed with a custom interval, it's the third argument:
```c++
// Declaration
rand_uniform (T a, T b, unsigned int _seed);

// In use:
sm::rand_uniform<int> rng (0, 100, 2303); // Interval [0,100], seed = 2303
```

### Getting arrays of randomly generated values

If you need to obtain a large number of randomly generated values, it is most efficient to get these into an array, rather than using `rand_uniform::get()`` repeatedly.

```c++
sm::rand_uniform<double> rng (0.0, 10.0);

// One overload returns std::vector of results
std::vector<double> randoms_in_vector = rng.get (100); // Return a vector with 100 random numbers

// Another places values into an existing std::array
std::array<double, 100> randoms_in_array;
rng.get (randoms_in_array);
```

### Changing the engine

By default the 32 bit mt19937 Mersenne Twister algorithm is used in `rand_uniform`. To select a different engine, specify it as a second template parameter.

```c++
sm::rand_uniform<float, std::mt19937_64> rng1;  // Use the 64 bit version of mt19937
sm::rand_uniform<float, std::minstd_rand> rng2; // Use the linear congruential engine
```
There are some notes about the different engines that you can use in the comments in [sm/random](https://github.com/sebsjames/maths/blob/main/sm/random). You can also consult [C++ reference material](https://en.cppreference.com/w/cpp/numeric/random).

## sm::rand_normal and sm::rand_lognormal

Two C++ classes to generate values from either a normal (Gaussian) distribution or a log-normal distribution.

All the examples here show `rand_normal`, but you can substitute `rand_lognormal` if you want the log-normal distribution to generate your values.

```c++
namespace morph {
    template <typename T = double, typename E = std::mt19937_64>
    class rand_normal { // or class rand_lognormal
```
`T` is the type for the generated random numbers and `E` is the generator algorithm/engine.

### Simplest usage

To generate values from a normal distribution with mean 0 and standard deviation 1, create the a default rand_normal object:
```c++
sm::rand_normal<float> rng;
rng.get(); // Getters are the same across the Rand* classes
```

### Specifying the normal distribution parameters

The mean and standard deviation can be specified in the constructor:
```c++
sm::rand_normal<double> rng (1.0, 2.0); // Mean 1, standard deviation 2.
```
### Using a fixed seed

To use a fixed seed with the default interval, use a single `unsigned int` argument to the constructor:
```c++
sm::rand_normal<double> rng (2303); // The first .get() should always return the same number now
```

To use a fixed seed with a custom interval, it's the third argument:
```c++
// Declaration
rand_normal (T a, T b, unsigned int _seed);
// In use:
sm::rand_normal<float> rng (2.0f, 3.0f, 2303); // mean 2, std 3, seed = 2303
```

### Getters

The `get()` function overloads are the same in all the `Rand*` classes, so you can:

```c++
T value = rng.get();
// or
std::vector<T> values = rng.get (num);
// or
std::array<T, N> avals;
rng.get (avals);
```

## sm::rand_poisson

A C++ classes to generate values from a normal Poisson distribution.

```c++
namespace morph {
    template <typename T = int, typename E = std::mt19937>
    class rand_poisson
```
`T` is the integral type for the generated random numbers and `E` is the generator algorithm/engine.

### Simplest usage

To generate values from a Poisson distribution with mean 0, create the a default object:
```c++
sm::rand_poisson<int> rng;
rng.get();
```

### Specifying the distribution mean

The mean can be specified in the constructor:
```c++
sm::rand_poisson<int> rng (3); // Mean 3
```
### Using a fixed seed

To use a fixed seed with the default interval, use a single `unsigned int` argument to the constructor:
```c++
sm::rand_poisson<int> rng (2303); // The first .get() should always return the same number now
```

To use a fixed seed with a custom mean, it's the second argument:
```c++
// Declaration
rand_poisson (T mean, unsigned int _seed);
// In use:
sm::rand_poisson<int> rng (4, 2303); // mean 4, seed = 2303
```

### Getters

The `get()` function overloads are the same as for `rand_uniform`, `rand_normal` and `rand_lognormal`.


## sm::rand_string

The `rand_string` class is a little different from the other `Rand*` classes because it uses a `sm::rand_uniform` member to help it generate character strings. It allows you to generate random characters from different character groups such as `sm::CharGroup::AlphaNumeric` or `sm::CharGroup::Decimal`. It is a non-templated class:

```c++
namespace morph {
    class rand_string
```

### Simplest usage

To generate 8 HexLowerCase characters:
```c++
sm::rand_string string_gen; // Default string length 8, default char group HexLowerCase.
std::string randchars = string_gen.get();
```

You can get any length of string. For the example above, to get another 10 HexLowerCase characters:
```c++
std::string morechars = string_gen.get (10); // updates rand_string::length to 10 before getting chars
```

You can also change the character group at runtime:
```c++
string_gen.setCharGroup (sm::CharGroup::AlphaNumeric);
```

### Constructors

```c++
rand_string();                  // Default, length set to 8, char group set to HexLowerCase
rand_string(const size_t l)     // Construct with length set to l
rand_string(const size_t l, const sm::CharGroup& _cg) // Length l, character group _cg
```

The list of possible character groups is given by the `CharGroup` declaration:

```c++
namespace morph {
   enum class CharGroup
    {
        AlphaNumeric,          // 0-9A-Za-z                   62 chars
        Alpha,                 // A-Za-z                      52 chars
        AlphaNumericUpperCase, // 0123456789ABCDEF... ...XYZ  36 chars
        AlphaNumericLowerCase, // 0123456789abcdef... ...xyz  36 chars
        AlphaUpperCase,        // A-Z                         26 chars
        AlphaLowerCase,        // a-z                         26 chars
        HexUpperCase,          // 0123456789ABCDEF            16 chars
        HexLowerCase,          // 0123456789abcdef            16 chars
        Decimal,               // 0123456789                  10 chars
        BinaryTF,              // TF                           2 chars
        Binary                 // 01                           2 chars
    };
}
```
