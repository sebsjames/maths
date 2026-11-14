#include <iostream>
#include <chrono>
#include <cstdint>
#include <cmath>

import sm.vvec;

template<typename F>
std::int32_t test_values (const sm::vvec<F>& r1, const sm::vvec<F>& r2, const sm::vvec<F>& r3,
                          const sm::vvec<F>& expect1, const sm::vvec<F>& expect2, const sm::vvec<F>& expect3)
{
    // return 0 for exact match, >0 for match within that many epsilons (up to limit), -1 fail.
    std::int32_t rtn = 0;

    constexpr F thresh = F{10};

    for (std::uint32_t i = 0; i < r1.size(); ++i) {
        if (r1[i] != expect1[i] || r2[i] != expect2[i] || r3[i] != expect3[i]) {
            // Not *exact* match, but we may allow machine precision errors.
            F e1 = std::abs(r1[i] - expect1[i]) / std::numeric_limits<F>::epsilon();
            F e2 = std::abs(r2[i] - expect2[i]) / std::numeric_limits<F>::epsilon();
            F e3 = std::abs(r3[i] - expect3[i]) / std::numeric_limits<F>::epsilon();

            if (e1 <= thresh && e2 <= thresh && e3 <= thresh) {
                // Ok, return size of e1/e2/e3
                F e = std::max (e1, e2);
                e = std::max (e, e3);
                rtn = std::max (rtn, static_cast<std::int32_t>(std::round (e)));

            } else {
                rtn = -1;
                std::cerr << "Failed on arith/arith test\n";
                if (r1[i] != expect1[i]) {
                    std::cerr << "  r1[i] != expect1[i] --- " << r1[i] << " != " << expect1[i] << std::endl;
                }
                if (r2[i] != expect2[i]) {
                    std::cerr << "  r2[i] != expect2[i] --- " << r2[i] << " != " << expect2[i] << std::endl;
                }
                if (r3[i] != expect3[i]) {
                    std::cerr << "  r3[i] != expect3[i] --- " << r3[i] << " != " << expect3[i] << std::endl;
                }
                break;
            }
        }
    }
    return rtn;
}

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
    std::int32_t code = test_values<F> (r1, r2, r3, expect1, expect2, expect3);
    if (code < 0) {
        std::cerr << "Failed on arith/arith test\n";
        return code;
    }
    rtn = std::max (rtn, code);

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
    code = test_values<F> (r1, r2, r3, expect1, expect2, expect3);
    if (code < 0) {
        std::cerr << "Failed on container/arith test\n";
        return code;
    }
    rtn = std::max (rtn, code);

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
    code = test_values<F> (r1, r2, r3, expect1, expect2, expect3);
    if (code < 0) {
        std::cerr << "Failed on arith/container test\n";
        return code;
    }
    rtn = std::max (rtn, code);

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
    code = test_values<F> (r1, r2, r3, expect1, expect2, expect3);
    if (code < 0) {
        std::cerr << "Failed on container/container test\n";
        return code;
    }
    rtn = std::max (rtn, code);

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
    std::int32_t rtn1 = test<float>();
    if (rtn1 == 0) {
        std::cout << "Success with perfect numerical matches\n";
    } else if (rtn1 > 0) {
        std::cout << "Success within " << rtn1 << " epsilons of machine precision\n";
    } else { // (rtn1 < 0)
        std::cout << "Failed\n";
        rtn--;
    }

    std::cout << "Double precision float\n";
    std::cout << "----------------------\n";
    std::int32_t rtn2 = test<double>();
    if (rtn2 == 0) {
        std::cout << "Success with perfect numerical matches\n";
    } else if (rtn2 > 0) {
        std::cout << "Success within " << rtn2 << " epsilons of machine precision\n";
    } else { // (rtn2 < 0)
        std::cout << "Failed\n";
        rtn--;
    }

    if (rtn == 0) { std::cout << "Success\n"; } else { std::cout << "Test failed\n"; }

    return rtn;
}
