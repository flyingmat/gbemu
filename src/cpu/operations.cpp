#include "operations.hpp"

namespace GB_Cpu::Operations {
    // const member variables are initialized and the step index is set to 0
    Operation::Operation(Cpu* const cpu, const uint8_t opcode)
        : cpu(cpu), opcode(opcode) {
        this->step_i = 0;
    }

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
            : SingleByteEditOperation(cpu, opcode, byte) {};

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
