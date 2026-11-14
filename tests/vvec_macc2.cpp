#include <iostream>
#include <chrono>
#include <cstdint>

import sm.vvec;

template<typename F>
std::int32_t test()
{
    std::int32_t rtn = 0;

    using namespace std::chrono;
    using sc = std::chrono::steady_clock;

    sm::vvec<F> vv (1000000, F{0});
    sm::vvec<F> vv2 (vv.size(), F{0});
    sm::vvec<F> vv3 (vv.size(), F{0});
    sm::vvec<F> r3 (vv.size(), F{0});

    sc::time_point t0, t1, t2, t3;
    sm::vvec<F> r1, r2;

    sm::vvec<F> expect1 (vv.size(), F{0});
    sm::vvec<F> expect2 (vv.size(), F{0});
    sm::vvec<F> expect3 (vv.size(), F{0});

    // Multiply and accumulate

    vv.randomize();
    vv2.randomize();
    vv3.randomize();
    r3.randomize();

    // Manually compute expected values
    for (std::uint32_t i = 0; i < vv.size(); ++i) {
        expect1[i] = vv[i] * F{5} + F{2};
        expect2[i] = vv[i] * F{5} + F{2};
        expect3[i] = r3[i] * F{5} + F{2};
    }

    t0 = sc::now();

    r1 = vv.macc (F{5}, F{2});
    t1 = sc::now();

    r2 = (vv * F{5});
    r2 += F{2};

    t2 = sc::now();

    r3.macc_inplace (F{5}, F{2});

    t3 = sc::now();

    // Test values
    for (std::uint32_t i = 0; i < vv.size(); ++i) {
        if (r2[i] != expect1[i] || r2[i] != expect2[i] || r3[i] != expect3[i]) {
            --rtn;
            std::cerr << "Failed on arith/arith test\n";
            break;
        }
    }

    std::cout << "\n1000000 random numbers * 5 + 2\n";
    std::cout << "macc     " << duration_cast<microseconds>((t1 - t0)).count() << " us\n";
    std::cout << "separate " << duration_cast<microseconds>((t2 - t1)).count() << " us\n";
    std::cout << "mac iplce " << duration_cast<microseconds>((t3 - t2)).count() << " us\n";


    vv.randomize();
    vv2.randomize();
    vv3.randomize();
    r3.randomize();

    // Manually compute
    for (std::uint32_t i = 0; i < vv.size(); ++i) {
        expect1[i] = vv[i] * vv2[i] + F{2};
        expect2[i] = vv[i] * vv2[i] + F{2};
        expect3[i] = r3[i] * vv2[i] + F{2};
    }

    t0 = sc::now();

    r1 = vv.macc (vv2, F{2});

    t1 = sc::now();

    r2 = (vv * vv2);
    r2 += F{2};

    t2 = sc::now();

    r3.macc_inplace (vv2, F{2});

    t3 = sc::now();

    // Test values
    for (std::uint32_t i = 0; i < vv.size(); ++i) {
        if (r2[i] != expect1[i] || r2[i] != expect2[i] || r3[i] != expect3[i]) {
            --rtn;
            std::cerr << "Failed on container/arith test\n";
            break;
        }
    }

    std::cout << "\n1000000 random numbers * 1000000 rns + 2\n";
    std::cout << "macc     " << duration_cast<microseconds>((t1 - t0)).count() << " us\n";
    std::cout << "separate " << duration_cast<microseconds>((t2 - t1)).count() << " us\n";
    std::cout << "mac iplce " << duration_cast<microseconds>((t3 - t2)).count() << " us\n";


    vv.randomize();
    vv2.randomize();
    vv3.randomize();
    r3.randomize();

    // Manually compute
    for (std::uint32_t i = 0; i < vv.size(); ++i) {
        expect1[i] = vv[i] * F{5} + vv2[i];
        expect2[i] = vv[i] * F{5} + vv2[i];
        expect3[i] = r3[i] * F{5} + vv2[i];
    }

    t0 = sc::now();

    r1 = vv.macc (F{5}, vv2);
    t1 = sc::now();

    r2 = (vv * F{5});
    r2 += vv2;

    t2 = sc::now();

    r3.macc_inplace (F{5}, vv2);

    t3 = sc::now();

    // Test values
    for (std::uint32_t i = 0; i < vv.size(); ++i) {
        if (r2[i] != expect1[i] || r2[i] != expect2[i] || r3[i] != expect3[i]) {
            --rtn;
            std::cerr << "Failed on container/arith test\n";
            break;
        }
    }

    std::cout << "\n1000000 random numbers * 5 + 1000000 rns\n";
    std::cout << "macc     " << duration_cast<microseconds>((t1 - t0)).count() << " us\n";
    std::cout << "separate " << duration_cast<microseconds>((t2 - t1)).count() << " us\n";
    std::cout << "mac iplce " << duration_cast<microseconds>((t3 - t2)).count() << " us\n";

    vv.randomize();
    vv2.randomize();
    vv3.randomize();
    r3.randomize();

    // Manually compute
    for (std::uint32_t i = 0; i < vv.size(); ++i) {
        expect1[i] = vv[i] * vv3[i] + vv2[i];
        expect2[i] = vv[i] * vv3[i] + vv2[i];
        expect3[i] = r3[i] * vv3[i] + vv2[i];
    }

    t0 = sc::now();

    r1 = vv.macc (vv3, vv2);
    t1 = sc::now();

    r2 = (vv * vv3);
    r2 += vv2;

    t2 = sc::now();

    r3.macc_inplace (vv3, vv2);

    t3 = sc::now();

    // Test values
    for (std::uint32_t i = 0; i < vv.size(); ++i) {
        if (r2[i] != expect1[i] || r2[i] != expect2[i] || r3[i] != expect3[i]) {
            --rtn;
            std::cerr << "Failed on container/arith test\n";
            break;
        }
    }

    std::cout << "1000000 random numbers * 1000000 rns + 1000000 rns\n";
    std::cout << "macc      " << duration_cast<microseconds>((t1 - t0)).count() << " us\n";
    std::cout << "separate  " << duration_cast<microseconds>((t2 - t1)).count() << " us\n";
    std::cout << "mac iplce " << duration_cast<microseconds>((t3 - t2)).count() << " us\n";

    return rtn;
}

std::int32_t main()
{
    std::int32_t rtn = 0;

    std::cout << "Single precision float\n";
    std::cout << "----------------------\n";
    rtn += test<float>();

    std::cout << "Double precision float\n";
    std::cout << "----------------------\n";
    rtn += test<double>();

    if (rtn == 0) { std::cout << "Success\n"; } else { std::cout << "Test failed\n"; }

    return rtn;
}
