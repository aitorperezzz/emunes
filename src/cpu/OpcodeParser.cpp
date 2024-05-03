#include <stdexcept>

#include "OpcodeParser.h"

namespace cpu
{
    OpcodeParser::OpcodeParser()
    {
        // Load/store operations

        // LDA - Load ACC
        opcodes.emplace(0xA9, Opcode{InstructionId::LDA, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0xA5, Opcode{InstructionId::LDA, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0xB5, Opcode{InstructionId::LDA, AddressingMode::ZPX, 2, 4});
        opcodes.emplace(0xAD, Opcode{InstructionId::LDA, AddressingMode::ABS, 3, 4});
        opcodes.emplace(0xBD, Opcode{InstructionId::LDA, AddressingMode::ABX, 3, 4});
        opcodes.emplace(0xB9, Opcode{InstructionId::LDA, AddressingMode::ABY, 3, 4});
        opcodes.emplace(0xA1, Opcode{InstructionId::LDA, AddressingMode::IXI, 2, 6});
        opcodes.emplace(0xB1, Opcode{InstructionId::LDA, AddressingMode::IIX, 2, 5});
        // LDX - Load X Register
        opcodes.emplace(0xA2, Opcode{InstructionId::LDX, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0xA6, Opcode{InstructionId::LDX, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0xB6, Opcode{InstructionId::LDX, AddressingMode::ZPY, 2, 4});
        opcodes.emplace(0xAE, Opcode{InstructionId::LDX, AddressingMode::ABS, 3, 4});
        opcodes.emplace(0xBE, Opcode{InstructionId::LDX, AddressingMode::ABY, 3, 4});
        // LDY - Load Y Register
        opcodes.emplace(0xA0, Opcode{InstructionId::LDY, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0xA4, Opcode{InstructionId::LDY, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0xB4, Opcode{InstructionId::LDY, AddressingMode::ZPX, 2, 4});
        opcodes.emplace(0xAC, Opcode{InstructionId::LDY, AddressingMode::ABS, 3, 4});
        opcodes.emplace(0xBC, Opcode{InstructionId::LDY, AddressingMode::ABX, 3, 4});
        // STA - Store ACC
        opcodes.emplace(0x85, Opcode{InstructionId::STA, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0x95, Opcode{InstructionId::STA, AddressingMode::ZPX, 2, 4});
        opcodes.emplace(0x8D, Opcode{InstructionId::STA, AddressingMode::ABS, 3, 4});
        opcodes.emplace(0x9D, Opcode{InstructionId::STA, AddressingMode::ABX, 3, 5});
        opcodes.emplace(0x99, Opcode{InstructionId::STA, AddressingMode::ABY, 3, 5});
        opcodes.emplace(0x81, Opcode{InstructionId::STA, AddressingMode::IXI, 2, 6});
        opcodes.emplace(0x91, Opcode{InstructionId::STA, AddressingMode::IIX, 2, 6});
        // STX - Store X Register
        opcodes.emplace(0x86, Opcode{InstructionId::STX, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0x96, Opcode{InstructionId::STX, AddressingMode::ZPY, 2, 4});
        opcodes.emplace(0x8E, Opcode{InstructionId::STX, AddressingMode::ABS, 3, 4});
        // STY - Store Y Register
        opcodes.emplace(0x84, Opcode{InstructionId::STY, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0x94, Opcode{InstructionId::STY, AddressingMode::ZPX, 2, 4});
        opcodes.emplace(0x8C, Opcode{InstructionId::STY, AddressingMode::ABS, 3, 4});

        // Register transfers

        // TAX - Transfer ACC to X
        opcodes.emplace(0xAA, Opcode{InstructionId::TAX, AddressingMode::IMP, 1, 2});
        // TAY - Transfer ACC to Y
        opcodes.emplace(0xA8, Opcode{InstructionId::TAY, AddressingMode::IMP, 1, 2});
        // TXA - Transfer X to ACC
        opcodes.emplace(0x8A, Opcode{InstructionId::TXA, AddressingMode::IMP, 1, 2});
        // TYA - Transfer Y to ACC
        opcodes.emplace(0x98, Opcode{InstructionId::TYA, AddressingMode::IMP, 1, 2});

        // Stack operations

        // TSX - Transfer Stack Pointer to X
        opcodes.emplace(0xBA, Opcode{InstructionId::TSX, AddressingMode::IMP, 1, 2});
        // TXS - Transfer X to Stack Pointer
        opcodes.emplace(0x9A, Opcode{InstructionId::TXS, AddressingMode::IMP, 1, 2});
        // PHA - Push ACC
        opcodes.emplace(0x48, Opcode{InstructionId::PHA, AddressingMode::IMP, 1, 3});
        // PHP - Push Processor Status
        opcodes.emplace(0x08, Opcode{InstructionId::PHP, AddressingMode::IMP, 1, 3});
        // PLA - Pull ACC
        opcodes.emplace(0x68, Opcode{InstructionId::PLA, AddressingMode::IMP, 1, 4});
        // PLP - Pull Processor Status
        opcodes.emplace(0x28, Opcode{InstructionId::PLP, AddressingMode::IMP, 1, 4});

        // Logical

        // AND - Logical AND
        opcodes.emplace(0x29, Opcode{InstructionId::AND, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0x25, Opcode{InstructionId::AND, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0x35, Opcode{InstructionId::AND, AddressingMode::ZPX, 2, 4});
        opcodes.emplace(0x2D, Opcode{InstructionId::AND, AddressingMode::ABS, 3, 4});
        opcodes.emplace(0x3D, Opcode{InstructionId::AND, AddressingMode::ABX, 3, 4});
        opcodes.emplace(0x39, Opcode{InstructionId::AND, AddressingMode::ABY, 3, 4});
        opcodes.emplace(0x21, Opcode{InstructionId::AND, AddressingMode::IXI, 2, 6});
        opcodes.emplace(0x31, Opcode{InstructionId::AND, AddressingMode::IIX, 2, 5});
        // EOR - Exclusive OR
        opcodes.emplace(0x49, Opcode{InstructionId::EOR, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0x45, Opcode{InstructionId::EOR, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0x55, Opcode{InstructionId::EOR, AddressingMode::ZPX, 2, 4});
        opcodes.emplace(0x4D, Opcode{InstructionId::EOR, AddressingMode::ABS, 3, 4});
        opcodes.emplace(0x5D, Opcode{InstructionId::EOR, AddressingMode::ABX, 3, 4});
        opcodes.emplace(0x59, Opcode{InstructionId::EOR, AddressingMode::ABY, 3, 4});
        opcodes.emplace(0x41, Opcode{InstructionId::EOR, AddressingMode::IXI, 2, 6});
        opcodes.emplace(0x51, Opcode{InstructionId::EOR, AddressingMode::IIX, 2, 5});
        // ORA - Logical Inclusive OR
        opcodes.emplace(0x09, Opcode{InstructionId::ORA, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0x05, Opcode{InstructionId::ORA, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0x15, Opcode{InstructionId::ORA, AddressingMode::ZPX, 2, 4});
        opcodes.emplace(0x0D, Opcode{InstructionId::ORA, AddressingMode::ABS, 3, 4});
        opcodes.emplace(0x1D, Opcode{InstructionId::ORA, AddressingMode::ABX, 3, 4});
        opcodes.emplace(0x19, Opcode{InstructionId::ORA, AddressingMode::ABY, 3, 4});
        opcodes.emplace(0x01, Opcode{InstructionId::ORA, AddressingMode::IXI, 2, 6});
        opcodes.emplace(0x11, Opcode{InstructionId::ORA, AddressingMode::IIX, 2, 5});
        // BIT - Bit Test
        opcodes.emplace(0x24, Opcode{InstructionId::BIT, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0x2C, Opcode{InstructionId::BIT, AddressingMode::ABS, 3, 4});

        // Arithmetic

        // ADC - Add with Carry
        opcodes.emplace(0x69, Opcode{InstructionId::ADC, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0x65, Opcode{InstructionId::ADC, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0x75, Opcode{InstructionId::ADC, AddressingMode::ZPX, 2, 4});
        opcodes.emplace(0x6D, Opcode{InstructionId::ADC, AddressingMode::ABS, 3, 4});
        opcodes.emplace(0x7D, Opcode{InstructionId::ADC, AddressingMode::ABX, 3, 4});
        opcodes.emplace(0x79, Opcode{InstructionId::ADC, AddressingMode::ABY, 3, 4});
        opcodes.emplace(0x61, Opcode{InstructionId::ADC, AddressingMode::IXI, 2, 6});
        opcodes.emplace(0x71, Opcode{InstructionId::ADC, AddressingMode::IIX, 2, 5});
        // SBC - Subtract with Carry
        opcodes.emplace(0xE9, Opcode{InstructionId::SBC, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0xE5, Opcode{InstructionId::SBC, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0xF5, Opcode{InstructionId::SBC, AddressingMode::ZPX, 2, 4});
        opcodes.emplace(0xED, Opcode{InstructionId::SBC, AddressingMode::ABS, 3, 4});
        opcodes.emplace(0xFD, Opcode{InstructionId::SBC, AddressingMode::ABX, 3, 4});
        opcodes.emplace(0xF9, Opcode{InstructionId::SBC, AddressingMode::ABY, 3, 4});
        opcodes.emplace(0xE1, Opcode{InstructionId::SBC, AddressingMode::IXI, 2, 6});
        opcodes.emplace(0xF1, Opcode{InstructionId::SBC, AddressingMode::IIX, 2, 5});
        // CMP - Compare
        opcodes.emplace(0xC9, Opcode{InstructionId::CMP, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0xC5, Opcode{InstructionId::CMP, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0xD5, Opcode{InstructionId::CMP, AddressingMode::ZPX, 2, 4});
        opcodes.emplace(0xCD, Opcode{InstructionId::CMP, AddressingMode::ABS, 3, 4});
        opcodes.emplace(0xDD, Opcode{InstructionId::CMP, AddressingMode::ABX, 3, 4});
        opcodes.emplace(0xD9, Opcode{InstructionId::CMP, AddressingMode::ABY, 3, 4});
        opcodes.emplace(0xC1, Opcode{InstructionId::CMP, AddressingMode::IXI, 2, 6});
        opcodes.emplace(0xD1, Opcode{InstructionId::CMP, AddressingMode::IIX, 2, 5});
        // CPX - Compare X Register
        opcodes.emplace(0xE0, Opcode{InstructionId::CPX, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0xE4, Opcode{InstructionId::CPX, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0xEC, Opcode{InstructionId::CPX, AddressingMode::ABS, 3, 4});
        // CPY - Compare Y Register
        opcodes.emplace(0xC0, Opcode{InstructionId::CPY, AddressingMode::IMM, 2, 2});
        opcodes.emplace(0xC4, Opcode{InstructionId::CPY, AddressingMode::ZP0, 2, 3});
        opcodes.emplace(0xCC, Opcode{InstructionId::CPY, AddressingMode::ABS, 3, 4});

        // Increments and decrements

        // INC - Increment Memory
        opcodes.emplace(0xE6, Opcode{InstructionId::INC, AddressingMode::ZP0, 2, 5});
        opcodes.emplace(0xF6, Opcode{InstructionId::INC, AddressingMode::ZPX, 2, 6});
        opcodes.emplace(0xEE, Opcode{InstructionId::INC, AddressingMode::ABS, 3, 6});
        opcodes.emplace(0xFE, Opcode{InstructionId::INC, AddressingMode::ABX, 3, 7});
        // INX - Increment X Register
        opcodes.emplace(0xE8, Opcode{InstructionId::INX, AddressingMode::IMP, 1, 2});
        // INY - Increment Y Register
        opcodes.emplace(0xC8, Opcode{InstructionId::INY, AddressingMode::IMP, 1, 2});
        // DEC - Decrement Memory
        opcodes.emplace(0xC6, Opcode{InstructionId::DEC, AddressingMode::ZP0, 2, 5});
        opcodes.emplace(0xD6, Opcode{InstructionId::DEC, AddressingMode::ZPX, 2, 6});
        opcodes.emplace(0xCE, Opcode{InstructionId::DEC, AddressingMode::ABS, 3, 6});
        opcodes.emplace(0xDE, Opcode{InstructionId::DEC, AddressingMode::ABX, 3, 7});
        // DEX - Decrement X Register
        opcodes.emplace(0xCA, Opcode{InstructionId::DEX, AddressingMode::IMP, 1, 2});
        // DEY - Decrement Y Register
        opcodes.emplace(0x88, Opcode{InstructionId::DEY, AddressingMode::IMP, 1, 2});

        // Shifts

        // ASL - Arithmetic Shift Left
        opcodes.emplace(0x0A, Opcode{InstructionId::ASL, AddressingMode::ACC, 1, 2});
        opcodes.emplace(0x06, Opcode{InstructionId::ASL, AddressingMode::ZP0, 2, 5});
        opcodes.emplace(0x16, Opcode{InstructionId::ASL, AddressingMode::ZPX, 2, 6});
        opcodes.emplace(0x0E, Opcode{InstructionId::ASL, AddressingMode::ABS, 3, 6});
        opcodes.emplace(0x1E, Opcode{InstructionId::ASL, AddressingMode::ABX, 3, 7});
        // LSR - Logical Shift Right
        opcodes.emplace(0x4A, Opcode{InstructionId::LSR, AddressingMode::ACC, 1, 2});
        opcodes.emplace(0x46, Opcode{InstructionId::LSR, AddressingMode::ZP0, 2, 5});
        opcodes.emplace(0x56, Opcode{InstructionId::LSR, AddressingMode::ZPX, 2, 6});
        opcodes.emplace(0x4E, Opcode{InstructionId::LSR, AddressingMode::ABS, 3, 6});
        opcodes.emplace(0x5E, Opcode{InstructionId::LSR, AddressingMode::ABX, 3, 7});
        // ROL - Rotate Left
        opcodes.emplace(0x2A, Opcode{InstructionId::ROL, AddressingMode::ACC, 1, 2});
        opcodes.emplace(0x26, Opcode{InstructionId::ROL, AddressingMode::ZP0, 2, 5});
        opcodes.emplace(0x36, Opcode{InstructionId::ROL, AddressingMode::ZPX, 2, 6});
        opcodes.emplace(0x2E, Opcode{InstructionId::ROL, AddressingMode::ABS, 3, 6});
        opcodes.emplace(0x3E, Opcode{InstructionId::ROL, AddressingMode::ABX, 3, 7});
        // ROR - Rotate Right
        opcodes.emplace(0x6A, Opcode{InstructionId::ROR, AddressingMode::ACC, 1, 2});
        opcodes.emplace(0x66, Opcode{InstructionId::ROR, AddressingMode::ZP0, 2, 5});
        opcodes.emplace(0x76, Opcode{InstructionId::ROR, AddressingMode::ZPX, 2, 6});
        opcodes.emplace(0x6E, Opcode{InstructionId::ROR, AddressingMode::ABS, 3, 6});
        opcodes.emplace(0x7E, Opcode{InstructionId::ROR, AddressingMode::ABX, 3, 7});

        // Jumps and calls

        // JMP - Jump
        opcodes.emplace(0x4C, Opcode{InstructionId::JMP, AddressingMode::ABS, 3, 3});
        opcodes.emplace(0x6C, Opcode{InstructionId::JMP, AddressingMode::IND, 3, 5});
        // JSR - Jump to Subroutine
        opcodes.emplace(0x20, Opcode{InstructionId::JSR, AddressingMode::ABS, 3, 6});
        // RTS - Return from Subroutine
        opcodes.emplace(0x60, Opcode{InstructionId::RTS, AddressingMode::IMP, 1, 6});

        // Branches

        // BCC - Branch if Carry Clear
        opcodes.emplace(0x90, Opcode{InstructionId::BCC, AddressingMode::REL, 2, 2});
        // BCS - Branch if Carry Set
        opcodes.emplace(0xB0, Opcode{InstructionId::BCS, AddressingMode::REL, 2, 2});
        // BEQ - Branch if Equal
        opcodes.emplace(0xF0, Opcode{InstructionId::BEQ, AddressingMode::REL, 2, 2});
        // BMI - Branch if Minus
        opcodes.emplace(0x30, Opcode{InstructionId::BMI, AddressingMode::REL, 2, 2});
        // BNE - Branch if Not Equal
        opcodes.emplace(0xD0, Opcode{InstructionId::BNE, AddressingMode::REL, 2, 2});
        // BPL - Branch if Positive
        opcodes.emplace(0x10, Opcode{InstructionId::BPL, AddressingMode::REL, 2, 2});
        // BVC - Branch if Overflow Clear
        opcodes.emplace(0x50, Opcode{InstructionId::BVC, AddressingMode::REL, 2, 2});
        // BVS - Branch if Overflow Set
        opcodes.emplace(0x70, Opcode{InstructionId::BVS, AddressingMode::REL, 2, 2});

        // Status flag changes

        // CLC - Clear Carry Flag
        opcodes.emplace(0x18, Opcode{InstructionId::CLC, AddressingMode::IMP, 1, 2});
        // CLD - Clear Decimal Mode
        opcodes.emplace(0xD8, Opcode{InstructionId::CLD, AddressingMode::IMP, 1, 2});
        // CLI - Clear Interrupt Disable
        opcodes.emplace(0x58, Opcode{InstructionId::CLI, AddressingMode::IMP, 1, 2});
        // CLV - Clear Overflow Flag
        opcodes.emplace(0xB8, Opcode{InstructionId::CLV, AddressingMode::IMP, 1, 2});
        // SEC - Set Carry Flag
        opcodes.emplace(0x38, Opcode{InstructionId::SEC, AddressingMode::IMP, 1, 2});
        // SED - Set Decimal Flag
        opcodes.emplace(0xF8, Opcode{InstructionId::SED, AddressingMode::IMP, 1, 2});
        // SEI - Set Interrupt Disable
        opcodes.emplace(0x78, Opcode{InstructionId::SEI, AddressingMode::IMP, 1, 2});

        // System functions

        // BRK - Force Interrupt
        opcodes.emplace(0x00, Opcode{InstructionId::BRK, AddressingMode::IMP, 1, 7});
        // NOP - No Operation
        opcodes.emplace(0xEA, Opcode{InstructionId::NOP, AddressingMode::IMP, 1, 2});
        // RTI - Return from Interrupt
        opcodes.emplace(0x40, Opcode{InstructionId::RTI, AddressingMode::IMP, 1, 6});
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