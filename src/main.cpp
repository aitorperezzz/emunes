#include <filesystem>
#include <iostream>

#include "nes/Nes.h"

int main(int argc, char *argv[])
{
    // The ROM filename is the only argument to this program
    if (argc != 2)
    {
        std::cout << "Provide a ROM filename: ./emunes <rom_filename>" << std::endl;
        return -1;
    }
    std::filesystem::path rom_filename = argv[1];

    // Create a NES emulator feeding it the name of the file
    nes::Nes nes;
    nes.insert_cartridge(rom_filename);
    return nes.init();
}