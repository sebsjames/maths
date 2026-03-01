export module foo;

export import :part;

namespace test
{
    export class bar
    {
    public:
        void func();
    };

    export void func();
}