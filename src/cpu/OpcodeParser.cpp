#include <stdexcept>

#include "OpcodeParser.h"

namespace cpu
{
    OpcodeParser::OpcodeParser()
    {
        // Load/store operations

        // LDA - Load Accumulator
        opcodes.emplace(0xA9, Opcode{InstructionId::LDA, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0xA5, Opcode{InstructionId::LDA, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0xB5, Opcode{InstructionId::LDA, AddressingMode::ZERO_PAGE_X, 2, 4});
        opcodes.emplace(0xAD, Opcode{InstructionId::LDA, AddressingMode::ABSOLUTE, 3, 4});
        opcodes.emplace(0xBD, Opcode{InstructionId::LDA, AddressingMode::ABSOLUTE_X, 3, 4});
        opcodes.emplace(0xB9, Opcode{InstructionId::LDA, AddressingMode::ABSOLUTE_Y, 3, 4});
        opcodes.emplace(0xA1, Opcode{InstructionId::LDA, AddressingMode::INDEXED_INDIRECT, 2, 6});
        opcodes.emplace(0xB1, Opcode{InstructionId::LDA, AddressingMode::INDIRECT_INDEXED, 2, 5});
        // LDX - Load X Register
        opcodes.emplace(0xA2, Opcode{InstructionId::LDX, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0xA6, Opcode{InstructionId::LDX, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0xB6, Opcode{InstructionId::LDX, AddressingMode::ZERO_PAGE_Y, 2, 4});
        opcodes.emplace(0xAE, Opcode{InstructionId::LDX, AddressingMode::ABSOLUTE, 3, 4});
        opcodes.emplace(0xBE, Opcode{InstructionId::LDX, AddressingMode::ABSOLUTE_Y, 3, 4});
        // LDY - Load Y Register
        opcodes.emplace(0xA0, Opcode{InstructionId::LDY, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0xA4, Opcode{InstructionId::LDY, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0xB4, Opcode{InstructionId::LDY, AddressingMode::ZERO_PAGE_X, 2, 4});
        opcodes.emplace(0xAC, Opcode{InstructionId::LDY, AddressingMode::ABSOLUTE, 3, 4});
        opcodes.emplace(0xBC, Opcode{InstructionId::LDY, AddressingMode::ABSOLUTE_X, 3, 4});
        // STA - Store Accumulator
        opcodes.emplace(0x85, Opcode{InstructionId::STA, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0x95, Opcode{InstructionId::STA, AddressingMode::ZERO_PAGE_X, 2, 4});
        opcodes.emplace(0x8D, Opcode{InstructionId::STA, AddressingMode::ABSOLUTE, 3, 4});
        opcodes.emplace(0x9D, Opcode{InstructionId::STA, AddressingMode::ABSOLUTE_X, 3, 5});
        opcodes.emplace(0x99, Opcode{InstructionId::STA, AddressingMode::ABSOLUTE_Y, 3, 5});
        opcodes.emplace(0x81, Opcode{InstructionId::STA, AddressingMode::INDEXED_INDIRECT, 2, 6});
        opcodes.emplace(0x91, Opcode{InstructionId::STA, AddressingMode::INDIRECT_INDEXED, 2, 6});
        // STX - Store X Register
        opcodes.emplace(0x86, Opcode{InstructionId::STX, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0x96, Opcode{InstructionId::STX, AddressingMode::ZERO_PAGE_Y, 2, 4});
        opcodes.emplace(0x8E, Opcode{InstructionId::STX, AddressingMode::ABSOLUTE, 3, 4});
        // STY - Store Y Register
        opcodes.emplace(0x84, Opcode{InstructionId::STY, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0x94, Opcode{InstructionId::STY, AddressingMode::ZERO_PAGE_X, 2, 4});
        opcodes.emplace(0x8C, Opcode{InstructionId::STY, AddressingMode::ABSOLUTE, 3, 4});

        // Register transfers

        // TAX - Transfer Accumulator to X
        opcodes.emplace(0xAA, Opcode{InstructionId::TAX, AddressingMode::IMPLICIT, 1, 2});
        // TAY - Transfer Accumulator to Y
        opcodes.emplace(0xA8, Opcode{InstructionId::TAY, AddressingMode::IMPLICIT, 1, 2});
        // TXA - Transfer X to Accumulator
        opcodes.emplace(0x8A, Opcode{InstructionId::TXA, AddressingMode::IMPLICIT, 1, 2});
        // TYA - Transfer Y to Accumulator
        opcodes.emplace(0x98, Opcode{InstructionId::TYA, AddressingMode::IMPLICIT, 1, 2});

        // Stack operations

        // TSX - Transfer Stack Pointer to X
        opcodes.emplace(0xBA, Opcode{InstructionId::TSX, AddressingMode::IMPLICIT, 1, 2});
        // TXS - Transfer X to Stack Pointer
        opcodes.emplace(0x9A, Opcode{InstructionId::TXS, AddressingMode::IMPLICIT, 1, 2});
        // PHA - Push Accumulator
        opcodes.emplace(0x48, Opcode{InstructionId::PHA, AddressingMode::IMPLICIT, 1, 3});
        // PHP - Push Processor Status
        opcodes.emplace(0x08, Opcode{InstructionId::PHP, AddressingMode::IMPLICIT, 1, 3});
        // PLA - Pull Accumulator
        opcodes.emplace(0x68, Opcode{InstructionId::PLA, AddressingMode::IMPLICIT, 1, 4});
        // PLP - Pull Processor Status
        opcodes.emplace(0x28, Opcode{InstructionId::PLP, AddressingMode::IMPLICIT, 1, 4});

        // Logical

        // AND - Logical AND
        opcodes.emplace(0x29, Opcode{InstructionId::AND, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0x25, Opcode{InstructionId::AND, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0x35, Opcode{InstructionId::AND, AddressingMode::ZERO_PAGE_X, 2, 4});
        opcodes.emplace(0x2D, Opcode{InstructionId::AND, AddressingMode::ABSOLUTE, 3, 4});
        opcodes.emplace(0x3D, Opcode{InstructionId::AND, AddressingMode::ABSOLUTE_X, 3, 4});
        opcodes.emplace(0x39, Opcode{InstructionId::AND, AddressingMode::ABSOLUTE_Y, 3, 4});
        opcodes.emplace(0x21, Opcode{InstructionId::AND, AddressingMode::INDEXED_INDIRECT, 2, 6});
        opcodes.emplace(0x31, Opcode{InstructionId::AND, AddressingMode::INDIRECT_INDEXED, 2, 5});
        // EOR - Exclusive OR
        opcodes.emplace(0x49, Opcode{InstructionId::EOR, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0x45, Opcode{InstructionId::EOR, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0x55, Opcode{InstructionId::EOR, AddressingMode::ZERO_PAGE_X, 2, 4});
        opcodes.emplace(0x4D, Opcode{InstructionId::EOR, AddressingMode::ABSOLUTE, 3, 4});
        opcodes.emplace(0x5D, Opcode{InstructionId::EOR, AddressingMode::ABSOLUTE_X, 3, 4});
        opcodes.emplace(0x59, Opcode{InstructionId::EOR, AddressingMode::ABSOLUTE_Y, 3, 4});
        opcodes.emplace(0x41, Opcode{InstructionId::EOR, AddressingMode::INDEXED_INDIRECT, 2, 6});
        opcodes.emplace(0x51, Opcode{InstructionId::EOR, AddressingMode::INDIRECT_INDEXED, 2, 5});
        // ORA - Logical Inclusive OR
        opcodes.emplace(0x09, Opcode{InstructionId::ORA, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0x05, Opcode{InstructionId::ORA, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0x15, Opcode{InstructionId::ORA, AddressingMode::ZERO_PAGE_X, 2, 4});
        opcodes.emplace(0x0D, Opcode{InstructionId::ORA, AddressingMode::ABSOLUTE, 3, 4});
        opcodes.emplace(0x1D, Opcode{InstructionId::ORA, AddressingMode::ABSOLUTE_X, 3, 4});
        opcodes.emplace(0x19, Opcode{InstructionId::ORA, AddressingMode::ABSOLUTE_Y, 3, 4});
        opcodes.emplace(0x01, Opcode{InstructionId::ORA, AddressingMode::INDEXED_INDIRECT, 2, 6});
        opcodes.emplace(0x11, Opcode{InstructionId::ORA, AddressingMode::INDIRECT_INDEXED, 2, 5});
        // BIT - Bit Test
        opcodes.emplace(0x24, Opcode{InstructionId::BIT, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0x2C, Opcode{InstructionId::BIT, AddressingMode::ABSOLUTE, 3, 4});

        // Arithmetic

        // ADC - Add with Carry
        opcodes.emplace(0x69, Opcode{InstructionId::ADC, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0x65, Opcode{InstructionId::ADC, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0x75, Opcode{InstructionId::ADC, AddressingMode::ZERO_PAGE_X, 2, 4});
        opcodes.emplace(0x6D, Opcode{InstructionId::ADC, AddressingMode::ABSOLUTE, 3, 4});
        opcodes.emplace(0x7D, Opcode{InstructionId::ADC, AddressingMode::ABSOLUTE_X, 3, 4});
        opcodes.emplace(0x79, Opcode{InstructionId::ADC, AddressingMode::ABSOLUTE_Y, 3, 4});
        opcodes.emplace(0x61, Opcode{InstructionId::ADC, AddressingMode::INDEXED_INDIRECT, 2, 6});
        opcodes.emplace(0x71, Opcode{InstructionId::ADC, AddressingMode::INDIRECT_INDEXED, 2, 5});
        // SBC - Subtract with Carry
        opcodes.emplace(0xE9, Opcode{InstructionId::SBC, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0xE5, Opcode{InstructionId::SBC, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0xF5, Opcode{InstructionId::SBC, AddressingMode::ZERO_PAGE_X, 2, 4});
        opcodes.emplace(0xED, Opcode{InstructionId::SBC, AddressingMode::ABSOLUTE, 3, 4});
        opcodes.emplace(0xFD, Opcode{InstructionId::SBC, AddressingMode::ABSOLUTE_X, 3, 4});
        opcodes.emplace(0xF9, Opcode{InstructionId::SBC, AddressingMode::ABSOLUTE_Y, 3, 4});
        opcodes.emplace(0xE1, Opcode{InstructionId::SBC, AddressingMode::INDEXED_INDIRECT, 2, 6});
        opcodes.emplace(0xF1, Opcode{InstructionId::SBC, AddressingMode::INDIRECT_INDEXED, 2, 5});
        // CMP - Compare
        opcodes.emplace(0xC9, Opcode{InstructionId::CMP, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0xC5, Opcode{InstructionId::CMP, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0xD5, Opcode{InstructionId::CMP, AddressingMode::ZERO_PAGE_X, 2, 4});
        opcodes.emplace(0xCD, Opcode{InstructionId::CMP, AddressingMode::ABSOLUTE, 3, 4});
        opcodes.emplace(0xDD, Opcode{InstructionId::CMP, AddressingMode::ABSOLUTE_X, 3, 4});
        opcodes.emplace(0xD9, Opcode{InstructionId::CMP, AddressingMode::ABSOLUTE_Y, 3, 4});
        opcodes.emplace(0xC1, Opcode{InstructionId::CMP, AddressingMode::INDEXED_INDIRECT, 2, 6});
        opcodes.emplace(0xD1, Opcode{InstructionId::CMP, AddressingMode::INDIRECT_INDEXED, 2, 5});
        // CPX - Compare X Register
        opcodes.emplace(0xE0, Opcode{InstructionId::CPX, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0xE4, Opcode{InstructionId::CPX, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0xEC, Opcode{InstructionId::CPX, AddressingMode::ABSOLUTE, 3, 4});
        // CPY - Compare Y Register
        opcodes.emplace(0xC0, Opcode{InstructionId::CPY, AddressingMode::IMMEDIATE, 2, 2});
        opcodes.emplace(0xC4, Opcode{InstructionId::CPY, AddressingMode::ZERO_PAGE, 2, 3});
        opcodes.emplace(0xCC, Opcode{InstructionId::CPY, AddressingMode::ABSOLUTE, 3, 4});

        // Increments and decrements

        // INC - Increment Memory
        opcodes.emplace(0xE6, Opcode{InstructionId::INC, AddressingMode::ZERO_PAGE, 2, 5});
        opcodes.emplace(0xF6, Opcode{InstructionId::INC, AddressingMode::ZERO_PAGE_X, 2, 6});
        opcodes.emplace(0xEE, Opcode{InstructionId::INC, AddressingMode::ABSOLUTE, 3, 6});
        opcodes.emplace(0xFE, Opcode{InstructionId::INC, AddressingMode::ABSOLUTE_X, 3, 7});
        // INX - Increment X Register
        opcodes.emplace(0xE8, Opcode{InstructionId::INX, AddressingMode::IMPLICIT, 1, 2});
        // INY - Increment Y Register
        opcodes.emplace(0xC8, Opcode{InstructionId::INY, AddressingMode::IMPLICIT, 1, 2});
        // DEC - Decrement Memory
        opcodes.emplace(0xC6, Opcode{InstructionId::DEC, AddressingMode::ZERO_PAGE, 2, 5});
        opcodes.emplace(0xD6, Opcode{InstructionId::DEC, AddressingMode::ZERO_PAGE_X, 2, 6});
        opcodes.emplace(0xCE, Opcode{InstructionId::DEC, AddressingMode::ABSOLUTE, 3, 6});
        opcodes.emplace(0xDE, Opcode{InstructionId::DEC, AddressingMode::ABSOLUTE_X, 3, 7});
        // DEX - Decrement X Register
        opcodes.emplace(0xCA, Opcode{InstructionId::DEX, AddressingMode::IMPLICIT, 1, 2});
        // DEY - Decrement Y Register
        opcodes.emplace(0x88, Opcode{InstructionId::DEY, AddressingMode::IMPLICIT, 1, 2});

        // Shifts

        // ASL - Arithmetic Shift Left
        opcodes.emplace(0x0A, Opcode{InstructionId::ASL, AddressingMode::ACCUMULATOR, 1, 2});
        opcodes.emplace(0x06, Opcode{InstructionId::ASL, AddressingMode::ZERO_PAGE, 2, 5});
        opcodes.emplace(0x16, Opcode{InstructionId::ASL, AddressingMode::ZERO_PAGE_X, 2, 6});
        opcodes.emplace(0x0E, Opcode{InstructionId::ASL, AddressingMode::ABSOLUTE, 3, 6});
        opcodes.emplace(0x1E, Opcode{InstructionId::ASL, AddressingMode::ABSOLUTE_X, 3, 7});
        // LSR - Logical Shift Right
        opcodes.emplace(0x4A, Opcode{InstructionId::LSR, AddressingMode::ACCUMULATOR, 1, 2});
        opcodes.emplace(0x46, Opcode{InstructionId::LSR, AddressingMode::ZERO_PAGE, 2, 5});
        opcodes.emplace(0x56, Opcode{InstructionId::LSR, AddressingMode::ZERO_PAGE_X, 2, 6});
        opcodes.emplace(0x4E, Opcode{InstructionId::LSR, AddressingMode::ABSOLUTE, 3, 6});
        opcodes.emplace(0x5E, Opcode{InstructionId::LSR, AddressingMode::ABSOLUTE_X, 3, 7});
        // ROL - Rotate Left
        opcodes.emplace(0x2A, Opcode{InstructionId::ROL, AddressingMode::ACCUMULATOR, 1, 2});
        opcodes.emplace(0x26, Opcode{InstructionId::ROL, AddressingMode::ZERO_PAGE, 2, 5});
        opcodes.emplace(0x36, Opcode{InstructionId::ROL, AddressingMode::ZERO_PAGE_X, 2, 6});
        opcodes.emplace(0x2E, Opcode{InstructionId::ROL, AddressingMode::ABSOLUTE, 3, 6});
        opcodes.emplace(0x3E, Opcode{InstructionId::ROL, AddressingMode::ABSOLUTE_X, 3, 7});
        // ROR - Rotate Right
        opcodes.emplace(0x6A, Opcode{InstructionId::ROR, AddressingMode::ACCUMULATOR, 1, 2});
        opcodes.emplace(0x66, Opcode{InstructionId::ROR, AddressingMode::ZERO_PAGE, 2, 5});
        opcodes.emplace(0x76, Opcode{InstructionId::ROR, AddressingMode::ZERO_PAGE_X, 2, 6});
        opcodes.emplace(0x6E, Opcode{InstructionId::ROR, AddressingMode::ABSOLUTE, 3, 6});
        opcodes.emplace(0x7E, Opcode{InstructionId::ROR, AddressingMode::ABSOLUTE_X, 3, 7});

        // Jumps and calls

        // JMP - Jump
        opcodes.emplace(0x4C, Opcode{InstructionId::JMP, AddressingMode::ABSOLUTE, 3, 3});
        opcodes.emplace(0x6C, Opcode{InstructionId::JMP, AddressingMode::INDIRECT, 3, 5});
        // JSR - Jump to Subroutine
        opcodes.emplace(0x20, Opcode{InstructionId::JSR, AddressingMode::ABSOLUTE, 3, 6});
        // RTS - Return from Subroutine
        opcodes.emplace(0x60, Opcode{InstructionId::RTS, AddressingMode::IMPLICIT, 1, 6});

        // Branches

        // BCC - Branch if Carry Clear
        opcodes.emplace(0x90, Opcode{InstructionId::BCC, AddressingMode::RELATIVE, 2, 2});
        // BCS - Branch if Carry Set
        opcodes.emplace(0xB0, Opcode{InstructionId::BCS, AddressingMode::RELATIVE, 2, 2});
        // BEQ - Branch if Equal
        opcodes.emplace(0xF0, Opcode{InstructionId::BEQ, AddressingMode::RELATIVE, 2, 2});
        // BMI - Branch if Minus
        opcodes.emplace(0x30, Opcode{InstructionId::BMI, AddressingMode::RELATIVE, 2, 2});
        // BNE - Branch if Not Equal
        opcodes.emplace(0xD0, Opcode{InstructionId::BNE, AddressingMode::RELATIVE, 2, 2});
        // BPL - Branch if Positive
        opcodes.emplace(0x10, Opcode{InstructionId::BPL, AddressingMode::RELATIVE, 2, 2});
        // BVC - Branch if Overflow Clear
        opcodes.emplace(0x50, Opcode{InstructionId::BVC, AddressingMode::RELATIVE, 2, 2});
        // BVS - Branch if Overflow Set
        opcodes.emplace(0x70, Opcode{InstructionId::BVS, AddressingMode::RELATIVE, 2, 2});

        // Status flag changes

        // CLC - Clear Carry Flag
        opcodes.emplace(0x18, Opcode{InstructionId::CLC, AddressingMode::IMPLICIT, 1, 2});
        // CLD - Clear Decimal Mode
        opcodes.emplace(0xD8, Opcode{InstructionId::CLD, AddressingMode::IMPLICIT, 1, 2});
        // CLI - Clear Interrupt Disable
        opcodes.emplace(0x58, Opcode{InstructionId::CLI, AddressingMode::IMPLICIT, 1, 2});
        // CLV - Clear Overflow Flag
        opcodes.emplace(0xB8, Opcode{InstructionId::CLV, AddressingMode::IMPLICIT, 1, 2});
        // SEC - Set Carry Flag
        opcodes.emplace(0x38, Opcode{InstructionId::SEC, AddressingMode::IMPLICIT, 1, 2});
        // SED - Set Decimal Flag
        opcodes.emplace(0xF8, Opcode{InstructionId::SED, AddressingMode::IMPLICIT, 1, 2});
        // SEI - Set Interrupt Disable
        opcodes.emplace(0x78, Opcode{InstructionId::SEI, AddressingMode::IMPLICIT, 1, 2});

        // System functions

        // BRK - Force Interrupt
        opcodes.emplace(0x00, Opcode{InstructionId::BRK, AddressingMode::IMPLICIT, 1, 7});
        // NOP - No Operation
        opcodes.emplace(0xEA, Opcode{InstructionId::NOP, AddressingMode::IMPLICIT, 1, 2});
        // RTI - Return from Interrupt
        opcodes.emplace(0x40, Opcode{InstructionId::RTI, AddressingMode::IMPLICIT, 1, 6});
    }

    Opcode OpcodeParser::parse(const uint8_t byte)
    {
        auto element = opcodes.find(byte);
        if (element != opcodes.end())
        {
            return element->second;
        }
        else
        {
            throw std::invalid_argument(std::string("Cannot parse instruction %u", byte));
        }
    }
}