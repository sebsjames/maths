#include <iostream>
import sm.vec;
import sm.range; // no absolutely necessary, as sm.vec exports sm.range

int main()
{
    auto v = sm::vec<float>{1,2,3};

    std::cout << "vec contains: " << v.str() << std::endl;

    std::cout << "vec range: " << v.range() << std::endl;

    // Explicit use of sm::range
    auto r = sm::range<float>::search_initialized();

    r.update (1.0f);
    r.update (-1.0f);
    r.update (10.0f);
    r.update (100.0f);

    std::cout << "Range is [" << r.min << ", " << r.max << "]" << std::endl;
    std::cout << r << std::endl;
}
