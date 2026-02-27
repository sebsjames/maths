#include <iostream>
#include <sm/mat>

int main()
{
    int32_t rtn = 0;
    sm::mat<float, 4> m;
    auto v = sm::vec<float, 15>{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3};
    m = v; // Should not compile
    return rtn;
}
