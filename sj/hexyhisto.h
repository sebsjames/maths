/*
 * A histogram class for a 2D histogram on a HexGrid.
 */
#pragma once

#include <sj/vec.h>
#include <sj/vvec.h>
#include <sj/HexGrid.h>
#include <utility>

namespace sj {

    template <typename T=float>
    struct hexyhisto
    {
        // Data is a vvec of coordinates. data[2] is ignored. hg is a hex grid, assumed to be in
        // same coordinate frame as data.
        hexyhisto (const sj::vvec<sj::vec<T>>& data, HexGrid* hg)
        {
            unsigned int n = hg->num();
            this->counts.resize(n, 0);
            this->proportions.resize(n, T{0});

            // For each coordinate, add it to a hex
            for (const sj::vec<T, 3>& datum : data) {
                if (datum[2] < 0.0f) { continue; }
                // if datum is in a hex hi, then counts[hi->vg] += T{1};
                auto hi = hg->findHexNearest (datum.less_one_dim());

                // dist from hi to datum:
                sj::vec<T> hipos = { hi->x, hi->y, 0 };
                T _d = (hipos - datum).length();
                if (_d <= hg->getv()) {
                    counts[hi->vi] += T{1};
                    //std::cout << "crossing at " << datum << " has nearest hex: " << hi->outputXY() << " for which count = " << counts[hi->vi] << std::endl;
                    this->datacount++;
                }
            }
            this->proportions = counts.as_float();
            this->proportions /= this->datacount;
            // Now just plot hexyhisto::proportions on your HexGrid. Simples.
        }

        T datacount = T{0}; // how many elements were there in data?
        sj::vvec<T> counts;
        sj::vvec<T> proportions;
    };
}
