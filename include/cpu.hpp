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
        uint16_t getBC();
        uint16_t getDE();
        uint16_t getHL();
        bool getFlag(Flag flag);
        void setFlag(Flag flag, bool value);
        void resetFlags(uint8_t flags);
        void inc8bRegister(uint8_t& r);
        void dec8bRegister(uint8_t& r);
        void rl8bRegister(uint8_t& r);
        void rr8bRegister(uint8_t& r);
        void rlc8bRegister(uint8_t& r);
        void rrc8bRegister(uint8_t& r);
        void inc16bRegister(uint8_t& r1, uint8_t& r2);
        void dec16bRegister(uint8_t& r1, uint8_t& r2);
        void mov16bRegister(uint16_t addr, uint16_t value);
        void add16bRegisters(uint8_t& r1, uint8_t& r2, uint8_t& r3, uint8_t& r4);

        Cpu(uint8_t*& memory);
        ~Cpu();
        uint8_t executeInstruction(bool cb, uint8_t opcode, uint8_t* args);
    };
}
