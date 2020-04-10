#include "cpu.hpp"

namespace GBEMU {

    Cpu::Cpu(uchar*& memory) {
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
        this->memory = &memory;
    }

    Cpu::~Cpu() {
        this->memory = nullptr;
    }

    ushort Cpu::joinBytes(uchar r1, uchar r2) {
        ushort out = r2;
        return out | ((ushort) r1 << 8);
    }

    ushort Cpu::getBC() {
        return this->joinBytes(this->B, this->C);
    }

    void Cpu::setFlag(Flag flag, bool value) {
        if (value)
            this->F |= (uchar) flag;
        else
            this->F &= ((uchar) flag ^ 0xFF);
    }

    void Cpu::resetFlags(uchar flags) {
        this->F &= (flags ^ 0xFF);
    }

    void Cpu::inc8bSetFlags(uchar result) {
        this->setFlag(Flag::zf, result == 0x00);
        this->setFlag(Flag::n, 0);
        this->setFlag(Flag::h, result == 0x10);
    }

    void Cpu::dec8bSetFlags(uchar result) {
        this->setFlag(Flag::zf, result == 0xFF);
        this->setFlag(Flag::n, 1);
        this->setFlag(Flag::h, result == 0x0F);
    }

    void Cpu::add16bSetFlags(ushort result) {
        this->setFlag(Flag::n, 0);
        this->setFlag(Flag::h, result == 0x0100);
        this->setFlag(Flag::cy, result == 0x0000);
    }

    void Cpu::inc8bRegister(uchar& r) {
        r++;
        this->inc8bSetFlags(r);
    }

    void Cpu::dec8bRegister(uchar& r) {
        r--;
        this->dec8bSetFlags(r);
    }

    void Cpu::rlc8bRegister(uchar& r) {
        this->setFlag(Flag::cy, r >> 7);
        this->resetFlags((uchar) Flag::zf | (uchar) Flag::n | (uchar) Flag::h);
    }

    void Cpu::inc16bRegister(uchar& r1, uchar& r2) {
        r2++;
        if (!r2)
            r1++;
    }

    void Cpu::mov16bRegister(ushort addr, ushort value) {
        (*this->memory)[addr] = value >> 8;
        (*this->memory)[addr+1] = value & 0x00FF;
    }

    void Cpu::add16bRegisters(uchar& r1, uchar& r2, uchar& r3, uchar& r4) {
        ushort result = this->joinBytes(r1, r2) + this->joinBytes(r3, r4);
        r1 = result >> 8;
        r2 = result & 0x00FF;
        this->add16bSetFlags(result);
    }

    uchar Cpu::executeInstruction(bool cb, uchar opcode, uchar* args) {
        if (!cb) {
            switch (opcode) {
            // NOP
            case 0x00:
                return 4;
            // LD BC,u16
            case 0x01:
                this->B = args[0];
                this->C = args[1];
                return 12;
            // LD (BC),A
            case 0x02:
                (*this->memory)[this->getBC()] = this->A;
                return 8;
            // INC BC
            case 0x03:
                this->inc16bRegister(this->B, this->C);
                return 8;
            // INC B
            case 0x04:
                this->inc8bRegister(this->B);
                return 4;
            // DEC B
            case 0x05:
                this->dec8bRegister(this->B);
                return 4;
            // LD B,u8
            case 0x06:
                this->B = args[0];
                return 8;
            // RLCA
            case 0x07:
                this->rlc8bRegister(this->A);
                return 4;
            // LD (u16),SP
            case 0x08:
                this->mov16bRegister(this->joinBytes(args[0], args[1]), this->SP);
                return 20;
            // ADD HL,BC
            case 0x09:
                this->add16bRegisters(this->H, this->L, this->B, this->C);
                return 8;
            }
        }

        return 0;
    }
}
