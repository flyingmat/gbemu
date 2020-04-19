#pragma once
#include <cstdint>
#include <memory>

namespace Cpu {
    class Cpu;
    enum class Flag : uint8_t;
}

namespace Cpu::Operations {
    /**
     * A generic CPU operation. Must be derived to be used.
     * Each time Step() is called, a sub-operation ("microcode") shall be executed.
     * After the last sub-operation is executed, CPU flags may be set.
     */
    class Operation {
    public:
        Cpu* const cpu;
        uint8_t step_i;

        Operation(Cpu* const cpu);
        virtual bool Step() = 0;
    };

    /**
     * CPU instruction. Each time Step() is called, a sub-operation is executed.
     * When Execute() is called, all remaining sub-operations are executed and
     * flags are automatically set, accordingly.
     */
    class Instruction {
    public:
        const std::shared_ptr<Operation> operation;
        const uint8_t opcode;
        const std::shared_ptr<uint8_t[]> args;
        const uint8_t extra_steps;
        uint8_t extra_step_i;

        Instruction(const std::shared_ptr<Operation> operation, const uint8_t opcode, const std::shared_ptr<uint8_t[]> args, const uint8_t extra_steps);
        bool Step();
        void Execute();
    };

    /// Increases an 8-bit unsigned integer and sets related flags.
    class IncreaseByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& byte;

        IncreaseByte(Cpu* const cpu, uint8_t& byte);
        virtual bool Step();
    };

    /// Decreases an 8-bit unsigned integer and sets related flags.
    class DecreaseByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& byte;

        DecreaseByte(Cpu* const cpu, uint8_t& byte);
        virtual bool Step();
    };

    enum ShiftDirection : bool {
        Left,
        Right
    };

    /// Rotates a byte in the specified direction, setting related flags.
    class RotateByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& byte;
        const ShiftDirection direction;
        const bool fast;

        RotateByte(Cpu* const cpu, uint8_t& byte, const ShiftDirection direction, const bool fast);
        virtual bool Step();
    };

    /// Loads a byte to the specified destination.
    class LoadByte : public Operation {
    public:
        uint8_t& dst;
        const uint8_t src;

        LoadByte(Cpu* const cpu, uint8_t& dst, const uint8_t src);
        virtual bool Step();
    };

    class AddByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& dst;
        const uint8_t src;

        AddByte(Cpu* const cpu, uint8_t& dst, const uint8_t src);
        virtual bool Step();
    };

    class AdcByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& dst;
        const uint8_t src;

        AdcByte(Cpu* const cpu, uint8_t& dst, const uint8_t src);
        virtual bool Step();
    };

    class SubByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& dst;
        const uint8_t src;

        SubByte(Cpu* const cpu, uint8_t& dst, const uint8_t src);
        virtual bool Step();
    };

    class SbcByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& dst;
        const uint8_t src;

        SbcByte(Cpu* const cpu, uint8_t& dst, const uint8_t src);
        virtual bool Step();
    };

    class AndByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& dst;
        const uint8_t src;

        AndByte(Cpu* const cpu, uint8_t& dst, const uint8_t src);
        virtual bool Step();
    };

    class XorByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& dst;
        const uint8_t src;

        XorByte(Cpu* const cpu, uint8_t& dst, const uint8_t src);
        virtual bool Step();
    };

    class OrByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& dst;
        const uint8_t src;

        OrByte(Cpu* const cpu, uint8_t& dst, const uint8_t src);
        virtual bool Step();
    };

    class CpByte : public Operation {
    public:
        const uint8_t dst;
        const uint8_t src;

        CpByte(Cpu* const cpu, const uint8_t dst, const uint8_t src);
        virtual bool Step();
    };

    /// Increases the value of an unsigned 16-bit integer treated as two bytes.
    class IncreaseDoubleByte : public Operation {
    public:
        uint8_t& upper_byte;
        uint8_t& lower_byte;

        IncreaseDoubleByte(Cpu* const cpu, uint8_t& upper_byte, uint8_t& lower_byte);
        virtual bool Step();
    };

    /// Decreases the value of an unsigned 16-bit integer treated as two bytes.
    class DecreaseDoubleByte : public Operation {
    public:
        uint8_t& upper_byte;
        uint8_t& lower_byte;

        DecreaseDoubleByte(Cpu* const cpu, uint8_t& upper_byte, uint8_t& lower_byte);
        virtual bool Step();
    };

    /// Loads a double-byte to the specified double-byte register.
    class LoadDoubleByte : public Operation {
    public:
        uint8_t& upper_dst;
        uint8_t& lower_dst;
        const uint8_t upper_src;
        const uint8_t lower_src;

        LoadDoubleByte(Cpu* const cpu, uint8_t& upper_dst, uint8_t& lower_dst, const uint8_t upper_src, const uint8_t lower_src);
        virtual bool Step();
    };

    class PopDoubleByte : public Operation {
    public:
        uint8_t& upper_dst;
        uint8_t& lower_dst;

        PopDoubleByte(Cpu* const cpu, uint8_t& upper_dst, uint8_t& lower_dst);
        virtual bool Step();
    };

    class PushDoubleByte : public Operation {
    public:
        const uint8_t upper_src;
        const uint8_t lower_src;

        PushDoubleByte(Cpu* const cpu, const uint8_t upper_src, const uint8_t lower_src);
        virtual bool Step();
    };

    class JumpRelative : public Operation {
    public:
        const uint8_t jump_offset;

        JumpRelative(Cpu* const cpu, const uint8_t jump_offset);
        virtual bool Step();
    };

    class JumpRelativeConditional : public Operation {
    public:
        const uint8_t jump_offset;
        const Flag flag;
        const bool flag_value;
        bool branch;

        JumpRelativeConditional(Cpu* const cpu, const uint8_t jump_offset, const Flag flag, const bool flag_value);
        virtual bool Step();
    };
}
