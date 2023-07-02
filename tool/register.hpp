#ifndef CODE_REGISTER_HPP
#define CODE_REGISTER_HPP

#include "../include/type.hpp"
#include "decoder.hpp"

class Registers;

class Memory;

class RegisterFile;

class Registers {
    RegisterUnit aRegister[32] = {0};
    RegisterUnit PC = 0;//program counter

    friend class Memory;

    friend class RegisterFile;

public:

    inline void Update(const Byte &ind, const Number &value);

    //有哪几个要更新
    void ReadRegister(const Instruction &instruction, Number &nrs1, Number &nrs2);

    //return value
    Number ReadRegister();

    bool WriteBack(const Instruction &instruction, const Number &nrs1, const Number &result);

    void Print();
};

void Registers::Update(const Byte &ind, const Number &value) {
    if (ind && ind != 32) aRegister[ind] = value;
}

void Registers::ReadRegister(const Instruction &instruction, Number &nrs1, Number &nrs2) {
    if (instruction.rs1 < 32) nrs1 = aRegister[instruction.rs1];
    if (instruction.rs2 < 32) nrs2 = aRegister[instruction.rs2];
}

bool Registers::WriteBack(const Instruction &instruction, const Number &nrs1, const Number &result) {
    switch (instruction.instructionType) {
        case LUI:
            aRegister[instruction.rd] = instruction.immediate;
            return true;
        case AUIPC:
            aRegister[instruction.rd] = PC + instruction.immediate;
            return true;
        case JAL:
            aRegister[instruction.rd] = PC;
            PC = instruction.immediate + PC;
            return true;
        case JALR:
            aRegister[instruction.rd] = PC;
            PC = instruction.immediate + nrs1;
            if (PC & 1) PC &= (PC - 1);
            return true;
        case BEQ:
        case BNE:
        case BLT:
        case BGE:
        case BLTU:
        case BGEU:
            if (result == 1) PC = PC + instruction.immediate;
            return true;
        default:
            if (instruction.rd < 32) {
                aRegister[instruction.rd] = result;
                return true;
            }
            return false;
    }
}

Number Registers::ReadRegister() {
    return aRegister[10];
}

void Registers::Print() {
    std::cout << "\nPrintReg\n";
    for (int i = 0; i < 32; ++i) {
        std::cout << aRegister[i] << ' ';
    }
    std::cout<<'\n';
}

#endif //CODE_REGISTER_HPP
