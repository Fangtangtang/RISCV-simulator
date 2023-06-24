#ifndef CODE_ALU_HPP
#define CODE_ALU_HPP

#include "decoder.hpp"

class ALU {
public:

    static Number Execute(const InstructionType &type, const Number &nrs1, const Number &nrs2, Number &imme);

};

Number ALU::Execute(const InstructionType &type, const Number &nrs1, const Number &nrs2, Number &imme) {
    switch (type) {
        case BEQ:
            if (nrs1 == nrs2)return 1;
            else return 0;
        case BNE:
            if (nrs1 != nrs2)return 1;
            else return 0;
        case BLT:
        case SLT:
            return nrs1 < nrs2 ? 1 : 0;
        case BGE:
            if (nrs1 >= nrs2)return 1;
            else return 0;
        case BLTU:
        case SLTU:
            if ((UnsignedNumber) nrs1 < (UnsignedNumber) nrs2)return 1;
            else return 0;
        case BGEU:
            return ((UnsignedNumber) nrs1 >= (UnsignedNumber) nrs2) ? 1 : 0;
        case ADDI:
            return nrs1 + imme;
        case SLTI:
            return nrs1 < imme ? 1 : 0;
        case SLTIU:
            return ((UnsignedNumber) nrs1 < (UnsignedNumber) imme) ? 1 : 0;
        case XORI:
            return nrs1 ^ imme;
        case ORI:
            return nrs1 | imme;
        case ANDI:
            return nrs1 & imme;
        case SLLI:
            return ((UnsignedNumber) nrs1 << imme);
        case SRLI:
            return ((UnsignedNumber) nrs1 >> imme);
        case SRAI:
            return nrs1 >> imme;
        case ADD:
            return nrs1 + nrs2;
        case SUB:
            return nrs1 - nrs2;
        case SLL:
            return nrs1 << nrs2;
        case XOR:
            return nrs1 ^ nrs2;
        case SRL:
            return (UnsignedNumber) nrs1 >> (nrs2 & ((1 << 5) - 1));
        case SRA:
            return nrs1 >> (nrs2 & ((1 << 5) - 1));
        case OR:
            return nrs1 | nrs2;
        case AND:
            return nrs1 & nrs2;
        default:
            return 0;//not execute in ALU
    }
}

#endif //CODE_ALU_HPP
