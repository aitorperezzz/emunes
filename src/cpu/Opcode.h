#ifndef CPU_OPCODE_H
#define CPU_OPCODE_H

#include <cstdint>
#include <vector>

#include "InstructionId.h"
#include "AddressingMode.h"
#include "Types.h"
#include "StatusRegisterBit.h"

namespace cpu
{
    class Opcode
    {
    public:
        Opcode() = default;

        Opcode(uint8_t byte);

        InstructionId instruction_id;

        AddressingMode addressing_mode;

        InstructionSize instruction_size;

        std::vector<StatusRegisterBit> status_register_bits;
    };
}

#endif