#ifndef CPU_MOS6502_H
#define CPU_MOS6502_H

#include <cstdint>
#include <map>
#include <memory>

#include "OpcodeParser.h"
#include "StatusRegisterBit.h"
#include "common/Logging.h"
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
    MOS6502(const std::shared_ptr<mmio::Mmio> &mmio);

    /// @brief Override the default reset vector with the provided pc.
    /// This function has to be called before reset
    void override_reset_vector(const uint16_t address);

    /// @brief Set a max number of instructions, after which the CPU will return
    /// execution control
    /// @param num_instructions Max number of instructions to execute
    void set_max_instructions(const size_t num_instructions);

    /// @brief Sets the log file object such that the CPU can add records to it
    void set_log_file(const std::shared_ptr<common::LogFile> &log_file);

    /// @brief Reset the chip. This kickstarts execution
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

    /// @brief Byte index 1 of the instruction, if fetched for the current opcode
    uint8_t instruction_byte_1;

    /// @brief Byte index 2 of the instruction, if fetched for the current opcode
    uint8_t instruction_byte_2;

    /// @brief Value read from the resolved address
    uint8_t value;

    /// @brief Intermediate address that will be filled by some addressing modes that
    /// use an intermediate address, like indirect addressing
    uint16_t intermediate_address;

    /// @brief Resolved address for the current addressing mode.
    /// This variable is only set if it makes sense to do so
    uint16_t address;

    /// @brief Flag indicating if the reset vector has been overriden
    bool rv_overriden = false;

    /// @brief If overriden, the CPU will start execution at this pc, and not at the one
    /// given by the reset vector
    uint16_t rv_value = 0;

    /// @brief Number of instructions to be executed by the CPU before quitting.
    /// If zero, the CPU will execute indefinitely
    size_t max_instructions = 0;

    /// @brief Link to mmio for all calls to the bus
    std::shared_ptr<mmio::Mmio> mmio;

    /// @brief Link to the official log file, to add records to it
    std::shared_ptr<common::LogFile> log_file;

    /// @brief The opcode parser that will decode bytes into opcodes
    OpcodeParser opcode_parser;

    /// @brief If true, the PC will advance, after the instruction execution, by as many
    /// bytes as the instruction size
    bool advance_pc = true;

    /// @brief This is the main loop of the CPU
    /// @return True if the operation was successful
    bool run();

    /// @brief Resolve the current addressing mode
    void resolve();

    /// @brief With the address resolved, fetch the required value from memory
    void fetch();

    /// @brief Execute the instruction indicated by the current opcode
    /// @return True if the operation was successful
    bool execute();

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

    /// @brief Disassemble the current status of the CPU. This operation should
    /// happen after the addressing has been resolved and the needed values have
    /// been fetched
    /// @return A complete CPU entry in the official NES log file
    std::string disassemble();
};
} // namespace cpu

#endif
