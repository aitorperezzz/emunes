#include <filesystem>
#include <iostream>

#include "common/Logging.h"
#include "nes/Nes.h"

int main(int argc, char *argv[])
{
    // The ROM filename is the only argument to this program
    if (argc != 2)
    {
        common::Log(common::LogLevel::ERROR, "Provide a ROM filename: ./emunes <rom_filename>");
        return -1;
    }
    std::filesystem::path rom_filename = argv[1];

    // Create a NES emulator
    nes::Nes nes;
    // TODO: choose a better filename
    nes.set_log_filename("nestest.log");

    // Insert the cartridge (check file consistency, prepare mmio, etc...)
    if (!nes.insert_cartridge(rom_filename))
    {
        common::Log(common::LogLevel::ERROR, "ROM cartridge loading failed");
        return -1;
    }

    // Execute
    return nes.init();
}