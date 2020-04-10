#include "cpu.hpp"

namespace GBEMU {

    Cpu::Cpu(unsigned char*& memory) {
        this->AF = 0;
        this->BC = 0;
        this->DE = 0;
        this->HL = 0;
        this->SP = 0;
        this->PC = 0;
        this->memory = &memory;
    }

    Cpu::~Cpu() {
        this->memory = nullptr;
    }

    void Cpu::setFlag(Flag flag, bool value) {
        if (value)
            this->AF |= (unsigned char) flag;
        else
            this->AF &= ((unsigned char) flag ^ 0xFF);
    }

    void Cpu::incSetZFH(unsigned char result) {
        this->setFlag(Flag::h, result == 0x10);
        this->setFlag(Flag::zf, result == 0x00);
    }

    void Cpu::decSetZFH(unsigned char result) {
        this->setFlag(Flag::h, result == 0x0F);
        this->setFlag(Flag::zf, result == 0xFF);
    }

    void Cpu::incHalfRegister(unsigned short int& r, Side side) {
        if (side == Side::right) {
            r = (r & 0xFF00) | (((r & 0x00FF) + 1) & 0x00FF);
            this->incSetZFH(r & 0x00FF);
        }
        else {
            r = (r & 0x00FF) | (((r & 0xFF00) + 0x0100) & 0xFF00);
            this->incSetZFH(r >> 8);
        }
    }

    void Cpu::decHalfRegister(unsigned short int& r, Side side) {
        if (side == Side::right) {
            r = (r & 0xFF00) | (((r & 0x00FF) - 1) & 0x00FF);
            this->decSetZFH(r & 0x00FF);
        }
        else {
            r = (r & 0x00FF) | (((r & 0xFF00) - 0x0100) & 0xFF00);
            this->decSetZFH(r >> 8);
        }
    }

    unsigned char Cpu::executeInstruction(bool cb, unsigned char opcode, unsigned char* args) {
        if (!cb) {
            switch (opcode) {
            // NOP
            case 0x00:
                return 4;
            // LD BC,u16
            case 0x01:
                this->BC = (args[0] << 8) | args[1];
                return 12;
            // LD (BC),A
            case 0x02:
                (*this->memory)[this->BC] = (this->AF >> 8);
                return 8;
            // INC BC
            case 0x03:
                this->BC++;
                return 8;
            // INC B
            case 0x04:
                this->incHalfRegister(this->BC, Side::left);
                return 4;
            case 0x05:
                this->decHalfRegister(this->BC, Side::left);
                return 4;
            }
        }

        return 0;
    }
}
