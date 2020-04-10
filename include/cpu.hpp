namespace GBEMU {

    enum Side : bool {
        right,
        left
    };

    enum class Flag : unsigned char {
        zf = 0x01,
        n  = 0x02,
        h  = 0x04,
        cy = 0x08
    };

    class Cpu {

    private:
        unsigned short int AF;
        unsigned short int BC;
        unsigned short int DE;
        unsigned short int HL;
        unsigned short int SP;
        unsigned short int PC;
        unsigned char** memory;

        void setFlag(Flag flag, bool value);
        void incSetZFH(unsigned char result);
        void decSetZFH(unsigned char result);
        void incHalfRegister(unsigned short int& r, Side side);
        void decHalfRegister(unsigned short int& r, Side side);

    public:
        Cpu(unsigned char*& memory);
        ~Cpu();
        unsigned char executeInstruction(bool cb, unsigned char opcode, unsigned char* args);
    };
}
