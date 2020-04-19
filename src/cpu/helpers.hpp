#pragma once
#include <cstdint>

namespace Cpu {
    class Cpu;
}

namespace Cpu::Helpers {

    enum DoubleByteOperation : uint8_t {
        Increase,
        Decrease,
        None
    };

    /// Concatenates two bytes to form a 16-bit unsigned integer.
    uint16_t JoinBytes(const uint8_t upper_byte, const uint8_t lower_byte);

    void ExecuteDoubleByteOperation(Cpu* const cpu, uint8_t& upper_byte, uint8_t& lower_byte, const DoubleByteOperation op);

    uint8_t* DereferenceDoubleByte(Cpu* const cpu, uint8_t& upper_byte, uint8_t& lower_byte, const DoubleByteOperation post_op);
    uint8_t* DereferenceBC(Cpu* const cpu);
    uint8_t* DereferenceDE(Cpu* const cpu);
    /// Returns a pointer to the byte stored in memory at address HL.
    uint8_t* DereferenceHL(Cpu* const cpu, const DoubleByteOperation post_op);
    uint8_t* DereferenceSP(Cpu* const cpu, const DoubleByteOperation sp_op);

    uint8_t GetArgsNumber(uint8_t opcode);
}
