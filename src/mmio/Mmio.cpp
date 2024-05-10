#include <iomanip>
#include <stdexcept>

#include "Mmio.h"
#include "common/Logging.h"

namespace mmio
{
/// *********************************
/// CPU RAM (32 pages)
/// *********************************

/// CPU RAM size is 2KB = 2048 bytes (8 pages in total)
/// It starts at zero.
/// It is mirrored in 4 banks for a total of 32 pages
static constexpr uint16_t CPU_RAM_START = 0x0000;
static constexpr uint16_t CPU_RAM_SIZE = 0x0800;
static constexpr uint16_t CPU_RAM_MIRRORS = 4;

/// *********************************
/// PPU registers (32 pages)
/// *********************************

static constexpr uint16_t PPU_START = 0x2000;
static constexpr uint16_t PPU_SIZE = 0x0008;
static constexpr uint16_t PPU_MIRRORS = 1024;

/// *********************************
/// APU and IO registers (24 bytes)
/// *********************************

static constexpr uint16_t APU_IO_START = 0x4000;
static constexpr uint16_t APU_IO_SIZE = 0x0018;

/// *********************************
/// Normally disabled (8 bytes)
/// *********************************

static constexpr uint16_t DISABLED_START = 0x4018;
static constexpr uint16_t DISABLED_SIZE = 0x0008;

/// *********************************
/// Unmapped (191 pages + 224 bytes)
/// *********************************

static constexpr uint16_t UNMAPPED_START = 0x4020;
static constexpr uint16_t UNMAPPED_SIZE = 0xBFE0;

/// Really unused data (31 pages + 224 bytes)

/// Cartridge RAM (32 pages)
static constexpr uint16_t CARTRIDGE_RAM_START = 0x6000;
static constexpr uint16_t CARTRIDGE_RAM_SIZE = 0x2000;

/// PRG ROM (128 pages)
static constexpr uint16_t CARTRIDGE_ROM_START = 0x8000;
static constexpr uint16_t CARTRIDGE_ROM_SIZE = 0x4000;
static constexpr uint16_t CARTRIDGE_ROM_MIRRORS = 2;

Mmio::Mmio()
{
    cpu_ram.resize(CPU_RAM_SIZE);
}

void Mmio::set_prg_rom(const std::vector<uint8_t> &prg_rom)
{
    this->prg_rom = prg_rom;
}

uint8_t Mmio::get(const uint16_t address)
{
    // CPU RAM
    if (address >= CPU_RAM_START && address < CPU_RAM_SIZE * CPU_RAM_MIRRORS)
    {
        uint8_t value = cpu_ram[address % CPU_RAM_SIZE];
        common::Log(common::LogLevel::DEBUG, "Read from CPU RAM, address " +
                                                 common::print_hex(address, sizeof(address)) + ", value " +
                                                 common::print_hex(value, sizeof(value)));
        return value;
    }
    // PPU registers
    else if (address >= PPU_START && address < PPU_START + PPU_SIZE * PPU_MIRRORS)
    {
        common::Log(common::LogLevel::WARNING,
                    "Cannot read from PPU registers, address " + common::print_hex(address, sizeof(address)));
    }
    // APU/IO area
    else if (address >= APU_IO_START && address < APU_IO_START + APU_IO_SIZE)
    {
        common::Log(common::LogLevel::WARNING,
                    "Cannot read from APU/IO registers, address " + common::print_hex(address, sizeof(address)));
    }
    // Disabled area
    else if (address >= DISABLED_START && address < DISABLED_START + DISABLED_SIZE)
    {
        common::Log(common::LogLevel::WARNING,
                    "Cannot read from disabled area, address " + common::print_hex(address, sizeof(address)));
    }
    // Unmapped area (available for cartridge use)
    else if (address >= UNMAPPED_START && address < UNMAPPED_START + UNMAPPED_SIZE)
    {
        // For the moment, this goes directly to PRG ROM
        if (address >= CARTRIDGE_ROM_START &&
            address < CARTRIDGE_ROM_START + CARTRIDGE_ROM_SIZE * CARTRIDGE_ROM_MIRRORS)
        {
            uint8_t value = prg_rom[(address - CARTRIDGE_ROM_START) % CARTRIDGE_ROM_SIZE];
            common::Log(common::LogLevel::DEBUG, "Read from cartridge ROM, address " +
                                                     common::print_hex(address, sizeof(address)) + ", value " +
                                                     common::print_hex(value, sizeof(value)));
            return value;
        }
    }

    return 0;
}

void Mmio::set(const uint16_t address, const uint8_t value)
{
    // CPU RAM
    if (address >= CPU_RAM_START && address < CPU_RAM_SIZE * CPU_RAM_MIRRORS)
    {
        common::Log(common::LogLevel::DEBUG, "Write to CPU RAM, address " +
                                                 common::print_hex(address, sizeof(address)) + ", value " +
                                                 common::print_hex(value, sizeof(value)));
        cpu_ram[address % CPU_RAM_SIZE] = value;
    }
    // PPU registers
    else if (address >= PPU_START && address < PPU_START + PPU_SIZE * PPU_MIRRORS)
    {
        common::Log(common::LogLevel::WARNING,
                    "Cannot write to PPU registers, address " + common::print_hex(address, sizeof(address)));
    }
    // APU/IO area
    else if (address >= APU_IO_START && address < APU_IO_START + APU_IO_SIZE)
    {
        common::Log(common::LogLevel::WARNING,
                    "Cannot write to APU/IO registers, address " + common::print_hex(address, sizeof(address)));
    }
    // Disabled area
    else if (address >= DISABLED_START && address < DISABLED_START + DISABLED_SIZE)
    {
        common::Log(common::LogLevel::WARNING,
                    "Cannot write to disabled memory area, address " + common::print_hex(address, sizeof(address)));
    }
    // Unmapped area (available for cartridge use)
    else if (address >= UNMAPPED_START && address < UNMAPPED_START + UNMAPPED_SIZE)
    {
        // For the moment, this goes directly to PRG ROM
        if (address >= CARTRIDGE_ROM_START &&
            address < CARTRIDGE_ROM_START + CARTRIDGE_ROM_SIZE * CARTRIDGE_ROM_MIRRORS)
        {
            common::Log(common::LogLevel::WARNING,
                        "Write to cartridge ROM, address " + common::print_hex(address, sizeof(address)));
        }
    }
}
} // namespace mmio