#ifndef CPU_OPCODE_H
#define CPU_OPCODE_H

#include <cstdint>
#include <map>

#include "InstructionId.h"
#include "AddressingMode.h"
#include "StatusRegisterBit.h"

namespace cpu
{
    struct Opcode
    {
        InstructionId instruction_id;
        AddressingMode addressing_mode;
        std::size_t instruction_size;
        std::size_t base_cycles;
    };

    class OpcodeParser
    {
    public:
        OpcodeParser();

        Opcode parse(const uint8_t byte);

        std::map<uint8_t, Opcode> opcodes;
    };
}

#endif