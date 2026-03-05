#include <iostream>
#include <sm/mat>

int main()
{
    sm::mat<float, 4> m;
    m.translate (sm::vec<float>{1,2,3});
    sm::vec<float, 4> v4 = {0.1f, 0.2f, 0.3f, 0.0f};
    std::cout << "m - v4\n" << (m - v4) << std::endl;    // Should not compile
    return 0;
}
