#ifndef CPU_MOS6502_H
#define CPU_MOS6502_H

#include <cstdint>
#include <vector>

#include "StatusRegisterBit.h"
#include "mmio/Mmio.h"
#include "Opcode.h"

namespace cpu
{

    constexpr uint16_t STACK_ADDRESS = 0x0100;

    class MOS6502
    {
    private:
        /// @brief Program counter
        uint8_t pc;

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

        /// @brief Stack pointer
        uint8_t sp;

        Opcode opcode;

        std::vector<uint8_t> instructions;

        mmio::Mmio mmio;

        bool execute();

        bool update_opcode();

        bool execute_instruction();

        /// @brief Return the specified bit in the status register
        /// @param bit The bit position that is requested
        /// @return The value stored at the specified position in the status register
        uint8_t get_sr_bit(StatusRegisterBit bit);

        void set_sr_bit(StatusRegisterBit bit);

        void clear_sr_bit(StatusRegisterBit bit);

        void push_to_stack(uint8_t value);

        uint8_t pull_from_stack();

        void adc(uint8_t value);
    };
}

#endif
