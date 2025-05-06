#include <utility>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <deque>

#include <sj/hdfdata>

int main()
{
    int rtn = 0;

    std::deque<double> vd = { 1.0, 2.0, 3.0, 4.0 };
    {
        sj::hdfdata data("test.h5", sj::file_access_mode::truncate_write);
        data.add_contained_vals ("/testvectordouble", vd);
    } // data closes when out of scope

    std::deque<double> vdread;
    {
        sj::hdfdata data("test.h5", sj::file_access_mode::read_only);
        data.read_contained_vals ("/testvectordouble", vdread);
    }

    if (vd.size() == vdread.size()) {
        for (unsigned int i = 0; i < vd.size(); ++i) {
            if (vd[i] != vdread[i]) {
                rtn -= 1;
                break;
            }
        }
    }

    std::vector<float> vf = { 1.0, 2.0, 3.0, 4.0 };
    {
        sj::hdfdata data("test.h5", std::ios::out | std::ios::trunc);
        data.add_contained_vals ("/testvectorfloat", vf);
    } // data closes when out of scope

    std::vector<float> vfread;
    {
        sj::hdfdata data("test.h5", std::ios::in); // equivalent to file_access_mode::read_only
        data.read_contained_vals ("/testvectorfloat", vfread);
    }

    if (vf.size() == vfread.size()) {
        for (unsigned int i = 0; i < vf.size(); ++i) {
            if (vf[i] != vfread[i]) {
                rtn -= 1;
                break;
            }
        }
    }

    std::list<std::pair<double, double>> listofpairs;
    listofpairs.push_back (std::make_pair (1.0, 2.3));
    listofpairs.push_back (std::make_pair (1.3, 2.4));
    listofpairs.push_back (std::make_pair (1.5, 2.6));
    listofpairs.push_back (std::make_pair (1.9, 2.9));
    {
        sj::hdfdata data("test.h5", std::ios::out | std::ios::trunc);
        data.add_contained_vals ("/testlistofpairs", listofpairs);
    }
    std::list<std::pair<double, double>> listofpairs_read;
    {
        sj::hdfdata data("test.h5", std::ios::in);
        data.read_contained_vals ("/testlistofpairs", listofpairs_read);
    }
    if (listofpairs.size() == listofpairs_read.size()) {
        auto li = listofpairs.begin();
        auto lird = listofpairs_read.begin();
        while (li != listofpairs.end() && lird != listofpairs_read.end()) {
            if (li->first == lird->first && li->second == lird->second) {
                // Good
            } else {
                rtn -= 1;
            }
            li++;
            lird++;
        }

    } else {
        rtn -= 1;
    }

    std::pair<float, float> pr = { 3.0f, 6.0f };
    {
        sj::hdfdata data("test.h5", std::ios::out | std::ios::trunc);
        data.add_contained_vals ("/pair", pr);
    }
    std::pair<float, float> pr_rd;
    {
        sj::hdfdata data("test.h5", std::ios::in);
        data.read_contained_vals ("/pair", pr_rd);
    }
    if (pr_rd.first != pr_rd.first || pr_rd.second != pr_rd.second) {
        rtn -= 1;
    }

    if (rtn != 0) {
        std::cout << "Failed " << (-rtn) << " times" << std::endl;
    }

    return rtn;
}
