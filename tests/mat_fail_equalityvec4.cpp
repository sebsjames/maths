#include <iostream>
#include <sm/mat>

int main()
{
    int32_t rtn = 0;
    sm::mat<float, 4> m;
    auto v = sm::vec<float, 4>{1,2,3,4};
    if (m == v) { std::cout << "Equals\n"; } else {  } // Should not compile
    return rtn;
}
