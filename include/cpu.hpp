#pragma once
#include <cstdint>

namespace GBEMU {

    enum class Flag : uint8_t {
        zf = 0x01,
        n  = 0x02,
        h  = 0x04,
        cy = 0x08
    };

    class Cpu {
    private:
        uint8_t A, F;
        uint8_t B, C;
        uint8_t D, E;
        uint8_t H, L;
        uint16_t SP;
        uint16_t PC;
        uint8_t** memory;
    public:
        uint16_t joinBytes(uint8_t r1, uint8_t r2);

        uint16_t readBC();
        uint16_t readDE();
        uint16_t readHL();

        bool getFlag(Flag flag);
        void setFlag(Flag flag, bool value);
        void setFlags(uint8_t flags);
        void resetFlags(uint8_t flags);
        void adx_8b_SetFlags(uint8_t r, uint16_t real);
        void sub_8b_SetFlags(uint8_t r, int real);

        void inc_8b(uint8_t& r);
        void dec_8b(uint8_t& r);

        void rl_8b(uint8_t& r);
        void rr_8b(uint8_t& r);
        void rlc_8b(uint8_t& r);
        void rrc_8b(uint8_t& r);

        void add_8b(uint8_t& r1, uint8_t r2);
        void adc_8b(uint8_t& r1, uint8_t r2);
        void sub_8b(uint8_t& r1, uint8_t r2);
        void sbc_8b(uint8_t& r1, uint8_t r2);
        void and_8b(uint8_t& r1, uint8_t r2);
        void xor_8b(uint8_t& r1, uint8_t r2);
        void or_8b(uint8_t& r1, uint8_t r2);
        void cp_8b(uint8_t r1, uint8_t r2);

        void inc_16b2(uint8_t& r1, uint8_t& r2);
        void dec_16b2(uint8_t& r1, uint8_t& r2);

        void mov_16bmem(uint16_t addr, uint16_t value);
        void add_16b2x2(uint8_t& r1, uint8_t& r2, uint8_t r3, uint8_t r4);

        uint8_t jr_conditional(Flag flag, bool value, int8_t jvalue);

        Cpu(uint8_t*& memory);
        ~Cpu();
        uint8_t executeInstruction(bool cb, uint8_t opcode, uint8_t* args);
    };
}
