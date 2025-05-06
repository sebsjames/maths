#include <iostream>

#include <sj/vvec>
#include <sj/vec>
#include <sj/hdfdata>

// Test containers of vvecs

int main()
{
    int rtn = 1;

    sj::vvec<sj::vvec<FLT>> vvv;
    vvv.push_back (sj::vvec<FLT>({1,2,3}));
    vvv.push_back (sj::vvec<FLT>({2,2,3}));
    vvv.push_back (sj::vvec<FLT>({3,2,3}));
    vvv.push_back (sj::vvec<FLT>({4,2,3}));

    // Sadly a vvec of sj::vec is nok.
    sj::vvec<sj::vec<FLT, 3>> vvec_of_vec (vvv.size());
    vvec_of_vec[0] = sj::vec<FLT, 3>({1,2,3});
    vvec_of_vec[1] = sj::vec<FLT, 3>({2,2,3});
    vvec_of_vec[2] = sj::vec<FLT, 3>({3,2,3});
    vvec_of_vec[3] = sj::vec<FLT, 3>({4,2,3});

    // Check content
    for (auto vv : vvv) { std::cout << "vv: " << vv << std::endl; }

    {
        sj::hdfdata data("test4.h5", std::ios::out | std::ios::trunc);
        data.add_contained_vals ("/vvv", vvv);
        data.add_contained_vals ("/vvec_of_vec", vvec_of_vec);
    } // data closes when out of scope

    // void read_contained_vals (const char* path, sj::vvec<sj::vvec<T>>& vals)
    sj::vvec<sj::vvec<FLT>> vvread;
    sj::vvec<sj::vec<FLT>> vvread_vvofv;
    {
        sj::hdfdata data("test4.h5", std::ios::in);
        data.read_contained_vals ("/vvv", vvread);
        data.read_contained_vals ("/vvec_of_vec", vvread_vvofv);
    }

    for (auto vv : vvread) {
        std::cout << "vv read: " << vv << std::endl;
    }

    if (vvv[0] == vvread[0]
        && vvv[3] == vvread[3]
        && vvec_of_vec[3][0] == vvread_vvofv[3][0]
        && vvec_of_vec[3][1] == vvread_vvofv[3][1]
        && vvec_of_vec[3][2] == vvread_vvofv[3][2]
        ) {
        rtn = 0;
    }

    return rtn;
}
