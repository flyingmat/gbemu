#include "helpers.hpp"
#include "cpu.hpp"

namespace Cpu::Helpers {
    uint16_t JoinBytes(const uint8_t upper_byte, const uint8_t lower_byte) {
        return (uint16_t) ((upper_byte << 8) | lower_byte);
    }

    void ExecuteDoubleByteOperation(Cpu* const cpu, uint8_t& upper_byte, uint8_t& lower_byte, const DoubleByteOperation op) {
        switch (op) {
            case DoubleByteOperation::Increase:
                if (!++lower_byte)
                    upper_byte++;
                break;
            case DoubleByteOperation::Decrease:
                if (--lower_byte == 0xFF)
                    upper_byte--;
                break;
            default:
                break;
        }
    }

    uint8_t* DereferenceDoubleByte(Cpu* const cpu, uint8_t& upper_byte, uint8_t& lower_byte, const DoubleByteOperation post_op) {
        uint8_t* memory_address = *cpu->memory + JoinBytes(upper_byte, lower_byte);
        ExecuteDoubleByteOperation(cpu, upper_byte, lower_byte, post_op);
        return memory_address;
    }

    uint8_t* DereferenceBC(Cpu* const cpu) {
        return DereferenceDoubleByte(cpu, cpu->B, cpu->C, DoubleByteOperation::None);
    }

    uint8_t* DereferenceDE(Cpu* const cpu) {
        return DereferenceDoubleByte(cpu, cpu->D, cpu->E, DoubleByteOperation::None);
    }

    uint8_t* DereferenceHL(Cpu* const cpu, const DoubleByteOperation post_op) {
        return DereferenceDoubleByte(cpu, cpu->H, cpu->L, post_op);
    }

    uint8_t* DereferenceSP(Cpu* const cpu, const DoubleByteOperation sp_op) {
        switch (sp_op) {
            case DoubleByteOperation::Increase:
            case DoubleByteOperation::None:
                return DereferenceDoubleByte(cpu, cpu->S, cpu->P, sp_op);
            case DoubleByteOperation::Decrease:
                ExecuteDoubleByteOperation(cpu, cpu->S, cpu->P, DoubleByteOperation::Decrease);
                return DereferenceDoubleByte(cpu, cpu->S, cpu->P, DoubleByteOperation::None);
        }
    }

    uint8_t GetArgsNumber(uint8_t opcode) {
        if (opcode < 0x40) {
            if ((opcode & 0x0F) == 0x01)
                return 2;
            else if ((opcode & 0x0F) % 8 == 0x06)
                return 1;
            else if ((opcode & 0x0F) % 8 == 0x00 && (opcode >> 4) > 0)
                return 1;
            else
                return 0;
        } else if (opcode < 0x80) {
            return 0;
        }

        return 0;
    }
}
