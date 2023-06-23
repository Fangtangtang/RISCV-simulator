#ifndef CODE_ALU_HPP
#define CODE_ALU_HPP

#include "decoder.hpp"

class ALU {
public:
    //do arithmatic work
    //return true if execute
    static bool Execute(Instruction &instruction, const Number &nrs1, const Number &nrs2, Number &result);

};

bool ALU::Execute(Instruction &instruction, const Number &nrs1, const Number &nrs2, Number &result) {
    switch (instruction.instructionType) {
        case BEQ:
            if (nrs1 == nrs2)result = 1;
            else result = 0;
            return true;
        case BNE:
            if (nrs1 != nrs2)result = 1;
            else result = 0;
            return true;
        case BLT:
        case SLT:
            result = nrs1 < nrs2 ? 1 : 0;
            return true;
        case BGE:
            if (nrs1 >= nrs2)result = 1;
            else result = 0;
            return true;
        case BLTU:
        case SLTU:
            if ((UnsignedNumber) nrs1 < (UnsignedNumber) nrs2)result = 1;
            else result = 0;
            return true;
        case BGEU:
            result = ((UnsignedNumber) nrs1 >= (UnsignedNumber) nrs2) ? 1 : 0;
            return true;
        case ADDI:
            if (instruction.rs1 == 0 && instruction.rd == 10 && instruction.immediate == 255) {
                instruction.instructionType = EXIT;
            } else result = nrs1 + instruction.immediate;
            return true;
        case SLTI:
            result = nrs1 < instruction.immediate ? 1 : 0;
            return true;
        case SLTIU:
            result = ((UnsignedNumber) nrs1 < (UnsignedNumber) instruction.immediate) ? 1 : 0;
            return true;
        case XORI:
            result = nrs1 ^ instruction.immediate;
            return true;
        case ORI:
            result = nrs1 | instruction.immediate;
            return true;
        case ANDI:
            result = nrs1 & instruction.immediate;
            return true;
        case SLLI:
            result = ((UnsignedNumber) nrs1 << instruction.immediate);
            return true;
        case SRLI:
            result = ((UnsignedNumber) nrs1 >> instruction.immediate);
            return true;
        case SRAI:
            result = nrs1 >> instruction.immediate;
            return true;
        case ADD:
            result = nrs1 + nrs2;
            return true;
        case SUB:
            result = nrs1 - nrs2;
            return true;
        case SLL:
            result = nrs1 << nrs2;
            return true;
        case XOR:
            result = nrs1 ^ nrs2;
            return true;
        case SRL:
            result =(UnsignedNumber) nrs1 >> (nrs2& ((1 << 5) - 1));
            return true;
        case SRA:
            result = nrs1 >> (nrs2& ((1 << 5) - 1));
            return true;
        case OR:
            result = nrs1 | nrs2;
            return true;
        case AND:
            result = nrs1 & nrs2;
            return true;
        default:
            return false;//not execute in ALU
    }
}

#endif //CODE_ALU_HPP
