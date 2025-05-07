#include <iostream>
#include <sm/random>

int main()
{
    int rtn = 0;

#ifdef __GNUC__
    // GCC not fussy and will accept this even though it defies the standard
    typedef unsigned char SMALL_T;
#else
    // VS enforces the strict minimum size for the std::random code in Random.h, so
    // smallest you can get away with is unsigned short.
    typedef unsigned short SMALL_T;
#endif
    // A random uniform generator returning integer types
    sm::rand_uniform<SMALL_T, std::mt19937> rui;
    std::cout << "Random number is " << static_cast<unsigned int>(rui.get()) << std::endl;
    // You can find the min and max:
    std::cout << "That integer unsigned char (or short on Windows) RNG has min and max: " << static_cast<unsigned int>(rui.min())
         << "/" << static_cast<unsigned int>(rui.max()) << std::endl;

    // A random uniform generator returning real/floating point types
    sm::rand_uniform<float, std::mt19937> ruf;
    std::cout << "Random float number is " << ruf.get() << std::endl;

    // You can find the min and max:
    std::cout << "That float RNG has min and max: " << ruf.min() << "/" << ruf.max() << std::endl;

    // You can get a vector of numbers taken from a random number generator:
    sm::rand_uniform<double, std::mt19937_64> rud;
    std::vector<double> tenrns = rud.get(10);
    std::cout << "10 random doubles:" << std::endl;
    for (auto d : tenrns) { std::cout << d << std::endl; }

    // You can set up an RNG which has different max and min values: Note use of 64 bit
    // generator even though unsigned int is 32 bits. Will generate a longer sequence of
    // numbers before a repeat.
    sm::rand_uniform<unsigned int, std::mt19937_64> rubnd (0, 3);
    // You can find the min and max:
    std::cout << "That bounded, unsigned integer RNG has min and max: " << rubnd.min() << "/" << rubnd.max() << std::endl;
    std::cout << "Ten random unsigned int numbers in that range:\n";
    std::vector<unsigned int> tenrns2 = rubnd.get(10);
    for (auto d : tenrns2) { std::cout << d << std::endl; }

    // There is an overload of get which fills a fixed size array that you pass it with random numbers
    std::array<unsigned int, 12> twelverns;
    rubnd.get(twelverns);
    std::cout << "Twelve random unsigned int numbers in an array:\n";
    for (auto d : twelverns) { std::cout << d << std::endl; }


    // Similar for float
    sm::rand_uniform<float, std::mt19937> rubndf (0.0f, 1000.0f, 1);
    // You can find the min and max:
    std::cout << "FIXED SEED: bounded, float RNG has min and max: " << rubndf.min() << "/" << rubndf.max() << std::endl;
    std::cout << "Ten random float numbers in that range:\n";
    std::vector<float> tenrns3 = rubndf.get(10);
    for (auto d : tenrns3) { std::cout << d << std::endl; }

    // Another with seed 1 for float
    sm::rand_uniform<float, std::mt19937> rubndf2 (0.0f, 1000.0f, 1);
    std::cout << "Ten random float numbers in that range from second rng with seed 1:\n";
    std::vector<float> tenrns32 = rubndf2.get(10);
    for (auto d : tenrns32) { std::cout << d << std::endl; }

    // Test two rng generators where no seed is specified
    sm::rand_uniform<float, std::mt19937> rubndf3 (0.0f, 1000.0f);
    std::cout << "Ten random float numbers from the first 'default seed rng':\n";
    std::vector<float> tenrns33 = rubndf3.get(10);
    for (auto d : tenrns33) { std::cout << d << std::endl; }
    sm::rand_uniform<float, std::mt19937> rubndf4 (0.0f, 1000.0f);
    std::cout << "Ten random float numbers from the second 'default seed rng':\n";
    std::vector<float> tenrns34 = rubndf4.get(10);
    for (auto d : tenrns34) { std::cout << d << std::endl; }

    // Normally distributed numbers:
    sm::rand_normal<double, std::mt19937_64> rnorm (5, 0.1);
    std::vector<double> tennorms = rnorm.get(10);
    std::cout << "10 random normals (double type):" << std::endl;
    for (auto d : tennorms) { std::cout << d << std::endl; }

    sm::rand_lognormal<double, std::mt19937_64> rln (5, 0.1);
    std::vector<double> tenlnorms = rln.get(10);
    std::cout << "10 log normals (double type):" << std::endl;
    for (auto d : tenlnorms) { std::cout << d << std::endl; }

    sm::rand_poisson<int, std::mt19937> rpois (5);
    std::vector<int> tenpois = rpois.get(10);
    std::cout << "10 Poisson RNs (int type):" << std::endl;
    for (auto d : tenpois) { std::cout << d << std::endl; }

    return rtn;
}
