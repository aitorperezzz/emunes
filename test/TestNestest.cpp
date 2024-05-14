#include <iostream>

#include "cppunit/TestCase.h"
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"

#include "nes/Nes.h"

class TestNestest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestNestest);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST_SUITE_END();

  public:
    void test(void);
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestNestest);

void TestNestest::test(void)
{
    common::mute();
    std::cout << std::endl;

    std::string rom_filename = "roms/test/nestest/nestest.nes";
    std::string ref_filename = "roms/test/nestest/nestest.log";
    std::string out_filename = "nestest.output.log";
    const size_t max_instructions = 5003;

    // Create and configure all the elements in the emulator
    nes::Nes nes;
    nes.set_log_filename(out_filename);
    nes.insert_cartridge(rom_filename);
    nes.set_max_instructions(max_instructions);
    nes.override_reset_vector(0xC000);

    // Run
    nes.init();

    // Read both files, keeping only a fixed amount of characters per line
    const size_t num_characters = 73;
    std::ifstream ref_file(ref_filename);
    std::ifstream out_file(out_filename);
    std::string out_string, ref_string;
    for (size_t i = 0; i < max_instructions; i++)
    {
        // Read a new line from reference and output file
        if (!std::getline(ref_file, ref_string))
        {
            std::cout << "Test is supposed to run until instruction " << max_instructions << " but line " << i + 1
                      << " was not found in the reference file" << std::endl;
            CPPUNIT_ASSERT(false);
            return;
        }
        if (!std::getline(out_file, out_string))
        {
            std::cout << "Line number " << i + 1 << " is found in reference file and not in output file" << std::endl;
            CPPUNIT_ASSERT(false);
            return;
        }

        // Cut the lines to the required width
        out_string = out_string.substr(0, num_characters);
        ref_string = ref_string.substr(0, num_characters);

        if (ref_string != out_string)
        {
            std::cout << "Line number " << i + 1 << " does not match" << std::endl;
            std::cout << "Ref line: " << ref_string << std::endl;
            std::cout << "Out line: " << out_string << std::endl;
            CPPUNIT_ASSERT(false);
        }
    }
    std::cout << "Tested " << max_instructions << " lines of nestest.log" << std::endl;
}