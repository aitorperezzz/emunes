#include <stdexcept>

#include "InstructionId.h"
#include "common/Logging.h"

namespace cpu
{
std::string print_instruction(const InstructionId instruction_id)
{
    switch (instruction_id)
    {

        // Load/store operations
    case InstructionId::LDA:
        return "LDA: Load accumulator";
    case InstructionId::LDX:
        return "LDX: Load X register";
    case InstructionId::LDY:
        return "LDY: Load Y register";
    case InstructionId::STA:
        return "STA: Store accumulator";
    case InstructionId::STX:
        return "STX: Store X register";
    case InstructionId::STY:
        return "STY: Store Y register";

        // Register transfers
    case InstructionId::TAX:
        return "TAX: Transfer accumulator to X";
    case InstructionId::TAY:
        return "TAY: Transfer accumulator to Y";
    case InstructionId::TXA:
        return "TXA: Transfer X to accumulator";
    case InstructionId::TYA:
        return "TYA: Transfer Y to accumulator";

        // Stack operations
    case InstructionId::TSX:
        return "TSX: Transfer stack pointer to X";
    case InstructionId::TXS:
        return "TXS: Transfer X to stack pointer";
    case InstructionId::PHA:
        return "PHA: Push accumulator on stack";
    case InstructionId::PHP:
        return "PHP: Push processor status on stack";
    case InstructionId::PLA:
        return "PLA: Pull accumulator from stack";
    case InstructionId::PLP:
        return "PLP: Pull processor status from stack";

        // Logical
    case InstructionId::AND:
        return "AND: Logical AND";
    case InstructionId::EOR:
        return "EOR: Exclusive OR";
    case InstructionId::ORA:
        return "ORA: Logical inclusive OR";
    case InstructionId::BIT:
        return "BIT: Bit test";

        // Arithmetic
    case InstructionId::ADC:
        return "ADC: Add with carry";
    case InstructionId::SBC:
        return "SBC: Subtract with carry";
    case InstructionId::CMP:
        return "CMP: Compare accumulator";
    case InstructionId::CPX:
        return "CPX: Compare X register";
    case InstructionId::CPY:
        return "CPY: Compare Y register";

        // Increments and decrements
    case InstructionId::INC:
        return "INC: Increment a memory location";
    case InstructionId::INX:
        return "INX: Increment the X register";
    case InstructionId::INY:
        return "INY: Increment the Y register";
    case InstructionId::DEC:
        return "DEC: Decrement a memory location";
    case InstructionId::DEX:
        return "DEX: Decrement the X register";
    case InstructionId::DEY:
        return "DEY: Decrement the Y register";

        // Shifts
    case InstructionId::ASL:
        return "ASL: Arithmetic shift left";
    case InstructionId::LSR:
        return "LSR: Logical shift right";
    case InstructionId::ROL:
        return "ROL: Rotate left";
    case InstructionId::ROR:
        return "ROR: Rotate right";

        // Jumps and calls
    case InstructionId::JMP:
        return "JMP: Jump to another location";
    case InstructionId::JSR:
        return "JSR: Jump to a subroutine";
    case InstructionId::RTS:
        return "RTS: Return from subroutine";

        // Branches
    case InstructionId::BCC:
        return "BCC: Branch if carry flag clear";
    case InstructionId::BCS:
        return "BCS: Branch if carry flag set";
    case InstructionId::BEQ:
        return "BEQ: Branch if zero flag set";
    case InstructionId::BMI:
        return "BMI: Branch if negative flag set";
    case InstructionId::BNE:
        return "BNE: Branch if zero flag clear";
    case InstructionId::BPL:
        return "BPL: Branch if negative flag clear";
    case InstructionId::BVC:
        return "BVC: Branch if overflow flag clear";
    case InstructionId::BVS:
        return "BVS: Branch if overflow flag set";

        // Status flag changes
    case InstructionId::CLC:
        return "CLC: Clear carry flag";
    case InstructionId::CLD:
        return "CLD: Clear decimal mode flag";
    case InstructionId::CLI:
        return "CLI: Clear interrupt disable flag";
    case InstructionId::CLV:
        return "CLV: Clear overflow flag";
    case InstructionId::SEC:
        return "SEC: Set carry flag";
    case InstructionId::SED:
        return "SED: Set decimal mode flag";
    case InstructionId::SEI:
        return "SEI: Set interrupt disable flag";

        // System functions
    case InstructionId::BRK:
        return "BRK: Force an interrupt";
    case InstructionId::NOP:
        return "NOP: No operation";
    case InstructionId::RTI:
        return "RTI: Return from interrupt";

    default:
        uint8_t instruction_id_raw = (uint8_t)instruction_id;
        throw std::out_of_range("Instruction id not understood: " +
                                common::print_hex(instruction_id_raw, sizeof(instruction_id_raw)));
    }
}
} // namespace cpu