#pragma once
#include <cstdint>
#include <memory>

namespace Cpu {
    class Cpu;
}

namespace Cpu::Operations {
    class Operation {
    public:
        Cpu* const cpu;
        uint8_t opcode, step_i;

        Operation(Cpu* const cpu);
        virtual bool Step() = 0;
    };

    class Instruction {
    public:
        std::shared_ptr<Operation> operation;
        const uint8_t opcode;
        const uint8_t extra_steps;
        uint8_t extra_step_i;

        Instruction(std::shared_ptr<Operation> operation, const uint8_t opcode, const uint8_t extra_steps);
        bool Step();
        void Execute();
    };

    class IncreaseByte : public Operation {
    private:
        void SetFlags();
    public:
        uint8_t& byte;

        IncreaseByte(Cpu* const cpu, uint8_t& byte);
        virtual bool Step();
    };

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

    class LoadByte : public Operation {
    public:
        uint8_t& dst;
        const uint8_t src;

        LoadByte(Cpu* const cpu, uint8_t& dst, const uint8_t src);
        virtual bool Step();
    };
}
