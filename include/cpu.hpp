namespace GBEMU {

    typedef unsigned char uchar;
    typedef unsigned short int ushort;

    enum class Flag : unsigned char {
        zf = 0x01,
        n  = 0x02,
        h  = 0x04,
        cy = 0x08
    };

    class Cpu {

    private:
        unsigned char A;
        unsigned char F;
        unsigned char B;
        unsigned char C;
        unsigned char D;
        unsigned char E;
        unsigned char H;
        unsigned char L;
        unsigned short int SP;
        unsigned short int PC;
        unsigned char** memory;

        unsigned short int joinBytes(unsigned char r1, unsigned char r2);
        unsigned short int getBC();
        void setFlag(Flag flag, bool value);
        void resetFlags(unsigned char flags);
        void inc8bSetFlags(unsigned char result);
        void dec8bSetFlags(unsigned char result);
        void add16bSetFlags(unsigned short int result);
        void inc8bRegister(unsigned char& r);
        void dec8bRegister(unsigned char& r);
        void rlc8bRegister(unsigned char& r);
        void inc16bRegister(unsigned char& r1, unsigned char& r2);
        void mov16bRegister(unsigned short int addr, unsigned short int value);
        void add16bRegisters(unsigned char& r1, unsigned char& r2, unsigned char& r3, unsigned char& r4);

    public:
        Cpu(unsigned char*& memory);
        ~Cpu();
        unsigned char executeInstruction(bool cb, unsigned char opcode, unsigned char* args);
    };
}
