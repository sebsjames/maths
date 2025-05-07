#include <sm/hdfdata>
#include <sm/vvec>

// Showing use of HdfData with std::ios flags to specify file access

int main()
{
    int rtn = 0;

    sm::vvec<float> vv = { 1.0f, 2.0f, 3.0f };

    {
        sm::hdfdata data("test5.h5", std::ios::out|std::ios::trunc);
        data.add_contained_vals ("/vv", vv);
    } // data closes when out of scope

    sm::vvec<float> vv1;
    {
        sm::hdfdata data("test5.h5", std::ios::in);
        data.read_contained_vals ("/vv", vv1);
    } // data closes when out of scope

    if (vv != vv1) { --rtn; }

    return rtn;
}
