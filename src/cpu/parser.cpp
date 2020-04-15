#include "parser.hpp"
#include "cpu.hpp"

namespace GB_Cpu {
    Parser::Parser(Cpu* const cpu)
        : cpu(cpu), byte_chooser{&cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L, Helpers::DereferenceHL(cpu), &cpu->A} {}

    std::unique_ptr<Operations::Operation> Parser::Parse(const uint8_t opcode, uint8_t* const args) {
        uint8_t upper_hb = (opcode & 0xF0) >> 8;
        uint8_t lower_hb = opcode & 0x0F;
        if (upper_hb < 0x04) {
            switch (lower_hb % 8) {
                case 0x04:
                    return std::make_unique<Operations::IncreaseByte>(this->cpu, opcode, *this->byte_chooser[(opcode - 4) >> 3]);
                case 0x05:
                    return std::make_unique<Operations::DecreaseByte>(this->cpu, opcode, *this->byte_chooser[(opcode - 5) >> 3]);
            }
        }

        return nullptr;
    }
}
