#ifndef NES_NES_H
#define NES_NES_H

#include <filesystem>

#include "cpu/MOS6502.h"

namespace nes
{

/// @brief This class holds the rest of the systems together. It is the only
/// interface to the user
class Nes
{
  public:
    Nes();

    /// @brief Set the NES log file for all the internal components
    void set_log_filename(const std::string &filename);

    /// @brief Insert a cartridge in the NES and perform all the necessary housekeeping
    bool insert_cartridge(const std::filesystem::path &filename);

    /// @brief Override the default reset vector by directly providing a starting pc
    void override_reset_vector(const uint16_t address);

    /// @brief Set the total number of instructions that the CPU will execute before
    /// giving back control
    void set_max_instructions(const size_t num_instructions);

    /// @brief Press the power button
    bool init();

  private:
    /// @brief The MOS6502
    cpu::MOS6502 cpu;

    /// @brief Shared pointer to the address bus
    std::shared_ptr<mmio::Mmio> mmio;

    /// @brief Shared pointer to the system NES log file
    std::shared_ptr<common::LogFile> log_file;
};
} // namespace nes

#endif