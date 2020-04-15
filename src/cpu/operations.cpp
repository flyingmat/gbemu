#include <cstdio>
#include "operations.hpp"
#include "cpu.hpp"

namespace GB_Cpu::Operations {
    // const member variables are initialized and the step index is set to 0
    Operation::Operation(Cpu* const cpu, const uint8_t opcode)
        : cpu(cpu), opcode(opcode) { this->step_i = 0; }

    // all remaining sub-operations are executed
    void Operation::Execute() {
        while (!Step()) {}
    }

    // call base constructor
    SingleByteEditOperation::SingleByteEditOperation(Cpu* const cpu, const uint8_t opcode, uint8_t& byte)
        : Operation(cpu, opcode), byte(byte) {}

    // call base constructor
    DoubleByteEditOperation::DoubleByteEditOperation(Cpu* const cpu, const uint8_t opcode, uint8_t& upper_byte, uint8_t& lower_byte)
        : Operation(cpu, opcode), upper_byte(upper_byte), lower_byte(lower_byte) {}

    // call base constructor
    IncreaseByte::IncreaseByte(Cpu* const cpu, const uint8_t opcode, uint8_t& byte)
        : SingleByteEditOperation(cpu, opcode, byte) {}

    // set the z, n, h flags
    void IncreaseByte::SetFlags() {
        // zero flag is only set if 11111111 -> 00000000
        this->cpu->SetFlag(Flag::z, this->byte == 0x00);
        // negative flag is always reset
        this->cpu->SetFlag(Flag::n, 0);
        // when increasing, half carry is set if 00001111 -> 00010000
        this->cpu->SetFlag(Flag::h, this->byte == 0x10);
    }

    bool IncreaseByte::Step() {
        switch (this->step_i++) {
            // only one step: the referenced byte is increased and flags are set
            case 0:
                this->byte++;
                this->SetFlags();
                return true;
            default:
                return true;
        }
    }

    // call base constructor
    DecreaseByte::DecreaseByte(Cpu* const cpu, const uint8_t opcode, uint8_t& byte)
        : SingleByteEditOperation(cpu, opcode, byte) {}

    // set the z, n, h flags
    void DecreaseByte::SetFlags() {
        // zero flag is only set if 00000001 -> 00000000
        this->cpu->SetFlag(Flag::z, this->byte == 0x00);
        // negative flag is always set
        this->cpu->SetFlag(Flag::n, 1);
        // when increasing, half carry is set if 00010000 -> 00001111
        this->cpu->SetFlag(Flag::h, this->byte == 0x0F);
    }

    bool DecreaseByte::Step() {
        switch (this->step_i++) {
            // only one step: the referenced byte is decreased and flags are set
            case 0:
                this->byte--;
                this->SetFlags();
                return true;
            default:
                return true;
        }
    }

    RotateLeftByte::RotateLeftByte(Cpu* const cpu, const uint8_t opcode, uint8_t& byte, bool fast)
        : SingleByteEditOperation(cpu, opcode, byte) { this-> fast = fast; }

    void RotateLeftByte::SetFlags() {
        // the zero flag is only set if fast is not set
        this->cpu->SetFlag(Flag::z, (this->byte == 0) & !this->fast);
        // n and h are always reset
        this->cpu->SetFlag(Flag::n, 0);
        this->cpu->SetFlag(Flag::h, 0);
        // the carry flag is set to the bit going out left (now bit zero)
        this->cpu->SetFlag(Flag::c, this->byte & 0x01);
    }

    bool RotateLeftByte::Step() {
        // if fast is set, only case 1 will be executed
        // step_i could also be initialized to 1 if flag is set
        switch (this->step_i++ ^ (uint8_t) this->fast) {
            case 0:
                return false;
            case 1:
                // rotate the byte to the left and set flags
                byte = ((byte << 1) & 0xFF) | (byte >> 7);
                this->SetFlags();
                return true;
            default:
                return true;
        }
    }

    // call base constructor
    IncreaseDoubleByte::IncreaseDoubleByte(Cpu* const cpu, const uint8_t opcode, uint8_t& upper_byte, uint8_t& lower_byte)
        : DoubleByteEditOperation(cpu, opcode, upper_byte, lower_byte) {};

    bool IncreaseDoubleByte::Step() {
        switch (this->step_i++) {
            // increase the lower eight bits, treated as single byte
            case 0:
                this->lower_byte++;
                return false;
            // if the lower eight bits are equal to zero, carry needed
            case 1:
                if (!this->lower_byte)
                    this->upper_byte++;
                // no flags are set
                return true;
            default:
                return true;
        }
    }
}
