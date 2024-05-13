#include <stdexcept>

#include "OpcodeParser.h"
#include "common/Logging.h"

namespace cpu
{
OpcodeParser::OpcodeParser()
{
    // Load/store operations

    // LDA - Load Accumulator
    add(0xA9, InstructionId::LDA, AddressingMode::IMM, 2, 2);
    add(0xA5, InstructionId::LDA, AddressingMode::ZP0, 2, 3);
    add(0xB5, InstructionId::LDA, AddressingMode::ZPX, 2, 4);
    add(0xAD, InstructionId::LDA, AddressingMode::ABS, 3, 4);
    add(0xBD, InstructionId::LDA, AddressingMode::ABX, 3, 4);
    add(0xB9, InstructionId::LDA, AddressingMode::ABY, 3, 4);
    add(0xA1, InstructionId::LDA, AddressingMode::IXI, 2, 6);
    add(0xB1, InstructionId::LDA, AddressingMode::IIX, 2, 5);
    // LDX - Load X Register
    add(0xA2, InstructionId::LDX, AddressingMode::IMM, 2, 2);
    add(0xA6, InstructionId::LDX, AddressingMode::ZP0, 2, 3);
    add(0xB6, InstructionId::LDX, AddressingMode::ZPY, 2, 4);
    add(0xAE, InstructionId::LDX, AddressingMode::ABS, 3, 4);
    add(0xBE, InstructionId::LDX, AddressingMode::ABY, 3, 4);
    // LDY - Load Y Register
    add(0xA0, InstructionId::LDY, AddressingMode::IMM, 2, 2);
    add(0xA4, InstructionId::LDY, AddressingMode::ZP0, 2, 3);
    add(0xB4, InstructionId::LDY, AddressingMode::ZPX, 2, 4);
    add(0xAC, InstructionId::LDY, AddressingMode::ABS, 3, 4);
    add(0xBC, InstructionId::LDY, AddressingMode::ABX, 3, 4);
    // STA - Store Accumulator
    add(0x85, InstructionId::STA, AddressingMode::ZP0, 2, 3);
    add(0x95, InstructionId::STA, AddressingMode::ZPX, 2, 4);
    add(0x8D, InstructionId::STA, AddressingMode::ABS, 3, 4);
    add(0x9D, InstructionId::STA, AddressingMode::ABX, 3, 5);
    add(0x99, InstructionId::STA, AddressingMode::ABY, 3, 5);
    add(0x81, InstructionId::STA, AddressingMode::IXI, 2, 6);
    add(0x91, InstructionId::STA, AddressingMode::IIX, 2, 6);
    // STX - Store X Register
    add(0x86, InstructionId::STX, AddressingMode::ZP0, 2, 3);
    add(0x96, InstructionId::STX, AddressingMode::ZPY, 2, 4);
    add(0x8E, InstructionId::STX, AddressingMode::ABS, 3, 4);
    // STY - Store Y Register
    add(0x84, InstructionId::STY, AddressingMode::ZP0, 2, 3);
    add(0x94, InstructionId::STY, AddressingMode::ZPX, 2, 4);
    add(0x8C, InstructionId::STY, AddressingMode::ABS, 3, 4);

    // Register transfers

    // TAX - Transfer Accumulator to X
    add(0xAA, InstructionId::TAX, AddressingMode::IMP, 1, 2);
    // TAY - Transfer Accumulator to Y
    add(0xA8, InstructionId::TAY, AddressingMode::IMP, 1, 2);
    // TXA - Transfer X to Accumulator
    add(0x8A, InstructionId::TXA, AddressingMode::IMP, 1, 2);
    // TYA - Transfer Y to Accumulator
    add(0x98, InstructionId::TYA, AddressingMode::IMP, 1, 2);

    // Stack operations

    // TSX - Transfer Stack Pointer to X
    add(0xBA, InstructionId::TSX, AddressingMode::IMP, 1, 2);
    // TXS - Transfer X to Stack Pointer
    add(0x9A, InstructionId::TXS, AddressingMode::IMP, 1, 2);
    // PHA - Push Accumulator
    add(0x48, InstructionId::PHA, AddressingMode::IMP, 1, 3);
    // PHP - Push Processor Status
    add(0x08, InstructionId::PHP, AddressingMode::IMP, 1, 3);
    // PLA - Pull Accumulator
    add(0x68, InstructionId::PLA, AddressingMode::IMP, 1, 4);
    // PLP - Pull Processor Status
    add(0x28, InstructionId::PLP, AddressingMode::IMP, 1, 4);

    // Logical

    // AND - Logical AND
    add(0x29, InstructionId::AND, AddressingMode::IMM, 2, 2);
    add(0x25, InstructionId::AND, AddressingMode::ZP0, 2, 3);
    add(0x35, InstructionId::AND, AddressingMode::ZPX, 2, 4);
    add(0x2D, InstructionId::AND, AddressingMode::ABS, 3, 4);
    add(0x3D, InstructionId::AND, AddressingMode::ABX, 3, 4);
    add(0x39, InstructionId::AND, AddressingMode::ABY, 3, 4);
    add(0x21, InstructionId::AND, AddressingMode::IXI, 2, 6);
    add(0x31, InstructionId::AND, AddressingMode::IIX, 2, 5);
    // EOR - Exclusive OR
    add(0x49, InstructionId::EOR, AddressingMode::IMM, 2, 2);
    add(0x45, InstructionId::EOR, AddressingMode::ZP0, 2, 3);
    add(0x55, InstructionId::EOR, AddressingMode::ZPX, 2, 4);
    add(0x4D, InstructionId::EOR, AddressingMode::ABS, 3, 4);
    add(0x5D, InstructionId::EOR, AddressingMode::ABX, 3, 4);
    add(0x59, InstructionId::EOR, AddressingMode::ABY, 3, 4);
    add(0x41, InstructionId::EOR, AddressingMode::IXI, 2, 6);
    add(0x51, InstructionId::EOR, AddressingMode::IIX, 2, 5);
    // ORA - Logical Inclusive OR
    add(0x09, InstructionId::ORA, AddressingMode::IMM, 2, 2);
    add(0x05, InstructionId::ORA, AddressingMode::ZP0, 2, 3);
    add(0x15, InstructionId::ORA, AddressingMode::ZPX, 2, 4);
    add(0x0D, InstructionId::ORA, AddressingMode::ABS, 3, 4);
    add(0x1D, InstructionId::ORA, AddressingMode::ABX, 3, 4);
    add(0x19, InstructionId::ORA, AddressingMode::ABY, 3, 4);
    add(0x01, InstructionId::ORA, AddressingMode::IXI, 2, 6);
    add(0x11, InstructionId::ORA, AddressingMode::IIX, 2, 5);
    // BIT - Bit Test
    add(0x24, InstructionId::BIT, AddressingMode::ZP0, 2, 3);
    add(0x2C, InstructionId::BIT, AddressingMode::ABS, 3, 4);

    // Arithmetic

    // ADC - Add with Carry
    add(0x69, InstructionId::ADC, AddressingMode::IMM, 2, 2);
    add(0x65, InstructionId::ADC, AddressingMode::ZP0, 2, 3);
    add(0x75, InstructionId::ADC, AddressingMode::ZPX, 2, 4);
    add(0x6D, InstructionId::ADC, AddressingMode::ABS, 3, 4);
    add(0x7D, InstructionId::ADC, AddressingMode::ABX, 3, 4);
    add(0x79, InstructionId::ADC, AddressingMode::ABY, 3, 4);
    add(0x61, InstructionId::ADC, AddressingMode::IXI, 2, 6);
    add(0x71, InstructionId::ADC, AddressingMode::IIX, 2, 5);
    // SBC - Subtract with Carry
    add(0xE9, InstructionId::SBC, AddressingMode::IMM, 2, 2);
    add(0xE5, InstructionId::SBC, AddressingMode::ZP0, 2, 3);
    add(0xF5, InstructionId::SBC, AddressingMode::ZPX, 2, 4);
    add(0xED, InstructionId::SBC, AddressingMode::ABS, 3, 4);
    add(0xFD, InstructionId::SBC, AddressingMode::ABX, 3, 4);
    add(0xF9, InstructionId::SBC, AddressingMode::ABY, 3, 4);
    add(0xE1, InstructionId::SBC, AddressingMode::IXI, 2, 6);
    add(0xF1, InstructionId::SBC, AddressingMode::IIX, 2, 5);
    // CMP - Compare
    add(0xC9, InstructionId::CMP, AddressingMode::IMM, 2, 2);
    add(0xC5, InstructionId::CMP, AddressingMode::ZP0, 2, 3);
    add(0xD5, InstructionId::CMP, AddressingMode::ZPX, 2, 4);
    add(0xCD, InstructionId::CMP, AddressingMode::ABS, 3, 4);
    add(0xDD, InstructionId::CMP, AddressingMode::ABX, 3, 4);
    add(0xD9, InstructionId::CMP, AddressingMode::ABY, 3, 4);
    add(0xC1, InstructionId::CMP, AddressingMode::IXI, 2, 6);
    add(0xD1, InstructionId::CMP, AddressingMode::IIX, 2, 5);
    // CPX - Compare X Register
    add(0xE0, InstructionId::CPX, AddressingMode::IMM, 2, 2);
    add(0xE4, InstructionId::CPX, AddressingMode::ZP0, 2, 3);
    add(0xEC, InstructionId::CPX, AddressingMode::ABS, 3, 4);
    // CPY - Compare Y Register
    add(0xC0, InstructionId::CPY, AddressingMode::IMM, 2, 2);
    add(0xC4, InstructionId::CPY, AddressingMode::ZP0, 2, 3);
    add(0xCC, InstructionId::CPY, AddressingMode::ABS, 3, 4);

    // Increments and decrements

    // INC - Increment Memory
    add(0xE6, InstructionId::INC, AddressingMode::ZP0, 2, 5);
    add(0xF6, InstructionId::INC, AddressingMode::ZPX, 2, 6);
    add(0xEE, InstructionId::INC, AddressingMode::ABS, 3, 6);
    add(0xFE, InstructionId::INC, AddressingMode::ABX, 3, 7);
    // INX - Increment X Register
    add(0xE8, InstructionId::INX, AddressingMode::IMP, 1, 2);
    // INY - Increment Y Register
    add(0xC8, InstructionId::INY, AddressingMode::IMP, 1, 2);
    // DEC - Decrement Memory
    add(0xC6, InstructionId::DEC, AddressingMode::ZP0, 2, 5);
    add(0xD6, InstructionId::DEC, AddressingMode::ZPX, 2, 6);
    add(0xCE, InstructionId::DEC, AddressingMode::ABS, 3, 6);
    add(0xDE, InstructionId::DEC, AddressingMode::ABX, 3, 7);
    // DEX - Decrement X Register
    add(0xCA, InstructionId::DEX, AddressingMode::IMP, 1, 2);
    // DEY - Decrement Y Register
    add(0x88, InstructionId::DEY, AddressingMode::IMP, 1, 2);

    // Shifts

    // ASL - Arithmetic Shift Left
    add(0x0A, InstructionId::ASL, AddressingMode::ACC, 1, 2);
    add(0x06, InstructionId::ASL, AddressingMode::ZP0, 2, 5);
    add(0x16, InstructionId::ASL, AddressingMode::ZPX, 2, 6);
    add(0x0E, InstructionId::ASL, AddressingMode::ABS, 3, 6);
    add(0x1E, InstructionId::ASL, AddressingMode::ABX, 3, 7);
    // LSR - Logical Shift Right
    add(0x4A, InstructionId::LSR, AddressingMode::ACC, 1, 2);
    add(0x46, InstructionId::LSR, AddressingMode::ZP0, 2, 5);
    add(0x56, InstructionId::LSR, AddressingMode::ZPX, 2, 6);
    add(0x4E, InstructionId::LSR, AddressingMode::ABS, 3, 6);
    add(0x5E, InstructionId::LSR, AddressingMode::ABX, 3, 7);
    // ROL - Rotate Left
    add(0x2A, InstructionId::ROL, AddressingMode::ACC, 1, 2);
    add(0x26, InstructionId::ROL, AddressingMode::ZP0, 2, 5);
    add(0x36, InstructionId::ROL, AddressingMode::ZPX, 2, 6);
    add(0x2E, InstructionId::ROL, AddressingMode::ABS, 3, 6);
    add(0x3E, InstructionId::ROL, AddressingMode::ABX, 3, 7);
    // ROR - Rotate Right
    add(0x6A, InstructionId::ROR, AddressingMode::ACC, 1, 2);
    add(0x66, InstructionId::ROR, AddressingMode::ZP0, 2, 5);
    add(0x76, InstructionId::ROR, AddressingMode::ZPX, 2, 6);
    add(0x6E, InstructionId::ROR, AddressingMode::ABS, 3, 6);
    add(0x7E, InstructionId::ROR, AddressingMode::ABX, 3, 7);

    // Jumps and calls

    // JMP - Jump
    add(0x4C, InstructionId::JMP, AddressingMode::ABS, 3, 3);
    add(0x6C, InstructionId::JMP, AddressingMode::IND, 3, 5);
    // JSR - Jump to Subroutine
    add(0x20, InstructionId::JSR, AddressingMode::ABS, 3, 6);
    // RTS - Return from Subroutine
    add(0x60, InstructionId::RTS, AddressingMode::IMP, 1, 6);

    // Branches

    // BCC - Branch if Carry Clear
    add(0x90, InstructionId::BCC, AddressingMode::REL, 2, 2);
    // BCS - Branch if Carry Set
    add(0xB0, InstructionId::BCS, AddressingMode::REL, 2, 2);
    // BEQ - Branch if Equal
    add(0xF0, InstructionId::BEQ, AddressingMode::REL, 2, 2);
    // BMI - Branch if Minus
    add(0x30, InstructionId::BMI, AddressingMode::REL, 2, 2);
    // BNE - Branch if Not Equal
    add(0xD0, InstructionId::BNE, AddressingMode::REL, 2, 2);
    // BPL - Branch if Positive
    add(0x10, InstructionId::BPL, AddressingMode::REL, 2, 2);
    // BVC - Branch if Overflow Clear
    add(0x50, InstructionId::BVC, AddressingMode::REL, 2, 2);
    // BVS - Branch if Overflow Set
    add(0x70, InstructionId::BVS, AddressingMode::REL, 2, 2);

    // Status flag changes

    // CLC - Clear Carry Flag
    add(0x18, InstructionId::CLC, AddressingMode::IMP, 1, 2);
    // CLD - Clear Decimal Mode
    add(0xD8, InstructionId::CLD, AddressingMode::IMP, 1, 2);
    // CLI - Clear Interrupt Disable
    add(0x58, InstructionId::CLI, AddressingMode::IMP, 1, 2);
    // CLV - Clear Overflow Flag
    add(0xB8, InstructionId::CLV, AddressingMode::IMP, 1, 2);
    // SEC - Set Carry Flag
    add(0x38, InstructionId::SEC, AddressingMode::IMP, 1, 2);
    // SED - Set Decimal Flag
    add(0xF8, InstructionId::SED, AddressingMode::IMP, 1, 2);
    // SEI - Set Interrupt Disable
    add(0x78, InstructionId::SEI, AddressingMode::IMP, 1, 2);

    // System functions

    // BRK - Force Interrupt
    add(0x00, InstructionId::BRK, AddressingMode::IMP, 1, 7);
    // NOP - No Operation
    add(0xEA, InstructionId::NOP, AddressingMode::IMP, 1, 2);
    // RTI - Return from Interrupt
    add(0x40, InstructionId::RTI, AddressingMode::IMP, 1, 6);
}

void OpcodeParser::add(const uint8_t raw, const InstructionId instruction_id, const AddressingMode addressing_mode,
                       const size_t instruction_size, const size_t base_cycles)
{
    opcodes.emplace(raw, Opcode{raw, instruction_id, addressing_mode, instruction_size, base_cycles});
}

Opcode OpcodeParser::parse(const uint8_t raw)
{
    auto element = opcodes.find(raw);
    if (element != opcodes.end())
    {
        return element->second;
    }
    else
    {
        throw std::out_of_range(std::string("Cannot parse instruction ") + common::print_hex(raw, sizeof(raw)));
    }
}
} // namespace cpu