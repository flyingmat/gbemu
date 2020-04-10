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

    ushort Cpu::getDE() {
        return this->joinBytes(this->D, this->E);
    }

    ushort Cpu::getHL() {
        return this->joinBytes(this->H, this->L);
    }

    bool Cpu::getFlag(Flag flag) {
        return (this->F & (uchar) flag) != 0;
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

    void Cpu::rl8bRegister(uchar& r) {
        bool msb = r >> 7;
        r = (r << 1) | this->getFlag(Flag::cy);
        this->setFlag(Flag::cy, msb);
        this->resetFlags((uchar) Flag::zf | (uchar) Flag::n | (uchar) Flag::h);
    }

    void Cpu::rr8bRegister(uchar& r) {
        bool lsb = r & 0x01;
        r = (r >> 1) | (((uchar) this->getFlag(Flag::cy)) << 7);
        this->setFlag(Flag::cy, lsb);
        this->resetFlags((uchar) Flag::zf | (uchar) Flag::n | (uchar) Flag::h);
    }

    void Cpu::rlc8bRegister(uchar& r) {
        this->setFlag(Flag::cy, r >> 7);
        r = (r << 1) | (r >> 7);
        this->resetFlags((uchar) Flag::zf | (uchar) Flag::n | (uchar) Flag::h);
    }

    void Cpu::rrc8bRegister(uchar& r) {
        this->setFlag(Flag::cy, r & 0x01);
        r = (r >> 1) | ((r & 0x01) << 7);
        this->resetFlags((uchar) Flag::zf | (uchar) Flag::n | (uchar) Flag::h);
    }

    void Cpu::inc16bRegister(uchar& r1, uchar& r2) {
        r2++;
        if (!r2)
            r1++;
    }

    void Cpu::dec16bRegister(uchar& r1, uchar& r2) {
        r2--;
        if (r2 == 0xFF)
            r1--;
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
            // LD A,(BC)
            case 0x0A:
                this->A = (*this->memory)[this->getBC()];
                return 8;
            // DEC BC
            case 0x0B:
                this->dec16bRegister(this->B, this->C);
                return 8;
            // INC C
            case 0x0C:
                this->inc8bRegister(this->C);
                return 4;
            // DEC C
            case 0x0D:
                this->dec8bRegister(this->C);
                return 4;
            // LD C,u8
            case 0x0E:
                this->C = args[0];
                return 8;
            // RRCA
            case 0x0F:
                this->rrc8bRegister(this->A);
                return 4;
            // STOP
            case 0x10:
                // to be implemented
                return 4;
            // LD DE,u16
            case 0x11:
                this->D = args[0];
                this->E = args[1];
                return 12;
            // LD (DE),A
            case 0x12:
                (*this->memory)[this->getDE()] = this->A;
                return 8;
            // INC DE
            case 0x13:
                this->inc16bRegister(this->D, this->E);
                return 8;
            // INC D
            case 0x14:
                this->inc8bRegister(this->D);
                return 4;
            // DEC D
            case 0x15:
                this->dec8bRegister(this->D);
                return 4;
            // LD D,u8
            case 0x16:
                this->D = args[0];
                return 8;
            // RLA
            case 0x17:
                this->rl8bRegister(this->A);
                return 4;
            // JR i8
            case 0x18:
                this->PC += (char) args[0];  // should work to get signed char back
                return 12;
            // ADD HL,DE
            case 0x19:
                this->add16bRegisters(this->H, this->L, this->D, this->E);
                return 8;
            // LD A,(DE)
            case 0x1A:
                this->A = (*this->memory)[this->getDE()];
                return 8;
            // DEC DE
            case 0x1B:
                this->dec16bRegister(this->D, this->E);
                return 8;
            // INC E
            case 0x1C:
                this->inc8bRegister(this->E);
                return 4;
            // DEC E
            case 0x1D:
                this->dec8bRegister(this->E);
                return 4;
            // LD E,u8
            case 0x1E:
                this->E = args[0];
                return 8;
            // RRA
            case 0x1F:
                this->rr8bRegister(this->A);
                return 4;
            // JR NZ,i8
            case 0x20:
                if (!this->getFlag(Flag::zf)) {
                    this->PC += (char) args[0];
                    return 12;
                }
                return 8;
            // LD HL,u16
            case 0x21:
                this->H = args[0];
                this->L = args[1];
                return 12;
            // LD (HL+),A
            case 0x22:
                (*this->memory)[this->getHL()] = this->A;
                this->inc16bRegister(this->H, this->L);
                return 8;
            // INC HL
            case 0x23:
                this->inc16bRegister(this->H, this->L);
                return 8;
            // INC H
            case 0x24:
                this->inc8bRegister(this->H);
                return 4;
            // DEC H
            case 0x25:
                this->dec8bRegister(this->H);
                return 4;
            // LD H,u8
            case 0x26:
                this->H = args[0];
                return 8;
            // // DAA
            // case 0x27:
            // // JR Z,i8
            // case 0x28:
            }
        }

        return 0;
    }
}
