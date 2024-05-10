#ifndef NES_NES_H
#define NES_NES_H

#include <filesystem>

#include "cpu/MOS6502.h"

namespace nes
{
class Nes
{
  public:
    Nes();

    /// @brief  Insert a cartridge in the NES and perform all the necessary housekeeping
    bool insert_cartridge(const std::filesystem::path &filename);

    /// @brief Press the power button
    bool init();

  private:
    /// @brief The MOS6502
    cpu::MOS6502 cpu;

    /// @brief Shared pointer to the address bus
    std::shared_ptr<mmio::Mmio> mmio;
};
} // namespace nes

#endif