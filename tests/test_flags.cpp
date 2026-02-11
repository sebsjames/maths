#include <cstdint>
#include <iostream>
#include <sm/flags>

// You need an enumerated class to use sm::flags.
enum class myflags : uint32_t
{
    one,
    two,
    three,
    four
};

enum class myflags2 : uint16_t
{
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    eight
};

enum class myflags3 : int8_t
{
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    eight
};

int main()
{
    int rtn = 0;

    sm::flags<myflags> fl;
    fl.set (myflags::one);
    if (fl.test(myflags::one)) {
        std::cout << "flags: one is set" << std::endl;
    } else {
        std::cout << "flags: one is not set" << std::endl;
    }
    std::cout << "After set one, fl bits: " << fl.get() << std::endl;
    std::cout << "Bit count: " << fl.count() << std::endl;
    if (fl.get() != uint32_t{1}) { --rtn; }

    fl.reset (myflags::one);
    if (fl.test(myflags::one)) {
        std::cout << "flags: one is set" << std::endl;
    } else {
        std::cout << "flags: one is not set" << std::endl;
    }
    std::cout << "After reset one, fl bits: " << fl.get() << std::endl;
    std::cout << "Bit count: " << fl.count() << std::endl;
    if (fl.get() != uint32_t{0}) { --rtn; }

    fl.reset (myflags::one);
    if (fl.test(myflags::one)) {
        std::cout << "flags: one is set" << std::endl;
    } else {
        std::cout << "flags: one is not set" << std::endl;
    }
    std::cout << "After another reset one, fl bits: " << fl.get() << std::endl;
    std::cout << "Bit count: " << fl.count() << std::endl;
    if (fl.get() != uint32_t{0}) { --rtn; }

    fl |= myflags::one;
    fl |= (myflags::two);
    if (fl.test(myflags::two)) {
        std::cout << "flags: two is set" << std::endl;
    } else {
        std::cout << "flags: two is not set" << std::endl;
    }
    std::cout << "Streaming flags: " << fl << std::endl;
    std::cout << "After set two, fl bits: " << fl.get() << std::endl;
    std::cout << "Bit count: " << fl.count() << std::endl;
    if (fl.get() != uint32_t{3}) { --rtn; }

    fl |= (myflags::three);
    if (fl.test(myflags::three)) {
        std::cout << "flags: three is set" << std::endl;
    } else {
        std::cout << "flags: three is not set" << std::endl;
    }
    std::cout << "After set three, fl bits: " << fl.get() << std::endl;
    std::cout << "Bit count: " << fl.count() << std::endl;
    if (fl.get() != uint32_t{7}) { --rtn; }

    if (fl.none() == true) { --rtn; }
    if (fl.any() == false) { --rtn; }

    fl |= (myflags::four);
    if (fl.test(myflags::four)) {
        std::cout << "flags: four is set" << std::endl;
    } else {
        std::cout << "flags: four is not set" << std::endl;
    }
    std::cout << "After set four, fl bits: " << fl.get() << std::endl;
    std::cout << "Bit count: " << fl.count() << std::endl;
    std::cout << "fl: " << fl << std::endl;
    if (fl.get() != uint32_t{15}) { --rtn; }

    // Test with operator
    if ((fl & sm::flags<myflags>(myflags::two)) != sm::flags<myflags>(myflags::two)) { --rtn; }

    // Test with test() is neater:
    if (fl.test (myflags::two) == false) { --rtn; }

    int ii = 1;
    std::cout << "1 ^ -1 is " << (ii ^ -1) << std::endl;

#if 0
    fl &= ~(sm::flags<myflags>(myflags::two));
    if (fl.test (myflags::two) == true) { --rtn; }
    fl |= myflags::two;
#endif

    // Test multiple flags:
    if (fl.test ({myflags::two, myflags::three})) {
        std::cout << "Flags: two and three are set\n";
    } else { --rtn; }

    // flip 2
    fl ^= myflags::two;
    if (fl.get() != uint32_t{13}) { --rtn; }

    // Make sure a test of multiple flags when one is known to be unset does not return true
    if (fl.test ({myflags::two, myflags::three})) {
        std::cout << "Flags: After flip2, two and three are set\n";
        --rtn;
    }

    // Save flags state:
    sm::flags<myflags> fl_save = fl;

    // Test equality/nonequality operators
    if ((fl_save == fl) == false) { --rtn; }
    if ((fl_save != fl) == true) { --rtn; }

    // More multiple flags set/reset/test attempts:
    fl.reset();
    fl.set ({myflags::two, myflags::three});

    // More equality testing
    if (fl == myflags::two) { --rtn; }
    if (fl == myflags::three) { --rtn; }
    // You can OR flags together like this, but it's ugly!
    if (fl != (sm::flags<myflags>(myflags::two) | sm::flags<myflags>(myflags::three))) { --rtn; }
    if (fl == (sm::flags<myflags>(myflags::two) | sm::flags<myflags>(myflags::four))) { --rtn; }

    if (fl.test ({myflags::two, myflags::three})) {
        std::cout << "Flags: After set (two, three), two and three are set\n";
    } else { --rtn; }
    if (fl.test (myflags::one)) { --rtn; }
    if (fl.test (myflags::four)) { --rtn; }

    fl.set ({myflags::two, myflags::three}, false);
    if (fl.test (myflags::two)) { --rtn; }
    if (fl.test (myflags::three)) { --rtn; }

    fl.set ({myflags::one, myflags::three});
    fl.reset ({myflags::one, myflags::three});
    if (fl.any()) { --rtn; }

    // Restore state
    fl = fl_save;

    if (fl.test (myflags::two) == true) { --rtn; }
    if (fl.test (myflags::one) == false) { --rtn; }

    fl.reset();
    std::cout << "After reset, fl bits: " << fl.get() << std::endl;
    std::cout << "Bit count: " << fl.count() << std::endl;
    if (fl.get() != uint32_t{0}) { --rtn; }

    if (fl.none() == false) { --rtn; }
    if (fl.any() == true) { --rtn; }

    sm::flags<myflags> fl2;
    fl2.set (myflags::three);
    fl2.set (myflags::one);
    std::cout << "fl2 = " << fl2.get() << std::endl;

    sm::flags fl3 = (fl2 | myflags::four); // the type myflags is deduced by the compiler
    fl3 |= myflags::two;
    if (fl3.get() != uint32_t{15}) { --rtn; }

    // More operator style stuff
    sm::flags<myflags> osfl;
    osfl = (osfl | myflags::one | myflags::two);
    std::cout << "osfl : " << osfl << std::endl;
    //osfl &= ~<myflags>(myflags::one);
    //osfl &= sm::flags<myflags>::~(myflags::one);

    sm::flags osfl1 = myflags::four | osfl; // invokes external operator
    std::cout << "osfl : " << osfl << std::endl;
    std::cout << "osfl1: " << osfl1 << std::endl;

    sm::flags osfl3 = osfl1 & myflags::four; // member bitwise operator
    std::cout << "osfl3: " << osfl3 << std::endl;

    sm::flags osfl2 = myflags::four & osfl1; // invokes external operator
    std::cout << "osfl2: " << osfl2 << std::endl;

    sm::flags osfl4 = osfl1 ^ myflags::one; // member bitwise operator
    std::cout << "osfl4: " << osfl4 << std::endl;
    osfl4 = osfl4 ^ myflags::one;
    std::cout << "osfl4: " << osfl4 << std::endl;

#if 1
    // Trying to invoke
    /*
    template <typename E> requires std::is_enum_v<E>
    inline constexpr flags<E> operator&(E lhs, E rhs) noexcept
    {
        return flags<E>(lhs) & rhs;
    }
    */
    sm::flags<myflags> osfl5 = operator& <myflags> (myflags::one, myflags::four);
    std::cout << "osfl5 " << osfl5 << std::endl;
#endif

    // Invokes the external operator:
    // template <typename E> requires std::is_enum_v<E>
    // constexpr flags<E> operator&(E flag, const flags<E>& flags) noexcept
    sm::flags fl4 = myflags::four & fl3;

    // This invokes the member bitwise operator
    // sm::flags fl4 = fl3 & myflags::four;
    std::cout << "fl4.get() = " << fl4.get() << " cf " << (8 & fl3.get()) << std::endl;
    if (fl4.get() != uint32_t{8}) { --rtn; }

    if (sm::flags<myflags>(myflags::one).get() != 1) { --rtn; }
    if (sm::flags<myflags>(myflags::two).get() != 2) { --rtn; }
    if (sm::flags<myflags>(myflags::three).get() != 4) { --rtn; }
    if (sm::flags<myflags>(myflags::four).get() != 8) { --rtn; }
    std::cout << "myflags::one : " << sm::flags<myflags>(myflags::one).get() << std::endl;
    std::cout << "myflags::two : " << sm::flags<myflags>(myflags::two).get() << std::endl;
    std::cout << "myflags::three : " << sm::flags<myflags>(myflags::three).get() << std::endl;
    std::cout << "myflags::four : " << sm::flags<myflags>(myflags::four).get() << std::endl;

    std::cout << "myflags::three streamed: " << sm::flags<myflags>(myflags::three) << std::endl;

    // Test constructor with type E (myflags2) arg
    sm::flags<myflags2> fl5 (myflags2::four);
    if (fl5.get() != 8) { --rtn; }
    std::cout << "A flags object with underlying type uint16_t: " << fl5 << std::endl;

    sm::flags<myflags3> fl5i (myflags3::four);
    if (fl5i.get() != 8) { --rtn; }
    std::cout << "A flags object with underlying type  int8_t: " << fl5i << std::endl;

    // Test copy constructor (template deduction works)
    sm::flags fl6 (fl5);
    if (fl6.get() != 8) { --rtn; }

    fl6 = fl5;
    if (fl6.get() != 8) { --rtn; }

    // Test constructor with passed in underlying type (template deduction not possible)
    sm::flags<myflags2> fl7 (uint16_t{4});
    if (fl7.get() != 4) { --rtn; }

    std::cout << "fl5 = " << fl5.get() << " !fl5 = " << !fl5 << std::endl;
    std::cout << "fl5 = " << fl5.get() << " !fl5 = " << !fl5 << std::endl;

    enum class myflagsC : uint8_t { one, two, three, four };
    std::cout << sm::flags<myflagsC>{myflagsC::one} << std::endl;
    std::cout << sm::flags<myflagsC>{myflagsC::three} << std::endl;

    std::cout << (rtn ? "Failed\n" : "Success\n");
    return rtn;
}
