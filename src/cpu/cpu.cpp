#include "cpu.hpp"

namespace GB_Cpu {
    Cpu::Cpu(uint8_t** const memory) : memory(memory) {
        this->A = 0;
        this->F = 0;
        this->B = 0;
        this->C = 0;
        this->D = 0;
        this->E = 0;
        this->H = 0;
        this->L = 0;
        this->SP = 0;
        this->PC = 0;

        this->parser = std::make_unique<Parser>(this);
    }

    void Cpu::SetFlag(Flag flag, bool flag_value) {
        if (flag_value)
            this->F |= (uint8_t) flag;
        else
            this->F &= ((uint8_t) flag ^ 0xFF);
    }

    void Cpu::Cycle() {
        uint8_t opcode = (*this->memory)[this->PC++];
        uint8_t args_n = Helpers::GetArgsNumber(opcode);

        uint8_t args [args_n];
        for (uint8_t i = 0; i < args_n; i++)
            args[i] = (*this->memory)[this->PC + i + 1];

        std::unique_ptr<Operations::Operation> operation = this->parser->Parse(opcode, args);
        operation->Execute();
    }
}

int main() {
    uint8_t* memory = new uint8_t [4096];
    memory[0] = 0x05;

    GB_Cpu::Cpu cpu = GB_Cpu::Cpu(&memory);
    printf("%02x\n", cpu.B);
    cpu.Cycle();
    printf("%02x\n", cpu.B);
    printf("%02x\n", cpu.F);
    delete [] memory;
    return 0;
}
