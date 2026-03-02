#include <iostream>
import sm.vec;

int main()
{
    auto v = sm::vec<float>{};
    std::cout << "Default vec contains: " << v.str() << std::endl;
}
