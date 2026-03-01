import foo;

int main()
{
    // module class
    test::bar b;
    b.func();

    // module free function
    test::func();

    // partition class
    test::thing t;
    t.func();

    // partition free function
    test::part();
}
