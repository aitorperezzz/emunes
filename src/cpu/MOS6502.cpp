#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <type_traits>

#include "MOS6502.h"
#include "common/Logging.h"

namespace cpu
{
/// The stack occupies the complete second page of the memory
static constexpr uint16_t STACK_OFFSET = 0x0100;

/// First byte of the reset vector
static constexpr uint16_t RESET_VECTOR = 0xFFFC;

/// First byte of the IRQ vector
static constexpr uint16_t IRQ_VECTOR = 0xFFFE;

MOS6502::MOS6502(const std::shared_ptr<mmio::Mmio> &mmio)
{
    this->mmio = mmio;
}

void MOS6502::override_reset_vector(const uint16_t address)
{
    rv_overriden = true;
    rv_value = address;
}

void MOS6502::set_max_instructions(const size_t num_instructions)
{
    this->max_instructions = num_instructions;
}

void MOS6502::set_log_file(const std::shared_ptr<common::LogFile> &log_file)
{
    this->log_file = log_file;
}

bool MOS6502::reset()
{
    // Initialise the internal variables
    pc = 0;
    acc = 0;
    xr = 0;
    yr = 0;
    sr = 0x24;
    // The stack grows downwards, so at reset the stack needs to point
    // to 0xFF (which is in reality 0x01FF) and grows downwards from there
    sp = 0xFD; // TODO: why?
    opcode = Opcode();

    if (rv_overriden)
    {
        // If the reset vector has been overriden, take this into account
        pc = rv_value;
    }
    else
    {
        // The program counter is set to the address read at the reset vector
        pc = (static_cast<uint16_t>(mmio->get(RESET_VECTOR + 1)) << 8) + static_cast<uint16_t>(mmio->get(RESET_VECTOR));
    }

    return this->run();
}

bool MOS6502::run()
{
    // Loop through the instructions
    uint8_t opcode_raw;
    size_t count_instructions = 0;
    while (true)
    {
        // Update the current opcode
        opcode_raw = mmio->get(pc);
        common::Log(common::LogLevel::DEBUG, "-> Raw opcode: " + common::print_hex(opcode_raw, sizeof(opcode_raw)));
        opcode = opcode_parser.parse(opcode_raw);

        // Get the next bytes of the instruction, if necessary
        if (opcode.instruction_size >= 2)
        {
            instruction_byte_1 = mmio->get(pc + 1);
        }
        if (opcode.instruction_size >= 3)
        {
            instruction_byte_2 = mmio->get(pc + 2);
        }

        // Resolve the instruction (memory addresses and intermediate values)
        resolve();

        // TODO: consider fetching only when it is absolutely necessary
        fetch();

        // Add record to log file
        log_file->add_record(disassemble());

        // Execute the current instruction
        if (!execute())
        {
            return false;
        }
        count_instructions++;
        if (count_instructions == max_instructions)
        {
            this->log_file->dump();
            return true;
        }
    }
}

void MOS6502::resolve()
{
    common::Log(common::LogLevel::DEBUG, "Addressing mode: " + print_addressing_mode(opcode.addressing_mode));
    switch (opcode.addressing_mode)
    {
    case AddressingMode::IMP:
        // The instruction does not need to access anything
        break;
    case AddressingMode::ACC:
        // It does not make sense to set an address for this instruction, so set only the value,
        // which is the accumulator
        value = acc;
        break;
    case AddressingMode::IMM:
        // Immediate addressing allows the programmer to directly specify
        // an 8 bit constant within the instruction
        value = instruction_byte_1;
        break;
    case AddressingMode::ZP0:
        // An instruction using zero page addressing mode has only an 8 bit address operand.
        // This limits it to addressing only the first 256 bytes of memory (e.g. $0000 to $00FF)
        // where the most significant byte of the address is always zero.
        // In zero page mode only the least significant byte of the address is held in the instruction making it shorter
        // by one byte (important for space saving) and one less memory fetch during execution (important for speed).
        address = instruction_byte_1;
        break;
    case AddressingMode::ZPX:
        // The address to be accessed by an instruction using indexed zero page addressing is calculated
        // by taking the 8 bit zero page address from the instruction and adding the current value of the X register to
        // it. For example if the X register contains $0F and the instruction LDA $80,X is executed then the accumulator
        // will be loaded from $008F (e.g. $80 + $0F => $8F).
        address = instruction_byte_1 + xr;
        break;
    case AddressingMode::ZPY:
        // The address to be accessed by an instruction using indexed zero page addressing is calculated
        // by taking the 8 bit zero page address from the instruction and adding the current value of the Y register to
        // it. This mode can only be used with the LDX and STX instructions.
        address = instruction_byte_1 + yr;
        break;
    case AddressingMode::REL:
        // Relative addressing mode is used by branch instructions (e.g. BEQ, BNE, etc.) which contain a signed 8 bit
        // relative offset (e.g. -128 to +127) which is added to program counter if the condition is true. As the
        // program counter itself is incremented during instruction execution by two the effective address range for the
        // target instruction must be with -126 to +129 bytes of the branch.
        address = pc + (int8_t)instruction_byte_1 + opcode.instruction_size;
        break;
    case AddressingMode::ABS:
        // Instructions using absolute addressing contain a full 16 bit address to identify the target location.
        address = ((uint16_t)instruction_byte_2 << 8) + (uint16_t)instruction_byte_1;
        break;
    case AddressingMode::ABX:
        // The address to be accessed by an instruction using X register indexed absolute addressing is computed
        // by taking the 16 bit address from the instruction and added the contents of the X register.
        // For example if X contains $92 then an STA $2000,X instruction will store the accumulator at $2092 (e.g. $2000
        // + $92).
        address = ((uint16_t)instruction_byte_2 << 8) + (uint16_t)instruction_byte_1 + (uint16_t)xr;
        break;
    case AddressingMode::ABY:
        // The Y register indexed absolute addressing mode is the same as the previous mode only with the contents
        // of the Y register added to the 16 bit address from the instruction.
        address = ((uint16_t)instruction_byte_2 << 8) + (uint16_t)instruction_byte_1 + (uint16_t)yr;
        break;
    case AddressingMode::IND: {
        // JMP is the only 6502 instruction to support indirection. The instruction contains a 16 bit address which
        // identifies the location of the least significant byte of another 16 bit memory address which is the real
        // target of the instruction. For example if location $0120 contains $FC and location $0121 contains $BA then
        // the instruction JMP ($0120) will cause the next instruction execution to occur at $BAFC (e.g. the contents of
        // $0120 and $0121).
        // Find the address given by the instruction
        intermediate_address = ((uint16_t)instruction_byte_2 << 8) + (uint16_t)instruction_byte_1;
        // Find the address that is stored at the location
        address = ((uint16_t)mmio->get(intermediate_address + 1) << 8) + (uint16_t)mmio->get(intermediate_address);
        break;
    }
    case AddressingMode::IXI:
        // Indexed indirect addressing is normally used in conjunction with a table of address held on zero page. The
        // address of the table is taken from the instruction and the X register added to it (with zero page wrap
        // around) to give the location of the least significant byte of the target address.
        // Perform indexing
        intermediate_address = instruction_byte_1 + xr;
        // Find the address stored there
        address = ((uint16_t)mmio->get(intermediate_address + 1) << 8) + (uint16_t)mmio->get(intermediate_address);
        break;
    case AddressingMode::IIX:
        // Indirect indexed addressing is the most common indirection mode used on the 6502. The instruction contains
        // the zero page location of the least significant byte of 16 bit address. The Y register is dynamically added
        // to this value to generated the actual target address for operation.
        // Perform indirection
        intermediate_address = mmio->get(((uint16_t)instruction_byte_1 << 8) + (uint16_t)(instruction_byte_1 + 1));
        // Perform indexing
        address = intermediate_address + (uint16_t)yr;
        break;
    default:
        break;
    }
}

bool MOS6502::execute()
{
    // By default, the PC will advance, at the end of the execution, by as many
    // bytes as the instruction size
    advance_pc = true;

    // Decide depending on the instruction id
    common::Log(common::LogLevel::DEBUG, "Execute instruction " + print_instruction_id(opcode.instruction_id));
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
        set_sr_bit(StatusRegisterBit::ZERO, acc == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, acc >> 7);
        break;
    case InstructionId::LDX:
        // Load X register
        // Loads a byte of memory into the X register
        // setting the zero and negative flags as appropriate.
        xr = value;
        set_sr_bit(StatusRegisterBit::ZERO, xr == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, xr >> 7);
        break;
    case InstructionId::LDY:
        // Load Y register
        // Loads a byte of memory into the Y register
        // setting the zero and negative flags as appropriate.
        yr = value;
        set_sr_bit(StatusRegisterBit::ZERO, yr == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, yr >> 7);
        break;
    case InstructionId::STA:
        // Store accumulator
        // Stores the contents of the accumulator into memory.
        mmio->set(address, acc);
        break;
    case InstructionId::STX:
        // Store X register
        // Stores the contents of the X register into memory.
        mmio->set(address, xr);
        break;
    case InstructionId::STY:
        // Store Y register
        // Stores the contents of the Y register into memory.
        mmio->set(address, yr);
        break;

        // ***************************
        // Register transfers
        // ***************************

    case InstructionId::TAX:
        // Transfer accumulator to X
        // Copies the current contents of the accumulator into the X register
        // and sets the zero and negative flags as appropriate.
        xr = acc;
        set_sr_bit(StatusRegisterBit::ZERO, xr == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, xr >> 7);
        break;
    case InstructionId::TAY:
        // Transfer accumulator to Y
        // Copies the current contents of the accumulator into the Y register
        // and sets the zero and negative flags as appropriate.
        yr = acc;
        set_sr_bit(StatusRegisterBit::ZERO, yr == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, yr >> 7);
        break;
    case InstructionId::TXA:
        // Transfer X to accumulator
        // Copies the current contents of the X register into the accumulator
        // and sets the zero and negative flags as appropriate.
        acc = xr;
        set_sr_bit(StatusRegisterBit::ZERO, acc == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, acc >> 7);
        break;
    case InstructionId::TYA:
        // Transfer Y to accumulator
        // Copies the current contents of the Y register into the accumulator
        // and sets the zero and negative flags as appropriate.
        acc = yr;
        set_sr_bit(StatusRegisterBit::ZERO, acc == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, acc >> 7);
        break;

        // ***************************
        // Stack operations
        // ***************************

    case InstructionId::TSX:
        // Transfer stack pointer to X
        // Copies the current contents of the stack register into the X register
        // and sets the zero and negative flags as appropriate.
        xr = sp;
        set_sr_bit(StatusRegisterBit::ZERO, xr == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, xr >> 7);
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
        // Note: PHP will push the sr with the B flag set
        // Note: the ignored flag is always pushed as 1
        push_to_stack(sr | (1 << (uint8_t)StatusRegisterBit::BREAK) | (1 << (uint8_t)StatusRegisterBit::IGNORED));
        break;
    case InstructionId::PLA:
        // Pull accumulator
        // Pulls an 8 bit value from the stack and into the accumulator.
        // The zero and negative flags are set as appropriate.
        acc = pull_from_stack();
        set_sr_bit(StatusRegisterBit::ZERO, acc == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, acc >> 7);
        break;
    case InstructionId::PLP:
        // Pull processor status
        // Pulls an 8 bit value from the stack and into the processor flags.
        // The flags will take on new states as determined by the value pulled.
        // Note: the B flag is cleared and the I flag is set before storing the value in the status register
        sr = ((pull_from_stack() & ~(1 << (uint8_t)StatusRegisterBit::BREAK)) |
              (1 << (uint8_t)StatusRegisterBit::IGNORED));
        break;

        // ***************************
        // Logical
        // ***************************

    case InstructionId::AND:
        // Logical AND
        // A logical AND is performed, bit by bit, on the accumulator contents
        // using the contents of a byte of memory.
        acc &= value;
        set_sr_bit(StatusRegisterBit::ZERO, acc == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, acc >> 7);
        break;
    case InstructionId::EOR:
        // Exclusive OR
        // An exclusive OR is performed, bit by bit, on the accumulator contents
        // using the contents of a byte of memory.
        acc ^= value;
        set_sr_bit(StatusRegisterBit::ZERO, acc == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, acc >> 7);
        break;
    case InstructionId::ORA:
        // Logical inclusive OR
        // An inclusive OR is performed, bit by bit, on the accumulator contents
        // using the contents of a byte of memory.
        acc |= value;
        set_sr_bit(StatusRegisterBit::ZERO, acc == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, acc >> 7);
        break;
    case InstructionId::BIT: {
        // Bit test
        // This instruction is used to test if one or more bits are set in a target memory location.
        // The mask pattern in A is ANDed with the value in memory to set or clear the zero flag, but the result is not
        // kept. Bits 7 and 6 of the value from memory are copied into the N and V flags.
        uint8_t result = acc & value;
        set_sr_bit(StatusRegisterBit::ZERO, result == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, value & 0x80);
        set_sr_bit(StatusRegisterBit::OVERFLOW, value & 0x40);
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
        // This instruction subtracts the contents of a memory location to the accumulator together with the not of the
        // carry bit. If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
        adc(~value);
        break;
    case InstructionId::CMP: {
        // Compare accumulator
        // This instruction compares the contents of the accumulator with another memory held value
        // and sets the zero and carry flags as appropriate.
        uint8_t result = acc - value;
        set_sr_bit(StatusRegisterBit::CARRY, acc >= value);
        set_sr_bit(StatusRegisterBit::ZERO, acc == value);
        set_sr_bit(StatusRegisterBit::NEGATIVE, result >> 7);
        break;
    }
    case InstructionId::CPX: {
        // Compare X register
        // This instruction compares the contents of the X register with another memory held value
        // and sets the zero and carry flags as appropriate.
        uint8_t result = xr - value;
        set_sr_bit(StatusRegisterBit::CARRY, xr >= value);
        set_sr_bit(StatusRegisterBit::ZERO, xr == value);
        set_sr_bit(StatusRegisterBit::NEGATIVE, result >> 7);
        break;
    }
    case InstructionId::CPY: {
        // Compare Y register
        // This instruction compares the contents of the Y register with another memory held value
        // and sets the zero and carry flags as appropriate.
        uint8_t result = yr - value;
        set_sr_bit(StatusRegisterBit::CARRY, yr >= value);
        set_sr_bit(StatusRegisterBit::ZERO, yr == value);
        set_sr_bit(StatusRegisterBit::NEGATIVE, result >> 7);
        break;
    }

        // ***************************
        // Increments and decrements
        // ***************************

    case InstructionId::INC: {
        // Increment a memory location
        // Adds one to the value held at a specified memory location
        // setting the zero and negative flags as appropriate.
        uint8_t result = value + 1;
        mmio->set(address, result);
        set_sr_bit(StatusRegisterBit::ZERO, result == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, result >> 7);
        break;
    }
    case InstructionId::INX:
        // Increment the X register
        // Adds one to the X register setting the zero and negative flags as appropriate.
        xr++;
        set_sr_bit(StatusRegisterBit::ZERO, xr == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, xr >> 7);
        break;
    case InstructionId::INY:
        // Increment the Y register
        // Adds one to the Y register setting the zero and negative flags as appropriate.
        yr++;
        set_sr_bit(StatusRegisterBit::ZERO, yr == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, yr >> 7);
        break;
    case InstructionId::DEC: {
        // Decrement a memory location
        // Subtracts one from the value held at a specified memory location
        // setting the zero and negative flags as appropriate.
        uint8_t result = value - 1;
        mmio->set(address, result);
        set_sr_bit(StatusRegisterBit::ZERO, result == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, result >> 7);
        break;
    }
    case InstructionId::DEX:
        // Decrement the X register
        // Subtracts one from the X register setting the zero and negative flags as appropriate.
        xr--;
        set_sr_bit(StatusRegisterBit::ZERO, xr == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, xr >> 7);
        break;
    case InstructionId::DEY:
        // Decrement the Y register
        // Subtracts one from the Y register setting the zero and negative flags as appropriate.
        yr--;
        set_sr_bit(StatusRegisterBit::ZERO, yr == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, yr >> 7);
        break;

        // ***************************
        // Shifts
        // ***************************

    case InstructionId::ASL: {
        // Arithmetic shift left
        // This operation shifts all the bits of the accumulator or memory contents one bit left.
        // Bit 0 is set to 0 and bit 7 is placed in the carry flag.
        // The effect of this operation is to multiply the memory contents by 2 (ignoring 2's complement
        // considerations), setting the carry if the result will not fit in 8 bits. Carry flag: store the bit that is
        // going to be displaced
        set_sr_bit(StatusRegisterBit::CARRY, value & 0x80);
        uint8_t result = value << 1;
        if (opcode.addressing_mode == AddressingMode::ACC)
        {
            acc = result;
        }
        else
        {
            mmio->set(address, result);
        }
        set_sr_bit(StatusRegisterBit::ZERO, result == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, result >> 7);
        break;
    }
    case InstructionId::LSR: {
        // Logical shift right
        // Each of the bits in A or M is shift one place to the right.
        // The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
        set_sr_bit(StatusRegisterBit::CARRY, value & 0x1);
        uint8_t result = value >> 1;
        if (opcode.addressing_mode == AddressingMode::ACC)
        {
            acc = result;
        }
        else
        {
            mmio->set(address, result);
        }
        set_sr_bit(StatusRegisterBit::ZERO, result == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, result >> 7);
        break;
    }
    case InstructionId::ROL: {
        // Rotate left
        // Move each of the bits in either A or M one place to the left.
        // Bit 0 is filled with the current value of the carry flag whilst the old bit 7 becomes the new carry flag
        // value.
        uint8_t new_carry = value & 0x80;
        uint8_t result = value << 1;
        result = get_sr_bit(StatusRegisterBit::CARRY) ? (result | 1) : (result & ~1);
        set_sr_bit(StatusRegisterBit::CARRY, new_carry);
        // Finally assign
        if (opcode.addressing_mode == AddressingMode::ACC)
        {
            acc = result;
        }
        else
        {
            mmio->set(address, result);
        }
        set_sr_bit(StatusRegisterBit::ZERO, result == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, result >> 7);
        break;
    }
    case InstructionId::ROR: {
        // Rotate right
        // Move each of the bits in either A or M one place to the right.
        // Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag
        // value.
        uint8_t new_carry = value & 0x1;
        uint8_t result = value >> 1;
        result = get_sr_bit(StatusRegisterBit::CARRY) ? (result | (1 << 7)) : (result & ~(1 << 7));
        set_sr_bit(StatusRegisterBit::CARRY, new_carry);
        // Finally assign
        if (opcode.addressing_mode == AddressingMode::ACC)
        {
            acc = result;
        }
        else
        {
            mmio->set(address, result);
        }
        set_sr_bit(StatusRegisterBit::ZERO, result == 0);
        set_sr_bit(StatusRegisterBit::NEGATIVE, result >> 7);
        break;
    }

        // ***************************
        // Jumps and calls
        // ***************************

    case InstructionId::JMP:
        // Jump to another location
        // Sets the program counter to the address specified by the operand.
        pc = address;
        advance_pc = false;
        break;
    case InstructionId::JSR: {
        // Jump to a subroutine
        // The JSR instruction pushes the address (minus one) of the return point on to the stack
        // and then sets the program counter to the target memory address.
        // Note: JSR is supposed to push to the stack the address of the next instruction to execute
        // after returning from the subroutine. Because the JSR instruction is always 3 bytes long
        // (it only supports absolute addressing), one would expect it would put in the stack the current
        // address +3, but it will put the current address +2, and RTS will pop this address from the stack
        // and perform +1 before jumping. This is to prevent the creation of additional registers in the chip.
        uint16_t address_to_stack = pc + 2;
        // The LSB will end up at the top of the stack
        push_to_stack((uint8_t)(address_to_stack >> 8));
        push_to_stack((uint8_t)(address_to_stack & 0xFF));
        pc = address;
        advance_pc = false;
        break;
    }
    case InstructionId::RTS: {
        // Return from subroutine
        // The RTS instruction is used at the end of a subroutine to return to the calling routine.
        // It pulls the program counter (minus one) from the stack.
        // The LSB is available at the top of the stack
        uint16_t address_in_stack = (uint16_t)pull_from_stack();
        address_in_stack += (uint16_t)pull_from_stack() << 8;
        pc = address_in_stack + 1;
        advance_pc = false;
        break;
    }

        // ***************************
        // Branches
        // ***************************

    case InstructionId::BCC:
        // Branch if carry flag clear
        // If the carry flag is clear then add the relative displacement
        // to the program counter to cause a branch to a new location.
        if (!get_sr_bit(StatusRegisterBit::CARRY))
        {
            pc = address;
            advance_pc = false;
        }
        break;
    case InstructionId::BCS:
        // Branch if carry flag set
        // If the carry flag is set then add the relative displacement to the program counter
        // to cause a branch to a new location.
        if (get_sr_bit(StatusRegisterBit::CARRY))
        {
            pc = address;
            advance_pc = false;
        }
        break;
    case InstructionId::BEQ:
        // Branch if zero flag set
        // If the zero flag is set then add the relative displacement to the program counter
        // to cause a branch to a new location.
        if (get_sr_bit(StatusRegisterBit::ZERO))
        {
            pc = address;
            advance_pc = false;
        }
        break;
    case InstructionId::BMI:
        // Branch if negative flag set
        // If the negative flag is set then add the relative displacement to the program counter
        // to cause a branch to a new location.
        if (get_sr_bit(StatusRegisterBit::NEGATIVE))
        {
            pc = address;
            advance_pc = false;
        }
        break;
    case InstructionId::BNE:
        // Branch if zero flag clear
        // If the zero flag is clear then add the relative displacement to the program counter
        // to cause a branch to a new location.
        if (!get_sr_bit(StatusRegisterBit::ZERO))
        {
            pc = address;
            advance_pc = false;
        }
        break;
    case InstructionId::BPL:
        // Branch if negative flag clear
        // If the negative flag is clear then add the relative displacement to the program counter
        // to cause a branch to a new location.
        if (!get_sr_bit(StatusRegisterBit::NEGATIVE))
        {
            pc = address;
            advance_pc = false;
        }
        break;
    case InstructionId::BVC:
        // Branch if overflow flag clear
        // If the overflow flag is clear then add the relative displacement to the program counter
        // to cause a branch to a new location.
        if (!get_sr_bit(StatusRegisterBit::OVERFLOW))
        {
            pc = address;
            advance_pc = false;
        }
        break;
    case InstructionId::BVS:
        // Branch if overflow flag set
        // If the overflow flag is set then add the relative displacement to the program counter
        // to cause a branch to a new location.
        if (get_sr_bit(StatusRegisterBit::OVERFLOW))
        {
            pc = address;
            advance_pc = false;
        }
        break;

        // ***************************
        // Status flag changes
        // ***************************

    case InstructionId::CLC:
        // Clear carry flag
        set_sr_bit(StatusRegisterBit::CARRY, 0);
        break;
    case InstructionId::CLD:
        // Clear decimal mode flag
        set_sr_bit(StatusRegisterBit::DECIMAL, 0);
        break;
    case InstructionId::CLI:
        // Clear interrupt disable flag
        set_sr_bit(StatusRegisterBit::INTERRUPT, 0);
        break;
    case InstructionId::CLV:
        // Clear overflow flag
        set_sr_bit(StatusRegisterBit::OVERFLOW, 0);
        break;
    case InstructionId::SEC:
        // Set carry flag
        set_sr_bit(StatusRegisterBit::CARRY, 1);
        break;
    case InstructionId::SED:
        // Set decimal mode flag
        set_sr_bit(StatusRegisterBit::DECIMAL, 1);
        break;
    case InstructionId::SEI:
        // Set interrupt disable flag
        set_sr_bit(StatusRegisterBit::INTERRUPT, 1);
        break;

        // ***************************
        // System functions
        // ***************************

    case InstructionId::BRK:
        // Force an interrupt
        // The BRK instruction forces the generation of an interrupt request.
        // The program counter and processor status are pushed on the stack then the IRQ interrupt vector at $FFFE/F
        // is loaded into the PC and the break flag in the status set to one.
        push_to_stack((uint8_t)(pc >> 8));
        push_to_stack((uint8_t)(pc & 0xFF));
        // Note: BRK will push the sr with the B flag set
        // Note: the ignored flag is always pushed as 1
        push_to_stack(sr | (1 << (uint8_t)StatusRegisterBit::BREAK) | (1 << (uint8_t)StatusRegisterBit::IGNORED));
        pc = (static_cast<uint16_t>(mmio->get(IRQ_VECTOR + 1)) << 8) + static_cast<uint16_t>(mmio->get(IRQ_VECTOR));
        advance_pc = false;
        break;
    case InstructionId::NOP:
        // No operation
        // The NOP instruction causes no changes to the processor other than the normal incrementing
        // of the program counter to the next instruction.
        break;
    case InstructionId::RTI: {
        // Return from interrupt
        // The RTI instruction is used at the end of an interrupt processing routine.
        // It pulls the processor flags from the stack followed by the program counter.
        sr = ((pull_from_stack() & ~(1 << (uint8_t)StatusRegisterBit::BREAK)) |
              (1 << (uint8_t)StatusRegisterBit::IGNORED));
        uint8_t pc_lsb = pull_from_stack();
        uint8_t pc_msb = pull_from_stack();
        pc = ((uint16_t)pc_msb << 8) + (uint16_t)pc_lsb;
        advance_pc = false;
        break;
    }

    default:
        // TODO: exception
        break;
    }

    // Jump as many bytes as indicated by the opcode
    if (advance_pc)
    {
        common::Log(common::LogLevel::DEBUG, "Increase PC " + std::to_string(+opcode.instruction_size) + " bytes");
        pc += opcode.instruction_size;
    }
    else
    {
        common::Log(common::LogLevel::DEBUG, "PC not increased");
    }
    common::Log(common::LogLevel::DEBUG, "CPU status: " + print_status());

    return true;
}

void MOS6502::fetch()
{
    switch (opcode.addressing_mode)
    {
    case AddressingMode::IMP:
    case AddressingMode::ACC:
    case AddressingMode::IMM:
    case AddressingMode::REL:
        break;
    case AddressingMode::ZP0:
    case AddressingMode::ZPX:
    case AddressingMode::ZPY:
    case AddressingMode::ABS:
    case AddressingMode::ABX:
    case AddressingMode::ABY:
    case AddressingMode::IND:
    case AddressingMode::IXI:
    case AddressingMode::IIX:
        value = mmio->get(address);
        break;
    }
}

uint8_t MOS6502::get_sr_bit(StatusRegisterBit bit)
{
    return (sr >> (uint8_t)bit) & 0x1;
}

void MOS6502::set_sr_bit(StatusRegisterBit bit, const uint8_t value)
{
    if (value)
    {
        sr |= 1 << static_cast<typename std::underlying_type<StatusRegisterBit>::type>(bit);
    }
    else
    {
        sr &= ~(1 << static_cast<typename std::underlying_type<StatusRegisterBit>::type>(bit));
    }
}

void MOS6502::push_to_stack(uint8_t value)
{
    mmio->set(STACK_OFFSET + (uint16_t)sp, value);
    sp--;
}

uint8_t MOS6502::pull_from_stack()
{
    sp++;
    return mmio->get(STACK_OFFSET + (uint16_t)sp);
}

void MOS6502::adc(uint8_t value)
{
    uint16_t result = acc + value + get_sr_bit(StatusRegisterBit::CARRY);
    // Set the carry flag if the result has overflowed the byte
    set_sr_bit(StatusRegisterBit::CARRY, result > 0xFF);
    // Set the overflow flag if the result has the "wrong" sign.
    // * If acc and value have different sign, one of them is negative and the other is positive.
    //   Starting from a negative number and adding a positive number we would never get a wrong sign.
    //   Even if the carry bit is set, a 1 will not change this condition
    // * Therefore we need to have acc and value with the same sign, so we have the following conditions:
    //   - Both negative and the result is positive
    //   - Both positive and the result is negative
    // * This condition is converted to both with the same sign and the result with different sign
    set_sr_bit(StatusRegisterBit::OVERFLOW, ~(acc ^ value) & (acc ^ result) & 0x80);
    // Assign
    acc = result & 0xFF;
    set_sr_bit(StatusRegisterBit::ZERO, acc == 0);
    set_sr_bit(StatusRegisterBit::NEGATIVE, acc >> 7);
}

std::string MOS6502::print_status()
{
    std::stringstream status;

    // Status register
    status << "SR[";
    status << "N" << (get_sr_bit(StatusRegisterBit::NEGATIVE) ? "1" : "0") << " ";
    status << "V" << (get_sr_bit(StatusRegisterBit::OVERFLOW) ? "1" : "0") << " ";
    status << "I" << (get_sr_bit(StatusRegisterBit::IGNORED) ? "1" : "0") << " ";
    status << "B" << (get_sr_bit(StatusRegisterBit::BREAK) ? "1" : "0") << " ";
    status << "D" << (get_sr_bit(StatusRegisterBit::DECIMAL) ? "1" : "0") << " ";
    status << "I" << (get_sr_bit(StatusRegisterBit::INTERRUPT) ? "1" : "0") << " ";
    status << "Z" << (get_sr_bit(StatusRegisterBit::ZERO) ? "1" : "0") << " ";
    status << "C" << (get_sr_bit(StatusRegisterBit::CARRY) ? "1" : "0");
    status << "] ";

    // Accumulator
    status << "ACC[" << common::print_hex(acc, sizeof(acc)) << "] ";

    // X register
    status << "X[" << common::print_hex(xr, sizeof(xr)) << "] ";

    // Y register
    status << "Y[" << common::print_hex(yr, sizeof(yr)) << "] ";

    // Stack pointer
    status << "SP[" << common::print_hex(sp, sizeof(sp)) << "] ";

    return status.str();
}

std::string MOS6502::disassemble()
{
    std::stringstream result;

    // Program counter
    result << common::print_hex(pc, sizeof(pc)) << "  ";

    // Bulk of bytes of the current instruction
    std::string instruction_bytes;
    instruction_bytes += common::print_hex(opcode.raw, sizeof(uint8_t)) + " ";
    if (opcode.instruction_size >= 2)
    {
        instruction_bytes += common::print_hex(instruction_byte_1, sizeof(instruction_byte_1)) + " ";
    }
    if (opcode.instruction_size >= 3)
    {
        instruction_bytes += common::print_hex(instruction_byte_2, sizeof(instruction_byte_2)) + " ";
    }
    result << std::left << std::setw(10) << instruction_bytes;

    // Name of the instruction
    result << print_instruction_id(opcode.instruction_id) << " ";

    // Addressing
    std::string details;
    switch (opcode.addressing_mode)
    {
    case AddressingMode::IMP:
        // Implicit
        break;
    case AddressingMode::ACC:
        // Accumulator
        // example "LSR A"
        details += "A";
        break;
    case AddressingMode::IMM:
        // Immediate
        // example "LDA #$66"
        details += "#$" + common::print_hex(instruction_byte_1, sizeof(instruction_byte_1));
        break;
    case AddressingMode::ZP0:
        // Zero page
        // example "LDA $33 = 44"
        details += "$" + common::print_hex(instruction_byte_1, sizeof(instruction_byte_1)) + " = " +
                   common::print_hex(value, sizeof(value));
        break;
    case AddressingMode::ZPX:
        // Zero page X
        // example "LDY $33,X @ 33 = AA"
        details += "$" + common::print_hex(instruction_byte_1, sizeof(instruction_byte_1)) + ",X @ " +
                   common::print_hex(address, sizeof(address)) + " = " + common::print_hex(value, sizeof(value));
        break;
    case AddressingMode::ZPY:
        // Zero page Y
        // example "LDY $33,Y @ 33 = AA"
        details += "$" + common::print_hex(instruction_byte_1, sizeof(instruction_byte_1)) + ",Y @ " +
                   common::print_hex(address, sizeof(address)) + " = " + common::print_hex(value, sizeof(value));
        break;
    case AddressingMode::REL:
        // Relative
        details += "$" + common::print_hex(address, sizeof(address));
        break;
    case AddressingMode::ABS:
        // Absolute
        details += "$" + common::print_hex(address, sizeof(address));
        switch (opcode.instruction_id)
        {
        case InstructionId::LDA:
        case InstructionId::LDX:
        case InstructionId::LDY:
        case InstructionId::STA:
        case InstructionId::STX:
        case InstructionId::STY:
            details += " = " + common::print_hex(value, sizeof(value));
            break;
        default:
            break;
        }
        break;
    case AddressingMode::ABX:
        // Absolute X
        // example "LDA $0300,X @ 0300 = 89"
        details +=
            "$" +
            common::print_hex(((uint16_t)instruction_byte_2 << 8) + (uint16_t)instruction_byte_1, sizeof(uint16_t)) +
            ",X @ " + common::print_hex(address, sizeof(address)) + " = " + common::print_hex(value, sizeof(value));
        break;
    case AddressingMode::ABY:
        // Absolute Y
        // example "LDA $0300,Y @ 0300 = 89"
        details +=
            "$" +
            common::print_hex(((uint16_t)instruction_byte_2 << 8) + (uint16_t)instruction_byte_1, sizeof(uint16_t)) +
            ",Y @ " + common::print_hex(address, sizeof(address)) + " = " + common::print_hex(value, sizeof(value));
        break;
    case AddressingMode::IND:
        // Indirect
        // example "JMP ($0200) = DB7E"
        details +=
            "($" +
            common::print_hex(((uint16_t)instruction_byte_2 << 8) + (uint16_t)instruction_byte_1, sizeof(uint16_t)) +
            ") = " + common::print_hex(address, sizeof(address));
        break;
    case AddressingMode::IXI:
        // Indexed indirect
        // example "LDA ($80,X) @ 80 = 0200 = 5A"
        details += "($" + common::print_hex(instruction_byte_1, sizeof(instruction_byte_1)) + ",X) @ " +
                   common::print_hex((uint8_t)intermediate_address, sizeof(uint8_t)) + " = " +
                   common::print_hex(address, sizeof(address)) + " = " + common::print_hex(value, sizeof(value));
        break;
    case AddressingMode::IIX:
        // Indirect indexed
        // example "LDA ($89),Y = 0300 @ 0300 = 89"
        details += "($" + common::print_hex(instruction_byte_1, sizeof(instruction_byte_1)) +
                   "),Y = " + common::print_hex(intermediate_address, sizeof(intermediate_address)) + " @ " +
                   common::print_hex(address, sizeof(address)) + " = " + common::print_hex(value, sizeof(value));
        break;
    }
    result << std::left << std::setw(28) << details;

    // Accumulator
    result << "A:" << common::print_hex(acc, sizeof(acc)) << " ";

    // X
    result << "X:" << common::print_hex(xr, sizeof(xr)) << " ";

    // Y
    result << "Y:" << common::print_hex(yr, sizeof(yr)) << " ";

    // P
    result << "P:" << common::print_hex(sr, sizeof(sr)) << " ";

    // Stack pointer
    result << "SP:" << common::print_hex(sp, sizeof(sp));

    return result.str();
}

} // namespace cpu