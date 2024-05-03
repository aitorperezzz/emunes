#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "nes/Nes.h"

static constexpr size_t HEADER_SIZE = 16;
static constexpr size_t TRAINER_SIZE = 512;
static constexpr size_t PRG_ROM_UNIT_SIZE = 16384;
static constexpr size_t CHR_ROM_UNIT_SIZE = 8192;

namespace nes
{
    Nes::Nes()
    {
        mmio = std::make_shared<mmio::Mmio>();
        cpu = cpu::MOS6502(mmio);
    }

    bool Nes::insert_cartridge(const std::filesystem::path &filename)
    {
        // Open the file to read
        std::ifstream rom_file(filename, std::ios_base::in | std::ios_base::binary);

        // Header (16 bytes)
        std::vector<uint8_t> header(16);
        rom_file.read(reinterpret_cast<char *>(header.data()), header.size());
        if (!(header[0] == 'N' || header[1] == 'E' || header[2] == 'S' || header[3] == EOF))
        {
            std::cout << "Header does not contain 'NES+EOF'" << std::endl;
            return false;
        }
        std::cout << "File is valid" << std::endl;
        const uint8_t prg_rom_num_units = header[4];
        const size_t prg_rom_size = prg_rom_num_units * PRG_ROM_UNIT_SIZE;
        std::cout << "PRG ROM size: " << +prg_rom_size << std::endl;
        const uint8_t chr_rom_num_units = header[5];
        const size_t chr_rom_size = chr_rom_num_units * CHR_ROM_UNIT_SIZE;
        std::cout << "CHR ROM size: " << +chr_rom_size << std::endl;
        // Flags
        std::map<uint8_t, uint8_t> flags;
        for (uint8_t i = 6; i <= 10; i++)
        {
            flags.emplace(i, header[i]);
        }

        // Trainer
        // Trainer could be absent, we need to know if we should skip it
        bool trainer = (flags.at(6) >> 2) & 0x1;

        // PRG ROM
        size_t prg_rom_offset = trainer ? HEADER_SIZE + TRAINER_SIZE : HEADER_SIZE;
        rom_file.seekg(prg_rom_offset);
        std::vector<uint8_t> prg_rom(prg_rom_size);
        rom_file.read(reinterpret_cast<char *>(prg_rom.data()), prg_rom.size());

        // CHR ROM

        // PlayChoice INST-ROM if present

        // PlayChoice PROM if present

        return true;
    }

    bool Nes::init()
    {
        if (!cpu.reset())
        {
            std::cout << "Could not initialise MOS6502" << std::endl;
            return false;
        }
        return true;
    }
}