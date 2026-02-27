#include <iostream>
#include <sm/mat>

int main()
{
    sm::mat<float, 4> m (sm::vec<float, 4>{1,2,3,4}); // Should not compile
    std::cout << "m initialized from v4\n" << m << std::endl;
    return 0;
}
