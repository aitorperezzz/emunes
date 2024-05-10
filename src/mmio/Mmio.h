#ifndef MMIO_MMIO_H
#define MMIO_MMIO_H

#include <cstdint>
#include <vector>

namespace mmio
{
class Mmio
{
  public:
    Mmio();

    /// @brief Provide the program ROM to be stored by MMIO.
    /// TODO: this needs to be removed in the future
    /// @param prg_rom the PRG ROM as read from the cartridge
    void set_prg_rom(const std::vector<uint8_t> &prg_rom);

    /// @brief Get a value from the bus
    /// @param address The address selection
    /// @return The value read by teh bus at the specified address
    uint8_t get(const uint16_t address);

    /// @brief Set a value in the bus
    /// @param address The address selection
    /// @param value The value to store
    void set(const uint16_t address, const uint8_t value);

  private:
    /// @brief Internal CPU RAM memory (8 pages)
    std::vector<uint8_t> cpu_ram;

    /// @brief Internal copy of the PRG ROM as read fom the cartridge
    std::vector<uint8_t> prg_rom;
};
} // namespace mmio

#endif