#include <iostream>
#include <sm/mat>

int main()
{
    int rtn = 0;

    {
        // What happens when we add a vec to a mat<float, 4>?
        sm::mat<float, 4> m;
        m.translate (sm::vec<float>{1,2,3});


        sm::vec<float, 3> v3 = {0.1f, 0.2f, 0.3f};

        // m1 = m + v
        std::cout << "m + v3\n" << (m + v3) << std::endl;    // Should not compile

        // If we added as if v3 were a translation, the right answer would be

        sm::mat<float, 4> m1 = m * sm::mat<float, 4>(v3);

        if (m1 != m + v3) { --rtn; }
    }

    return rtn;
}
