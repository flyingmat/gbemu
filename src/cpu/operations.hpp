#pragma once
#include <cstdint>

namespace Cpu {
    class Cpu;
}

namespace Cpu::Operations {
    /**
     * A generic CPU operation. Must be derived to be used.
     * Each time Step() is called, a sub-operation ("microcode") is executed.
     * After the last sub-operation is executed, CPU flags may be set.
     * When Execute() is called, all remaining sub-operations are executed and
     * flags are automatically set, accordingly.
     */
    class Operation {
    public:
        const uint8_t opcode;
        Cpu* const cpu;
        uint8_t step_i;

        Operation(Cpu* const cpu, const uint8_t opcode);
        void Execute();
        virtual bool Step() = 0;
    };

    /// Generic operation editing an 8-bit unsigned integer.
    class SingleByteEditOperation : public Operation {
    public:
        uint8_t& byte;

        SingleByteEditOperation(Cpu* const cpu, const uint8_t opcode, uint8_t& byte);
    };

    /// Generic operation editing a 16-bit unsigned integer referenced as two separate bytes.
    class DoubleByteEditOperation : public Operation {
    public:
        uint8_t& upper_byte;
        uint8_t& lower_byte;

        DoubleByteEditOperation(Cpu* const cpu, const uint8_t opcode, uint8_t& upper_byte, uint8_t& lower_byte);
    };

    /// Increases an 8-bit unsigned integer and sets related flags.
    class IncreaseByte : public SingleByteEditOperation {
    private:
        void SetFlags();
    public:
        IncreaseByte(Cpu* const cpu, const uint8_t opcode, uint8_t& byte);
        virtual bool Step();
    };

    /// Decreases an 8-bit unsigned integer and sets related flags.
    class DecreaseByte : public SingleByteEditOperation {
    private:
        void SetFlags();
    public:
        DecreaseByte(Cpu* const cpu, const uint8_t opcode, uint8_t& byte);
        virtual bool Step();
    };

    enum ShiftDirection : bool {
        Left,
        Right
    };

    /// Rotates a byte in the specified direction, setting related flags.
    class RotateByte : public SingleByteEditOperation {
    private:
        void SetFlags();
    public:
        const ShiftDirection direction;
        const bool fast;

        RotateByte(Cpu* const cpu, const uint8_t opcode, uint8_t& byte, ShiftDirection direction, bool fast);
        virtual bool Step();
    };

    class LoadByte : public SingleByteEditOperation {
    public:
        const uint8_t src;
        const bool immediate;

        LoadByte(Cpu* const cpu, const uint8_t opcode, uint8_t& dst, uint8_t src, bool immediate);
        virtual bool Step();
    };

    /// Increases a 16-bit unsigned integer referenced as two separate bytes.
    class IncreaseDoubleByte : public DoubleByteEditOperation {
    public:
        IncreaseDoubleByte(Cpu* const cpu, const uint8_t opcode, uint8_t& upper_byte, uint8_t& lower_byte);
        virtual bool Step();
    };
}
