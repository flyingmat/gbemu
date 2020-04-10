namespace GBEMU {

    typedef unsigned char uchar;
    typedef unsigned short int ushort;

    enum class Flag : uchar {
        zf = 0x01,
        n  = 0x02,
        h  = 0x04,
        cy = 0x08
    };

    class Cpu {
    private:
        uchar A;
        uchar F;
        uchar B;
        uchar C;
        uchar D;
        uchar E;
        uchar H;
        uchar L;
        ushort SP;
        ushort PC;
        uchar** memory;

        ushort joinBytes(uchar r1, uchar r2);
        ushort getBC();
        ushort getDE();
        bool getFlag(Flag flag);
        void setFlag(Flag flag, bool value);
        void resetFlags(uchar flags);
        void inc8bSetFlags(uchar result);
        void dec8bSetFlags(uchar result);
        void add16bSetFlags(ushort result);
        void inc8bRegister(uchar& r);
        void dec8bRegister(uchar& r);
        void rl8bRegister(uchar& r);
        void rr8bRegister(uchar& r);
        void rlc8bRegister(uchar& r);
        void rrc8bRegister(uchar& r);
        void inc16bRegister(uchar& r1, uchar& r2);
        void dec16bRegister(uchar& r1, uchar& r2);
        void mov16bRegister(ushort addr, ushort value);
        void add16bRegisters(uchar& r1, uchar& r2, uchar& r3, uchar& r4);

    public:
        Cpu(uchar*& memory);
        ~Cpu();
        uchar executeInstruction(bool cb, uchar opcode, uchar* args);
    };
}
