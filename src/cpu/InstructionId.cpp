#include <map>
#include <stdexcept>

#include "InstructionId.h"
#include "common/Logging.h"

namespace cpu
{

/// Static variable containing a relationship between instruction ids and pairs of
/// (instruction mnemonic + instruction description)
static std::map<InstructionId, std::pair<std::string, std::string>> instruction_id_map{
    // Load/store operations
    {InstructionId::LDA, {"LDA", "Load accumulator"}},
    {InstructionId::LDX, {"LDX", "Load X register"}},
    {InstructionId::LDY, {"LDY", "Load Y register"}},
    {InstructionId::STA, {"STA", "Store accumulator"}},
    {InstructionId::STX, {"STX", "Store X register"}},
    {InstructionId::STY, {"STY", "Store Y register"}},

    // Register transfers
    {InstructionId::TAX, {"TAX", "Transfer accumulator to X"}},
    {InstructionId::TAY, {"TAY", "Transfer accumulator to Y"}},
    {InstructionId::TXA, {"TXA", "Transfer X to accumulator"}},
    {InstructionId::TYA, {"TYA", "Transfer Y to accumulator"}},

    // Stack operations
    {InstructionId::TSX, {"TSX", "Transfer stack pointer to X"}},
    {InstructionId::TXS, {"TXS", "Transfer X to stack pointer"}},
    {InstructionId::PHA, {"PHA", "Push accumulator on stack"}},
    {InstructionId::PHP, {"PHP", "Push processor status on stack"}},
    {InstructionId::PLA, {"PLA", "Pull accumulator from stack"}},
    {InstructionId::PLP, {"PLP", "Pull processor status from stack"}},

    // Logical
    {InstructionId::AND, {"AND", "Logical AND"}},
    {InstructionId::EOR, {"EOR", "Exclusive OR"}},
    {InstructionId::ORA, {"ORA", "Logical inclusive OR"}},
    {InstructionId::BIT, {"BIT", "Bit test"}},

    // Arithmetic
    {InstructionId::ADC, {"ADC", "Add with carry"}},
    {InstructionId::SBC, {"SBC", "Subtract with carry"}},
    {InstructionId::CMP, {"CMP", "Compare accumulator"}},
    {InstructionId::CPX, {"CPX", "Compare X register"}},
    {InstructionId::CPY, {"CPY", "Compare Y register"}},

    // Increments and decrements
    {InstructionId::INC, {"INC", "Increment a memory location"}},
    {InstructionId::INX, {"INX", "Increment the X register"}},
    {InstructionId::INY, {"INY", "Increment the Y register"}},
    {InstructionId::DEC, {"DEC", "Decrement a memory location"}},
    {InstructionId::DEX, {"DEX", "Decrement the X register"}},
    {InstructionId::DEY, {"DEY", "Decrement the Y register"}},

    // Shifts
    {InstructionId::ASL, {"ASL", "Arithmetic shift left"}},
    {InstructionId::LSR, {"LSR", "Logical shift right"}},
    {InstructionId::ROL, {"ROL", "Rotate left"}},
    {InstructionId::ROR, {"ROR", "Rotate right"}},

    // Jumps and calls
    {InstructionId::JMP, {"JMP", "Jump to another location"}},
    {InstructionId::JSR, {"JSR", "Jump to a subroutine"}},
    {InstructionId::RTS, {"RTS", "Return from subroutine"}},

    // Branches
    {InstructionId::BCC, {"BCC", "Branch if carry flag clear"}},
    {InstructionId::BCS, {"BCS", "Branch if carry flag set"}},
    {InstructionId::BEQ, {"BEQ", "Branch if zero flag set"}},
    {InstructionId::BMI, {"BMI", "Branch if negative flag set"}},
    {InstructionId::BNE, {"BNE", "Branch if zero flag clear"}},
    {InstructionId::BPL, {"BPL", "Branch if negative flag clear"}},
    {InstructionId::BVC, {"BVC", "Branch if overflow flag clear"}},
    {InstructionId::BVS, {"BVS", "Branch if overflow flag set"}},

    // Status flag changes
    {InstructionId::CLC, {"CLC", "Clear carry flag"}},
    {InstructionId::CLD, {"CLD", "Clear decimal mode flag"}},
    {InstructionId::CLI, {"CLI", "Clear interrupt disable flag"}},
    {InstructionId::CLV, {"CLV", "Clear overflow flag"}},
    {InstructionId::SEC, {"SEC", "Set carry flag"}},
    {InstructionId::SED, {"SED", "Set decimal mode flag"}},
    {InstructionId::SEI, {"SEI", "Set interrupt disable flag"}},

    // System functions
    {InstructionId::BRK, {"BRK", "Force an interrupt"}},
    {InstructionId::NOP, {"NOP", "No operation"}},
    {InstructionId::RTI, {"RTI", "Return from interrupt"}},
};

std::string print_instruction_id(const InstructionId instruction_id)
{
    return instruction_id_map.find(instruction_id)->second.first;
}

std::string print_instruction_description(const InstructionId instruction_id)
{
    return instruction_id_map.find(instruction_id)->second.second;
}

}