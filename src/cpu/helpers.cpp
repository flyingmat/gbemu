#include "helpers.hpp"
#include "cpu.hpp"

namespace Cpu::Helpers {
    uint16_t JoinBytes(const uint8_t upper_byte, const uint8_t lower_byte) {
        return (uint16_t) ((upper_byte << 8) | lower_byte);
    }

    uint8_t* DereferenceDoubleByte(Cpu* const cpu, uint8_t& upper_byte, uint8_t& lower_byte, PostOperation post_op) {
        uint8_t* memory_address = *cpu->memory + JoinBytes(upper_byte, lower_byte);
        switch (post_op) {
            case PostOperation::Increase:
                if (!++lower_byte)
                    upper_byte++;
                break;
            case PostOperation::Decrease:
                if (--lower_byte == 0xFF)
                    upper_byte--;
                break;
        }
        return memory_address;
    }

    uint8_t* DereferenceBC(Cpu* const cpu) {
        return DereferenceDoubleByte(cpu, cpu->B, cpu->C, PostOperation::None);
    }

    uint8_t* DereferenceDE(Cpu* const cpu) {
        return DereferenceDoubleByte(cpu, cpu->D, cpu->E, PostOperation::None);
    }

    uint8_t* DereferenceHL(Cpu* const cpu, PostOperation post_op) {
        return DereferenceDoubleByte(cpu, cpu->H, cpu->L, post_op);
    }

    uint8_t GetArgsNumber(uint8_t opcode) {
        if (opcode < 0x40) {
            if ((opcode & 0x0F) == 0x01)
                return 2;
            else if ((opcode & 0x0F) % 8 == 0x06)
                return 1;
            else
                return 0;
        } else if (opcode < 0x80) {
            return 0;
        }

        return 0;
    }
}
