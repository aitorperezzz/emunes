#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "common/Logging.h"
#include "nes/Nes.h"

namespace nes
{

static constexpr size_t HEADER_SIZE = 16;
static constexpr size_t TRAINER_SIZE = 512;
static constexpr size_t PRG_ROM_UNIT_SIZE = 16384;
static constexpr size_t CHR_ROM_UNIT_SIZE = 8192;

Nes::Nes()
{
    mmio = std::make_shared<mmio::Mmio>();
    cpu = cpu::MOS6502(mmio);

    // Create a log file and pass it to all the interested components
    this->log_file = std::make_shared<common::LogFile>();
    cpu.set_log_file(log_file);
}

void Nes::set_log_filename(const std::string &filename)
{
    this->log_file->set_filename(filename);
}

bool Nes::insert_cartridge(const std::filesystem::path &filename)
{
    // Open the file to read
    std::ifstream rom_file(filename, std::ios_base::in | std::ios_base::binary);
    if (!rom_file.is_open())
    {
        common::Log(common::LogLevel::ERROR, "File " + filename.string() + " could not be opened");
        return false;
    }

    // Header (16 bytes)
    std::vector<uint8_t> header(16);
    rom_file.read(reinterpret_cast<char *>(header.data()), header.size());
    if (!(header[0] == 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == 0x1A))
    {
        common::Log(common::LogLevel::ERROR, "Header does not contain 'N+E+S+0x1A'");
        return false;
    }
    common::Log(common::LogLevel::DEBUG, "File is valid");
    const uint8_t prg_rom_num_units = header[4];
    const size_t prg_rom_size = prg_rom_num_units * PRG_ROM_UNIT_SIZE;
    common::Log(common::LogLevel::DEBUG, "PRG ROM size: " + std::to_string(+prg_rom_size));
    const uint8_t chr_rom_num_units = header[5];
    const size_t chr_rom_size = chr_rom_num_units * CHR_ROM_UNIT_SIZE;
    common::Log(common::LogLevel::DEBUG, "CHR ROM size: " + std::to_string(+chr_rom_size));
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

    // TODO: refactor this when more mappers are developed
    // Mapper 0 will send all the PRG ROM to the unmapped memory
    mmio->set_prg_rom(prg_rom);

    return true;
}

void Nes::override_reset_vector(const uint16_t address)
{
    cpu.override_reset_vector(address);
}

void Nes::set_max_instructions(const size_t num_instructions)
{
    cpu.set_max_instructions(num_instructions);
}

bool Nes::init()
{
    if (!cpu.reset())
    {
        common::Log(common::LogLevel::ERROR, "Could not reset MOS6502 CPU");
        return false;
    }
    return true;
}
} // namespace nes