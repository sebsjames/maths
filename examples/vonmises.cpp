#include <iostream>
import sm.random;

int main()
{
    sm::rand_vonmises<double> rng_vonmises(0, 3); // mu = 0, kappa = 3
    double s = rng_vonmises.get();
    std::cout << "sample from the von Mises distribution: " << s << std::endl;
}
