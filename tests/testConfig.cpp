#include <fstream>
#include <iostream>
#include <string>

#include <sm/config>

int main()
{
    std::string jsonfile ("./testConfig.json");

    std::ofstream f;
    f.open ("./testConfig.json", std::ios::out | std::ios::trunc);
    if (!f.is_open()) {
        std::cerr << "Failed to open a file to write the config JSON into\n";
        return -1;
    }
    f << "{\n"
      << "\"testbool\" : true,\n"
      << "\"testint\" : 27,\n"
      << "\"testfloat\" : 7.63\n"
      << "}\n";
    f.close();

    int rtn = -1;
    {
        sm::config config(jsonfile);
        if (config.ready) {
            const bool testbool = config.getBool ("testbool", false);
            std::cout << "testbool from JSON: " << (testbool ? "true" : "false") << " (expect: true)\n";
            const int testint = config.getInt ("testint", 3);
            std::cout << "testint from JSON: " << testint << " (expect: 27)\n";
            const float testfloat = config.getFloat ("testfloat", 9.8f);
            std::cout << "testfloat from JSON: " << testfloat << " (expect: 7.63)\n";

            if (testbool == true && testint == 27 && testfloat == 7.63f) {
                std::cout << "All test values were read correctly from the JSON.\n";
                rtn = 0;
            }
        }
    }

    // What if we open a config that doesn't exist?
    {
        sm::config config("non-existent.json");
        // If you open a non-existent json, then config.ready will be false
        std::cout << "config.ready? : " << config.ready << std::endl;
        if (config.ready) { --rtn; }

        // BUT, you can still can get functions, and you will get the defaults:
        std::cout << "Get non existent float field from non existent config: "
                  << config.get<float> ("imaginary", 1.0f) << std::endl;
        float itllbeone = config.get<float> ("imaginary", 1.0f);
        if (itllbeone != 1.0f) { --rtn; }
        std::cout << "Get non existent string field from non existent config: "
                  << config.get<std::string> ("imaginary chars", "The default") << std::endl;
        std::string str = config.get<std::string> ("imaginary chars", "The default");
        if (str != "The default") { --rtn; }

        // Get a vvec and it should be empty
        sm::vvec<float> vv = config.getvvec<float> ("a_name");
        std::cout << "We get a vvec from an empty config: " << vv << std::endl;
        if (!vv.empty()) { --rtn; }

        // Get a vec and it should be all zeros
        sm::vec<int32_t, 2> v = config.getvec<int32_t, 2> ("a_name");
        std::cout << "We get a vec from an empty config: " << v << std::endl;
        if (v[0] != 0 || v[1] != 0) { --rtn; }
    }

    std::cout << "sm::config test " << (rtn ? "FAILED\n" : "PASSED\n");
    return rtn;
}
