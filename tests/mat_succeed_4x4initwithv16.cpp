#include <iostream>
#include <sm/mat>

int main()
{
    int32_t rtn = 0;
    sm::mat<float, 4> m (sm::vec<float, 16>{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4});
    std::cout << "m initialized from v16\n" << m << std::endl;
    auto v = sm::vec<float, 16>{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4};
    if (m.arr != v) { --rtn; }
    return rtn;
}
