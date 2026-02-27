#include <iostream>
#include <sm/mat>

int main()
{
    int32_t rtn = 0;
    sm::mat<float, 4> m;
    auto v = sm::vec<float, 16>{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4};
    m = v; // ok
    if (m != v) { --rtn; } else { std::cout << "Not Not-Equals, Good\n"; } // Should not compile
    return rtn;
}
