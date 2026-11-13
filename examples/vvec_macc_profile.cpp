#include <iostream>
#include <chrono>

import sm.vvec;

template<typename F>
void test()
{
    using namespace std::chrono;
    using sc = std::chrono::steady_clock;

    sm::vvec<F> vv (1000000, F{0});
    sm::vvec<F> vv2 (vv.size(), F{0});
    sm::vvec<F> vv3 (vv.size(), F{0});
    sm::vvec<F> r3 (vv.size(), F{0});    

    sc::time_point t0, t1, t2, t3;
    sm::vvec<F> r1, r2;

    // Multiply and accumulate

    vv.randomize();
    vv2.randomize();    
    vv3.randomize();    
    r3.randomize();
    
    t0 = sc::now();

    r1 = vv.macc (F{5}, F{2});
    t1 = sc::now();

    r2 = (vv * F{5});
    r2 += F{2};

    t2 = sc::now();

    r3.macc_inplace (F{5}, F{2});
    
    t3 = sc::now();

    std::cout << "\n1000000 random numbers * 5 + 2\n";
    std::cout << "macc     " << duration_cast<microseconds>((t1 - t0)).count() << " us\n";
    std::cout << "separate " << duration_cast<microseconds>((t2 - t1)).count() << " us\n";    
    std::cout << "mac iplce " << duration_cast<microseconds>((t3 - t2)).count() << " us\n";    


    vv.randomize();
    vv2.randomize();    
    vv3.randomize();    
    r3.randomize();

    t0 = sc::now();

    r1 = vv.macc (vv2, F{2});
    t1 = sc::now();

    r2 = (vv * vv2);
    r2 += F{2};

    t2 = sc::now();

    r3.macc_inplace (vv2, F{2});
    
    t3 = sc::now();


    std::cout << "\n1000000 random numbers * 1000000 rns + 2\n";
    std::cout << "macc     " << duration_cast<microseconds>((t1 - t0)).count() << " us\n";
    std::cout << "separate " << duration_cast<microseconds>((t2 - t1)).count() << " us\n";    
    std::cout << "mac iplce " << duration_cast<microseconds>((t3 - t2)).count() << " us\n";    
    

    vv.randomize();
    vv2.randomize();    
    vv3.randomize();
    r3.randomize();

    t0 = sc::now();

    r1 = vv.macc (F{5}, vv2);
    t1 = sc::now();

    r2 = (vv * F{5});
    r2 += vv2;

    t2 = sc::now();

    r3.macc_inplace (F{5}, vv2);
    
    t3 = sc::now();

    std::cout << "\n1000000 random numbers * 5 + 1000000 rns\n";
    std::cout << "macc     " << duration_cast<microseconds>((t1 - t0)).count() << " us\n";
    std::cout << "separate " << duration_cast<microseconds>((t2 - t1)).count() << " us\n";    
    std::cout << "mac iplce " << duration_cast<microseconds>((t3 - t2)).count() << " us\n";    
    
    vv.randomize();
    vv2.randomize();    
    vv3.randomize();    
    r3.randomize();

    t0 = sc::now();

    r1 = vv.macc (vv3, vv2);
    t1 = sc::now();

    r2 = (vv * vv3);
    r2 += vv2;

    t2 = sc::now();

    r3.macc_inplace (vv3, vv2);
    
    t3 = sc::now();

    std::cout << "1000000 random numbers * 1000000 rns + 1000000 rns\n";
    std::cout << "macc      " << duration_cast<microseconds>((t1 - t0)).count() << " us\n";
    std::cout << "separate  " << duration_cast<microseconds>((t2 - t1)).count() << " us\n";    
    std::cout << "mac iplce " << duration_cast<microseconds>((t3 - t2)).count() << " us\n";    
}

int main()
{
    int rtn = 0;

    std::cout << "Single precision float\n";
    std::cout << "----------------------\n";    
    test<float>();

    std::cout << "Double precision float\n";
    std::cout << "----------------------\n";    
    test<double>();
    
    return rtn;
}
