#ifndef CPU_MOS6502_H
#define CPU_MOS6502_H

#include <cstdint>
#include <map>
#include <memory>

#include "OpcodeParser.h"
#include "StatusRegisterBit.h"
#include "mmio/Mmio.h"

namespace cpu
{

/// @brief MOS technologies 6502 chip without decimal mode, as this mode
/// was not implemented in the chip included with the NES
class MOS6502
{
  public:
    MOS6502() = default;

    /// @brief Constructor
    /// @param mmio MMIO object
    MOS6502(const std::shared_ptr<mmio::Mmio> &mmio);

    /// @brief Reset the chip. This should kickstart it to run
    /// @return True if the operation was successful
    bool reset();

  private:
    /// @brief Program counter
    uint16_t pc;

    /// @brief Accumulator
    uint8_t acc;

    /// @brief X register
    uint8_t xr;

    /// @brief Y register
    uint8_t yr;

    /// @brief Status register
    /// bit 7: N Negative
    /// bit 6: V overflow
    /// bit 5: [ignored]
    /// bit 4: B break
    /// bit 3: D Decimal
    /// bit 2: I Interrupt
    /// bit 1: Zero
    /// bit 0: Carry
    uint8_t sr;

    /// @brief Stack pointer (from the stack offset)
    uint8_t sp;

    /// @brief The current opcode
    Opcode opcode;

    /// @brief Link to mmio for all calls to the bus
    std::shared_ptr<mmio::Mmio> mmio;

    /// @brief The opcode parser that will decode bytes into opcodes
    OpcodeParser opcode_parser;

    /// @brief If true, the PC will advance, after the instruction execution, by as many
    /// bytes as the instruction size
    bool advance_pc = true;

    /// @brief This is the main loop of the CPU
    /// @return True if the operation was successful
    bool run();

    /// @brief Execute the instruction indicated by the current opcode
    /// @return True if the operation was successful
    bool execute_instruction();

    /// @brief Return the specified bit in the status register
    /// @param bit The bit position that is requested
    /// @return The value stored at the specified position in the status register
    uint8_t get_sr_bit(StatusRegisterBit bit);

    /// @brief Set the corresponding bit in the status register
    /// @param bit The bit position that will be accessed
    /// @param value The value (a 0 or a 1)
    void set_sr_bit(StatusRegisterBit bit, const uint8_t value);

    /// @brief Push a value to the stack
    /// @param value The value to push
    void push_to_stack(uint8_t value);

    /// @brief Pull a value from the stack
    /// @return The value that was on top of the stack
    uint8_t pull_from_stack();

    /// @brief Perform add with carry
    /// @param value Value on which to perform the operation
    void adc(uint8_t value);

    /// @brief Return a string that contains the status of SR, ACC, X and Y
    std::string print_status();
};
} // namespace cpu

#endif
