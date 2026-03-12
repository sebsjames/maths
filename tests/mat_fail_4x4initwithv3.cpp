#include <iostream>

import sm.vec;
import sm.mat;

int main()
{
    sm::mat<float, 4> m (sm::vec<float>{1,2,3}); // Should not compile
    std::cout << "m initialized from v3\n" << m << std::endl;
    return 0;
}
