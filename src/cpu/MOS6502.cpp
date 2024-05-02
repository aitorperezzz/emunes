#include <type_traits>
#include <cstddef>
#include <iostream>

#include "MOS6502.h"

namespace cpu
{
    bool MOS6502::init(const std::vector<uint8_t> &instructions)
    {
        // Initialise variables
        pc = 0;
        acc = 0;
        xr = 0;
        yr = 0;
        sr = 0;
        sp = 0;
        opcode = Opcode();
        this->instructions.clear();
        mmio = mmio::Mmio();

        // Keep a copy of the provided instructions
        this->instructions = instructions;

        return true;
    }

    bool MOS6502::run()
    {
        // Loop through the instructions
        while (true)
        {
            // Update the current opcode
            if (!update_opcode())
            {
                return false;
            }

            // Execute the current instruction
            if (!execute_instruction())
            {
                return false;
            }
        }
    }

    bool MOS6502::update_opcode()
    {
        if (pc >= instructions.size())
        {
            return false;
        }
        opcode = opcode_parser.parse(instructions[pc]);
        return true;
    }

    bool MOS6502::execute_instruction()
    {
        std::cout << "pc " << +pc << std::endl;
        // Take value from memory depending on the addressing mode
        uint16_t address = 0;
        uint8_t value = 0;
        int8_t signed_value = 0;
        // TODO: if an instruction is only going to write to a memory location, then reading values here
        // is a waste of computation time
        switch (opcode.addressing_mode)
        {
        case AddressingMode::IMPLICIT:
            // The instruction does not need to access anything
            break;
        case AddressingMode::ACCUMULATOR:
            // It does not make sense to set an address for this instruction, so set only the value,
            // which is the accumulator
            value = acc;
            break;
        case AddressingMode::IMMEDIATE:
            // Immediate addressing allows the programmer to directly specify
            // an 8 bit constant within the instruction
            value = instructions[pc + 1];
            break;
        case AddressingMode::ZERO_PAGE:
            // An instruction using zero page addressing mode has only an 8 bit address operand.
            // This limits it to addressing only the first 256 bytes of memory (e.g. $0000 to $00FF)
            // where the most significant byte of the address is always zero.
            // In zero page mode only the least significant byte of the address is held in the instruction making it shorter
            // by one byte (important for space saving) and one less memory fetch during execution (important for speed).
            address = instructions[pc + 1];
            value = mmio.get(address);
            break;
        case AddressingMode::ZERO_PAGE_X:
            // The address to be accessed by an instruction using indexed zero page addressing is calculated
            // by taking the 8 bit zero page address from the instruction and adding the current value of the X register to it.
            // For example if the X register contains $0F and the instruction LDA $80,X is executed then the accumulator
            // will be loaded from $008F (e.g. $80 + $0F => $8F).
            address = instructions[pc + 1] + xr;
            value = mmio.get(address);
            break;
        case AddressingMode::ZERO_PAGE_Y:
            // The address to be accessed by an instruction using indexed zero page addressing is calculated
            // by taking the 8 bit zero page address from the instruction and adding the current value of the Y register to it.
            // This mode can only be used with the LDX and STX instructions.
            address = instructions[pc + 1] + yr;
            value = mmio.get(address);
            break;
        case AddressingMode::RELATIVE:
            // Relative addressing mode is used by branch instructions (e.g. BEQ, BNE, etc.) which contain a signed 8 bit relative offset
            // (e.g. -128 to +127) which is added to program counter if the condition is true. As the program counter itself is incremented
            // during instruction execution by two the effective address range for the target instruction must be
            // with -126 to +129 bytes of the branch.
            signed_value = instructions[pc + 1];
            break;
        case AddressingMode::ABSOLUTE:
            // Instructions using absolute addressing contain a full 16 bit address to identify the target location.
            address = (instructions[pc + 2] << 8) + instructions[pc + 1];
            value = mmio.get(address);
            break;
        case AddressingMode::ABSOLUTE_X:
            // The address to be accessed by an instruction using X register indexed absolute addressing is computed
            // by taking the 16 bit address from the instruction and added the contents of the X register.
            // For example if X contains $92 then an STA $2000,X instruction will store the accumulator at $2092 (e.g. $2000 + $92).
            address = (instructions[pc + 2] << 8) + instructions[pc + 1] + xr;
            value = mmio.get(address);
            break;
        case AddressingMode::ABSOLUTE_Y:
            // The Y register indexed absolute addressing mode is the same as the previous mode only with the contents
            // of the Y register added to the 16 bit address from the instruction.
            address = (instructions[pc + 2] << 8) + instructions[pc + 1] + yr;
            value = mmio.get(address);
            break;
        case AddressingMode::INDIRECT:
        {
            // JMP is the only 6502 instruction to support indirection. The instruction contains a 16 bit address which identifies the location of
            // the least significant byte of another 16 bit memory address which is the real target of the instruction.
            // For example if location $0120 contains $FC and location $0121 contains $BA then the instruction JMP ($0120) will cause
            // the next instruction execution to occur at $BAFC (e.g. the contents of $0120 and $0121).
            uint16_t address_in_instruction = (instructions[pc + 2] << 8) + instructions[pc + 1];
            address = (mmio.get(address_in_instruction + 1) << 8) + mmio.get(address_in_instruction);
            break;
        }
        case AddressingMode::INDEXED_INDIRECT:
            break;
        case AddressingMode::INDIRECT_INDEXED:
            break;
        default:
            break;
        }

        // Decide depending on the instruction id
        switch (opcode.instruction_id)
        {

            // ***************************
            // Load/store operations
            // ***************************

        case InstructionId::LDA:
            // Load accumulator
            // Loads a byte of memory into the accumulator
            // setting the zero and negative flags as appropriate.
            acc = value;
            // Zero flag
            acc == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            acc >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::LDX:
            // Load X register
            // Loads a byte of memory into the X register
            // setting the zero and negative flags as appropriate.
            xr = value;
            // Zero flag
            xr == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            xr >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::LDY:
            // Load Y register
            // Loads a byte of memory into the Y register
            // setting the zero and negative flags as appropriate.
            yr = value;
            // Zero flag
            yr == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            yr >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::STA:
            // Store accumulator
            // Stores the contents of the accumulator into memory.
            mmio.set(value, acc);
            break;
        case InstructionId::STX:
            // Store X register
            // Stores the contents of the X register into memory.
            mmio.set(value, xr);
            break;
        case InstructionId::STY:
            // Store Y register
            // Stores the contents of the Y register into memory.
            mmio.set(value, yr);
            break;

            // ***************************
            // Register transfers
            // ***************************

        case InstructionId::TAX:
            // Transfer accumulator to X
            // Copies the current contents of the accumulator into the X register
            // and sets the zero and negative flags as appropriate.
            xr = acc;
            // Zero flag
            xr == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            xr >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::TAY:
            // Transfer accumulator to Y
            // Copies the current contents of the accumulator into the Y register
            // and sets the zero and negative flags as appropriate.
            yr = acc;
            // Zero flag
            yr == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            yr >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::TXA:
            // Transfer X to accumulator
            // Copies the current contents of the X register into the accumulator
            // and sets the zero and negative flags as appropriate.
            acc = xr;
            // Zero flag
            acc == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            acc >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::TYA:
            // Transfer Y to accumulator
            // Copies the current contents of the Y register into the accumulator
            // and sets the zero and negative flags as appropriate.
            acc = yr;
            // Zero flag
            acc == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            acc >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;

            // ***************************
            // Stack operations
            // ***************************

        case InstructionId::TSX:
            // Transfer stack pointer to X
            // Copies the current contents of the stack register into the X register
            // and sets the zero and negative flags as appropriate.
            xr = sp;
            // Zero flag
            xr == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            xr >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::TXS:
            // Transfer X to stack pointer
            // Copies the current contents of the X register into the stack register.
            sp = xr;
            break;
        case InstructionId::PHA:
            // Push accumulator
            // Pushes a copy of the accumulator on to the stack.
            push_to_stack(acc);
            break;
        case InstructionId::PHP:
            // Push processor status
            // Pushes a copy of the status flags on to the stack.
            push_to_stack(sr);
            break;
        case InstructionId::PLA:
            // Pull accumulator
            // Pulls an 8 bit value from the stack and into the accumulator.
            // The zero and negative flags are set as appropriate.
            acc = pull_from_stack();
            // Zero flag
            acc == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            acc >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::PLP:
            // Pull processor status
            // Pulls an 8 bit value from the stack and into the processor flags.
            // The flags will take on new states as determined by the value pulled.
            sr = pull_from_stack();
            break;

            // ***************************
            // Logical
            // ***************************

        case InstructionId::AND:
            // Logical AND
            // A logical AND is performed, bit by bit, on the accumulator contents
            // using the contents of a byte of memory.
            acc &= value;
            // Zero flag
            acc == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            acc >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::EOR:
            // Exclusive OR
            // An exclusive OR is performed, bit by bit, on the accumulator contents
            // using the contents of a byte of memory.
            acc ^= value;
            // Zero flag
            acc == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            acc >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::ORA:
            // Logical inclusive OR
            // An inclusive OR is performed, bit by bit, on the accumulator contents
            // using the contents of a byte of memory.
            acc |= value;
            // Zero flag
            acc == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            acc >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::BIT:
        {
            // Bit test
            // This instruction is used to test if one or more bits are set in a target memory location.
            // The mask pattern in A is ANDed with the value in memory to set or clear the zero flag, but the result is not kept.
            // Bits 7 and 6 of the value from memory are copied into the N and V flags.
            uint8_t result = acc & value;
            // Carry flag
            result == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            value & 0x80 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            // Overflow flag
            value & 0x40 ? set_sr_bit(StatusRegisterBit::OVERFLOW) : clear_sr_bit(StatusRegisterBit::OVERFLOW);
            break;
        }

            // ***************************
            // Arithmetic
            // ***************************

        case InstructionId::ADC:
            // Add with carry
            // This instruction adds the contents of a memory location to the accumulator together with the carry bit.
            // If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
            adc(value);
            break;
        case InstructionId::SBC:
            // Subtract with carry
            // This instruction subtracts the contents of a memory location to the accumulator together with the not of the carry bit.
            // If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
            adc(~value);
            break;
        case InstructionId::CMP:
        {
            // Compare accumulator
            // This instruction compares the contents of the accumulator with another memory held value
            // and sets the zero and carry flags as appropriate.
            uint8_t result = acc - value;
            // Carry flag, set if acc >= value
            result >= 0 ? set_sr_bit(StatusRegisterBit::CARRY) : clear_sr_bit(StatusRegisterBit::CARRY);
            // Zero flag
            result == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            result >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        }
        case InstructionId::CPX:
        {
            // Compare X register
            // This instruction compares the contents of the X register with another memory held value
            // and sets the zero and carry flags as appropriate.
            uint8_t result = xr - value;
            // Carry flag, set if acc >= value
            result >= 0 ? set_sr_bit(StatusRegisterBit::CARRY) : clear_sr_bit(StatusRegisterBit::CARRY);
            // Zero flag
            result == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            result >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        }
        case InstructionId::CPY:
        {
            // Compare Y register
            // This instruction compares the contents of the Y register with another memory held value
            // and sets the zero and carry flags as appropriate.
            uint8_t result = yr - value;
            // Carry flag, set if acc >= value
            result >= 0 ? set_sr_bit(StatusRegisterBit::CARRY) : clear_sr_bit(StatusRegisterBit::CARRY);
            // Zero flag
            result == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            result >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        }

            // ***************************
            // Increments and decrements
            // ***************************

        case InstructionId::INC:
        {
            // Increment a memory location
            // Adds one to the value held at a specified memory location
            // setting the zero and negative flags as appropriate.
            uint8_t result = value + 1;
            mmio.set(address, result);
            // Zero flag
            result == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            result >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        }
        case InstructionId::INX:
            // Increment the X register
            // Adds one to the X register setting the zero and negative flags as appropriate.
            xr++;
            // Zero flag
            xr == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            xr >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::INY:
            // Increment the Y register
            // Adds one to the Y register setting the zero and negative flags as appropriate.
            yr++;
            // Zero flag
            yr == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            yr >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::DEC:
        {
            // Decrement a memory location
            // Subtracts one from the value held at a specified memory location
            // setting the zero and negative flags as appropriate.
            uint8_t result = value - 1;
            mmio.set(address, result);
            // Zero flag
            result == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            result >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        }
        case InstructionId::DEX:
            // Decrement the X register
            // Subtracts one from the X register setting the zero and negative flags as appropriate.
            xr--;
            // Zero flag
            xr == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            xr >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        case InstructionId::DEY:
            // Decrement the Y register
            // Subtracts one from the Y register setting the zero and negative flags as appropriate.
            yr--;
            // Zero flag
            yr == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            yr >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;

            // ***************************
            // Shifts
            // ***************************

        case InstructionId::ASL:
        {
            // Arithmetic shift left
            // This operation shifts all the bits of the accumulator or memory contents one bit left.
            // Bit 0 is set to 0 and bit 7 is placed in the carry flag.
            // The effect of this operation is to multiply the memory contents by 2 (ignoring 2's complement considerations),
            // setting the carry if the result will not fit in 8 bits.
            // Carry flag: store the bit that is going to be displaced
            value & 0x80 ? set_sr_bit(StatusRegisterBit::CARRY) : clear_sr_bit(StatusRegisterBit::CARRY);
            uint8_t result = value << 1;
            if (opcode.addressing_mode == AddressingMode::ACCUMULATOR)
            {
                acc = result;
            }
            else
            {
                mmio.set(address, result);
            }
            // Zero flag
            result == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            result >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        }
        case InstructionId::LSR:
        {
            // Logical shift right
            // Each of the bits in A or M is shift one place to the right.
            // The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
            value & 0x1 ? set_sr_bit(StatusRegisterBit::CARRY) : clear_sr_bit(StatusRegisterBit::CARRY);
            uint8_t result = value >> 1;
            if (opcode.addressing_mode == AddressingMode::ACCUMULATOR)
            {
                acc = result;
            }
            else
            {
                mmio.set(address, result);
            }
            // Zero flag
            result == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            result >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        }
        case InstructionId::ROL:
        {
            // Rotate left
            // Move each of the bits in either A or M one place to the left.
            // Bit 0 is filled with the current value of the carry flag whilst the old bit 7 becomes the new carry flag value.
            uint8_t new_carry = value & 0x80;
            uint8_t result = value << 1;
            result = get_sr_bit(StatusRegisterBit::CARRY) ? (result | 1) : (result & ~1);
            new_carry ? set_sr_bit(StatusRegisterBit::CARRY) : clear_sr_bit(StatusRegisterBit::CARRY);
            // Finally assign
            if (opcode.addressing_mode == AddressingMode::ACCUMULATOR)
            {
                acc = result;
            }
            else
            {
                mmio.set(address, result);
            }
            // Zero flag
            result == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            result >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        }
        case InstructionId::ROR:
        {
            // Rotate right
            // Move each of the bits in either A or M one place to the right.
            // Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag value.
            uint8_t new_carry = value & 0x1;
            uint8_t result = value >> 1;
            result = get_sr_bit(StatusRegisterBit::CARRY) ? (result | (1 << 7)) : (result & ~(1 << 7));
            new_carry ? set_sr_bit(StatusRegisterBit::CARRY) : clear_sr_bit(StatusRegisterBit::CARRY);
            // Finally assign
            if (opcode.addressing_mode == AddressingMode::ACCUMULATOR)
            {
                acc = result;
            }
            else
            {
                mmio.set(address, result);
            }
            // Zero flag
            result == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
            // Negative flag
            result >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
            break;
        }

            // ***************************
            // Jumps and calls
            // ***************************

        case InstructionId::JMP:
            // Jump to another location
            // Sets the program counter to the address specified by the operand.
            pc = address;
            break;
        case InstructionId::JSR:
            // Jump to a subroutine
            // The JSR instruction pushes the address (minus one) of the return point on to the stack
            // and then sets the program counter to the target memory address.
            // Note: JSR is supposed to push to the stack the address of the next instruction to execute
            // after returning from the subroutine. Because the JSR instruction is always 3 bytes long
            // (it only supports absolute addressing), one would expect it would put in the stack the current
            // address + 3, but it will put the current address +2, and RTS will pop this address from the stack
            // and perform +1 before jumping. This is to prevent the creation of additional registers in the chip.
            push_to_stack(pc + 2);
            pc = value;
            break;
        case InstructionId::RTS:
            // Return from subroutine
            // The RTS instruction is used at the end of a subroutine to return to the calling routine.
            // It pulls the program counter (minus one) from the stack.
            pc = pull_from_stack() + 1;
            break;

            // ***************************
            // Branches
            // ***************************

        case InstructionId::BCC:
            // Branch if carry flag clear
            // If the carry flag is clear then add the relative displacement
            // to the program counter to cause a branch to a new location.
            if (!get_sr_bit(StatusRegisterBit::CARRY))
            {
                pc += signed_value;
            }
            break;
        case InstructionId::BCS:
            // Branch if carry flag set
            // If the carry flag is set then add the relative displacement to the program counter
            // to cause a branch to a new location.
            if (get_sr_bit(StatusRegisterBit::CARRY))
            {
                pc += signed_value;
            }
            break;
        case InstructionId::BEQ:
            // Branch if zero flag set
            // If the zero flag is set then add the relative displacement to the program counter
            // to cause a branch to a new location.
            if (get_sr_bit(StatusRegisterBit::ZERO))
            {
                pc += signed_value;
            }
            break;
        case InstructionId::BMI:
            // Branch if negative flag set
            // If the negative flag is set then add the relative displacement to the program counter
            // to cause a branch to a new location.
            if (get_sr_bit(StatusRegisterBit::NEGATIVE))
            {
                pc += signed_value;
            }
            break;
        case InstructionId::BNE:
            // Branch if zero flag clear
            // If the zero flag is clear then add the relative displacement to the program counter
            // to cause a branch to a new location.
            if (!get_sr_bit(StatusRegisterBit::ZERO))
            {
                pc += signed_value;
            }
            break;
        case InstructionId::BPL:
            // Branch if negative flag clear
            // If the negative flag is clear then add the relative displacement to the program counter
            // to cause a branch to a new location.
            if (!get_sr_bit(StatusRegisterBit::NEGATIVE))
            {
                pc += signed_value;
            }
            break;
        case InstructionId::BVC:
            // Branch if overflow flag clear
            // If the overflow flag is clear then add the relative displacement to the program counter
            // to cause a branch to a new location.
            if (!get_sr_bit(StatusRegisterBit::OVERFLOW))
            {
                pc += signed_value;
            }
            break;
        case InstructionId::BVS:
            // Branch if overflow flag set
            // If the overflow flag is set then add the relative displacement to the program counter
            // to cause a branch to a new location.
            if (get_sr_bit(StatusRegisterBit::OVERFLOW))
            {
                pc += signed_value;
            }
            break;

            // ***************************
            // Status flag changes
            // ***************************

        case InstructionId::CLC:
            // Clear carry flag
            clear_sr_bit(StatusRegisterBit::CARRY);
            break;
        case InstructionId::CLD:
            // Clear decimal mode flag
            clear_sr_bit(StatusRegisterBit::DECIMAL);
            break;
        case InstructionId::CLI:
            // Clear interrupt disable flag
            clear_sr_bit(StatusRegisterBit::INTERRUPT);
            break;
        case InstructionId::CLV:
            // Clear overflow flag
            clear_sr_bit(StatusRegisterBit::OVERFLOW);
            break;
        case InstructionId::SEC:
            // Set carry flag
            set_sr_bit(StatusRegisterBit::CARRY);
            break;
        case InstructionId::SED:
            // Set decimal mode flag
            set_sr_bit(StatusRegisterBit::DECIMAL);
            break;
        case InstructionId::SEI:
            // Set interrupt disable flag
            set_sr_bit(StatusRegisterBit::INTERRUPT);
            break;

            // ***************************
            // System functions
            // ***************************

        case InstructionId::BRK:
            // Force an interrupt
            // The BRK instruction forces the generation of an interrupt request.
            // The program counter and processor status are pushed on the stack then the IRQ interrupt vector at $FFFE/F
            // is loaded into the PC and the break flag in the status set to one.
            push_to_stack(pc);
            push_to_stack(sr);
            pc = mmio.get(0xFFFE / 0xF);
            set_sr_bit(StatusRegisterBit::BREAK);
            break;
        case InstructionId::NOP:
            // No operation
            // The NOP instruction causes no changes to the processor other than the normal incrementing
            // of the program counter to the next instruction.
            break;
        case InstructionId::RTI:
            // Return from interrupt
            // The RTI instruction is used at the end of an interrupt processing routine.
            // It pulls the processor flags from the stack followed by the program counter.
            sr = pull_from_stack();
            pc = pull_from_stack();
            break;

        default:
            // TODO: exception
            break;
        }

        // Jump as many bytes as indicated by the opcode
        pc += opcode.instruction_size;

        return true;
    }

    uint8_t MOS6502::get_sr_bit(StatusRegisterBit bit)
    {
        return (sr >> (uint8_t)bit) & 0x1;
    }

    void MOS6502::set_sr_bit(StatusRegisterBit bit)
    {
        sr |= 1 >> static_cast<typename std::underlying_type<StatusRegisterBit>::type>(bit);
    }

    void MOS6502::clear_sr_bit(StatusRegisterBit bit)
    {
        sr &= ~(1 >> static_cast<typename std::underlying_type<StatusRegisterBit>::type>(bit));
    }

    void MOS6502::push_to_stack(uint8_t value)
    {
        mmio.set(STACK_ADDRESS + sp, value);
        sp++;
    }

    uint8_t MOS6502::pull_from_stack()
    {
        auto value = mmio.get(STACK_ADDRESS + sp);
        sp--;
        return value;
    }

    void MOS6502::adc(uint8_t value)
    {
        uint16_t result = acc + value + get_sr_bit(StatusRegisterBit::CARRY);
        // Set the carry flag if the result has overflowed the byte
        result > 0xFF ? set_sr_bit(StatusRegisterBit::CARRY) : clear_sr_bit(StatusRegisterBit::CARRY);
        // Set the overflow flag if the result has the "wrong" sign.
        // * If acc and value have different sign, one of them is negative and the other is positive.
        //   Starting from a negative number and adding a positive number we would never get a wrong sign.
        //   Even if the carry bit is set, a 1 will not change this condition
        // * Therefore we need to have acc and value with the same sign, so we have the following conditions:
        //   - Both negative and the result is positive
        //   - Both positive and the result is negative
        // * This condition is converted to both with the same sign and the result with different sign
        ~(acc ^ value) & (acc ^ result) & 0x80 ? set_sr_bit(StatusRegisterBit::OVERFLOW) : clear_sr_bit(StatusRegisterBit::OVERFLOW);
        // Assign
        acc = result & 0xFF;
        // Zero flag
        acc == 0 ? set_sr_bit(StatusRegisterBit::ZERO) : clear_sr_bit(StatusRegisterBit::ZERO);
        // Negative flag
        acc >> 7 ? set_sr_bit(StatusRegisterBit::NEGATIVE) : clear_sr_bit(StatusRegisterBit::NEGATIVE);
    }
}