#include <iostream>
import sm.range;

int main()
{
    auto r = sm::range<float>::search_initialized();

    r.update (1.0f);
    r.update (-1.0f);
    r.update (10.0f);
    r.update (100.0f);

    std::cout << "Range is [" << r.min << ", " << r.max << "]" << std::endl;
    std::cout << r << std::endl;
}
