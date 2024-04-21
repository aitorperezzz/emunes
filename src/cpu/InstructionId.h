#ifndef CPU_INSTRUCTION_ID_H
#define CPU_INSTRUCTION_ID_H

#include <cstdint>

namespace cpu
{
    enum class InstructionId : uint8_t
    {
        // Load/store operations
        LDA, // Load accumulator
        LDX, // Load X register
        LDY, // Load Y register
        STA, // Store accumulator
        STX, // Store X register
        STY, // Store Y register

        // Register transfers
        TAX, // Transfer accumulator to X
        TAY, // Transfer accumulator to Y
        TXA, // Transfer X to accumulator
        TYA, // Transfer Y to accumulator

        // Stack operations
        TSX, // Transfer stack pointer to X
        TXS, // Transfer X to stack pointer
        PHA, // Push accumulator on stack
        PHP, // Push processor status on stack
        PLA, // Pull accumulator from stack
        PLP, // Pull processor status from stack

        // Logical
        AND, // Logical AND
        EOR, // Exclusive OR
        ORA, // Logical inclusive OR
        BIT, // Bit test

        // Arithmetic
        ADC, // Add with carry
        SBC, // Subtract with carry
        CMP, // Compare accumulator
        CPX, // Compare X register
        CPY, // Compare Y register

        // Increments and decrements
        INC, // Increment a memory location
        INX, // Increment the X register
        INY, // Increment the Y register
        DEC, // Decrement a memory location
        DEX, // Decrement the X register
        DEY, // Decrement the Y register

        // Shifts
        ASL, // Arithmetic shift left
        LSR, // Logical shift right
        ROL, // Rotate left
        ROR, // Rotate right

        // Jumps and calls
        JMP, // Jump to another location
        JSR, // Jump to a subroutine
        RTS, // Return from subroutine

        // Branches
        BCC, // Branch if carry flag clear
        BCS, // Branch if carry flag set
        BEQ, // Branch if zero flag set
        BMI, // Branch if negative flag set
        BNE, // Branch if zero flag clear
        BPL, // Branch if negative flag clear
        BVC, // Branch if overflow flag clear
        BVS, // Branch if overflow flag set

        // Status flag changes
        CLC, // Clear carry flag
        CLD, // Clear decimal mode flag
        CLI, // Clear interrupt disable flag
        CLV, // Clear overflow flag
        SEC, // Set carry flag
        SED, // Set decimal mode flag
        SEI, // Set interrupt disable flag

        // System functions
        BRK, // Force an interrupt
        NOP, // No operation
        RTI  // Return from interrupt
    };
}

#endif