#include <iostream>
#include <sm/vonmises>

int main()
{
    sm::rand_uniform<double> rngu;
    sm::rand_normal<double> rngn;
    double sample_angle = sm::random::vonmises (rngu, rngn, 0, 3);
    std::cout << "sample: " << sample_angle << std::endl;
}
