#include <string>
#include <iostream>
#include <fstream>
#include <sm/vec>
#include <sm/util>

int main()
{
    int rtn = 0;

    // Test binary read and write
    std::string filename = "util1_testfile";

    // Write out
    {
        std::ofstream fout (filename, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!fout.is_open()) {
            std::cout << "Failed to open for write\n";
            return -1;
        }

        float f = 4.2f;
        double d = 67.2;
        uint32_t u32 = 324953;
        uint64_t u64 = 19384938324;

        sm::util::binary_write (fout, f);
        sm::util::binary_write (fout, d);
        sm::util::binary_write (fout, u32);
        sm::util::binary_write (fout, u64);
        sm::vec<float, 4> vf4 = { 1.0f, 2.0f, 3.0f, 4.0f };
        sm::util::binary_write (fout, vf4);

        sm::vec<int32_t, 4> vi3 = { 10, 30, 50 };
        sm::util::binary_write (fout, vi3);
    }

    // Read back
    {
        std::ifstream fin (filename, std::ios::binary | std::ios::in);
        if (!fin.is_open()) {
            std::cout << "Failed to open for read\n";
            return -1;
        }

        float f = 0.0f;
        sm::util::binary_read (fin, f);
        if (f != 4.2f) { --rtn; }

        double d = 0.0;
        sm::util::binary_read (fin, d);
        if (d != 67.2) { --rtn; }

        uint32_t u32 = 0u;
        sm::util::binary_read (fin, u32);
        if (u32 != 324953) { --rtn; }

        uint64_t u64 = 0u;
        sm::util::binary_read (fin, u64);
        if (u64 != 19384938324) { --rtn; }

        sm::vec<float, 4> vf4 = {};
        sm::util::binary_read (fin, vf4);
        std::cout << "vf4: " << vf4 << std::endl;
        if (vf4[0] != 1.0f || vf4[1] != 2.0f || vf4[2] != 3.0f || vf4[3] != 4.0f) { --rtn; }

        sm::vec<int32_t, 3> vi3 = {};
        sm::util::binary_read (fin, vi3);
        std::cout << "vi3: " << vi3 << std::endl;
        if (vi3[0] != 10 || vi3[1] != 30 || vi3[2] != 50) { --rtn; }
    }

    std::cout << "Test " << (rtn ? "FAILED" : "PASSED") << std::endl;
    return rtn;
}
