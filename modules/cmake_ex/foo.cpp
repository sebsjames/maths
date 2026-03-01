module;

#include <iostream>

module foo;

namespace test
{
    void bar::func()
    {
        std::cout << "bar::func()" << "\n";
    }

    void func()
    {
        std::cout << "foo::func()" << "\n";
    }
}
