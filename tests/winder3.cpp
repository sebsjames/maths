#include <iostream>
#include <vector>
#include <sm/vec>
#include <sm/winder>

/*
 * More tests for winder
 */
int main()
{
    int rtn = 0;

    // Make another boundary
    std::vector<sm::vec<float, 2>> bnd;

    bnd.push_back ({-1.151757,  -0.865333});
    bnd.push_back ({-0.580842,  -1.169353});
    bnd.push_back ({0.155741,  -1.100337});
    bnd.push_back ({0.860626,  -0.676779});
    bnd.push_back ({1.344939,  -0.012170});
    bnd.push_back ({1.478909,   0.715408});
    bnd.push_back ({1.226639,   1.311001});
    bnd.push_back ({0.655724,   1.615021});
    bnd.push_back ({-0.080859,   1.546006});
    bnd.push_back ({-0.785743,   1.122448});
    bnd.push_back ({-1.270057,   0.457838});
    bnd.push_back ({-1.404027,  -0.269740});

    sm::winder w (bnd);
    sm::vec<float, 2> px = { -0.27075, 1.4834 }; // outside, but says inside?!?

    std::cout << "pt = " << px.str_mat() << std::endl;
    int wn = w.wind (px);
    std::cout << "Winding number for " << px << " = " << wn << std::endl;
    if (wn != 0) { --rtn; }


#if 0
    for (int i = 0; i < 10; ++i) {
        std::cout << "pt = " << px.str_mat() << std::endl;
        wn = w.wind (px);
        std::cout << "Winding number for " << px << " = " << wn << std::endl;
        px[1] += 0.01f;
    }
#endif

    std::cout << "Test " << (rtn ? "failed\n" : "passed\n");
    return rtn;
}
