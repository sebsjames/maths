module;

#include <iostream>

// NOTE: Do not use the partition name here; use the module
// the partition belongs to
// module foo:part; <= wrong

// correct (the module the partition belongs to)
module foo;

namespace test
{
    void thing::func()
    {
        std::cout << "thing::func()" << "\n";
    }

    void part()
    {
        std::cout << "part1()" << "\n";
    }
}
