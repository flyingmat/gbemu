#include "cpu.hpp"

namespace Cpu {
    Cpu::Cpu(uint8_t** const memory) : memory(memory) {
        this->A = 0;
        this->F = 0;
        this->B = 0;
        this->C = 0;
        this->D = 0;
        this->E = 0;
        this->H = 0;
        this->L = 0;
        this->S = 0;
        this->P = 0;
        this->PC = 0;

        this->parser = std::make_unique<Parser>(this);
    }

    bool Cpu::GetFlag(const Flag flag) {
        return (this->F & (uint8_t) flag) != 0;
    }

    void Cpu::SetFlag(const Flag flag, const bool flag_value) {
        if (flag_value)
            this->F |= (uint8_t) flag;
        else
            this->F &= ((uint8_t) flag ^ 0xFF);
    }

    std::shared_ptr<Operations::Instruction> Cpu::Cycle() {
        uint8_t opcode = (*this->memory)[this->PC++];
        uint8_t args_n = Helpers::GetArgsNumber(opcode);

        uint8_t args [args_n];
        for (uint8_t i = 0; i < args_n; i++)
            args[i] = (*this->memory)[this->PC++];

        std::shared_ptr<Operations::Instruction> instruction = this->parser->Parse(opcode, args);
        if (instruction != nullptr)
            instruction->Execute();
        return instruction;
    }
}
