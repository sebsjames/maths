#include <iostream>
#include <sm/mat>

int main()
{
    sm::mat<float, 4> m;
    m.translate (sm::vec<float>{1,2,3});
    sm::vec<float, 3> v3 = {0.1f, 0.2f, 0.3f};
    std::cout << "m - v3\n" << (m - v3) << std::endl;    // Should not compile
    return 0;
}
