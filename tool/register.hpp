#ifndef CODE_REGISTER_HPP
#define CODE_REGISTER_HPP

#include "../include/type.hpp"
#include "decoder.hpp"

class Registers;

class Memory;

class Register {
    RegisterUnit pre_state = 0;
    RegisterUnit after_state = 0;

    friend class Registers;

    friend class Memory;
};

class Registers {
    Register aRegister[32];
    Register PC;//program counter

    friend class Memory;

public:

    void UpdateRegister();

    void UpdatePC();

    void GetPC(RegisterUnit &inf) const;

    //有哪几个要更新
    void ReadRegister(const Instruction &instruction, Number &nrs1, Number &nrs2);

    //return value
    Number ReadRegister();

    bool WriteBack(const Instruction &instruction, const Number &nrs1, const Number &result);

    void Print();
};

void Registers::UpdateRegister() {
    for (auto &i: aRegister) {
        i.pre_state = i.after_state;
    }
    PC.pre_state = PC.after_state;
    aRegister[0].pre_state = aRegister[0].after_state = 0;
}

void Registers::UpdatePC() {
    PC.after_state = PC.pre_state + 4;
}

void Registers::GetPC(RegisterUnit &inf) const {
    inf = PC.pre_state;
}

void Registers::ReadRegister(const Instruction &instruction, Number &nrs1, Number &nrs2) {
    if (instruction.rs1 < 32) nrs1 = aRegister[instruction.rs1].pre_state;
    if (instruction.rs2 < 32) nrs2 = aRegister[instruction.rs2].pre_state;
}

bool Registers::WriteBack(const Instruction &instruction, const Number &nrs1, const Number &result) {
    switch (instruction.instructionType) {
        case LUI:
            aRegister[instruction.rd].after_state = instruction.immediate;
            return true;
        case AUIPC:
            aRegister[instruction.rd].after_state = PC.pre_state + instruction.immediate;
            return true;
        case JAL:
            aRegister[instruction.rd].after_state = PC.after_state;
            PC.after_state = instruction.immediate + PC.pre_state;
            return true;
        case JALR:
            aRegister[instruction.rd].after_state = PC.after_state;
            PC.after_state = instruction.immediate + nrs1;
            if (PC.after_state & 1) PC.after_state &= (PC.after_state - 1);
            return true;
        case BEQ:
        case BNE:
        case BLT:
        case BGE:
        case BLTU:
        case BGEU:
            if (result == 1) PC.after_state = PC.pre_state + instruction.immediate;
            return true;
        default:
            if (instruction.rd < 32) {
                aRegister[instruction.rd].after_state = result;
                return true;
            }
            return false;
    }
}

Number Registers::ReadRegister() {
    return aRegister[10].pre_state;
}

void Registers::Print() {
    std::cout << "\nPrintReg\n";
    for (int i = 0; i < 32; ++i) {
        std::cout << i << '\t' << aRegister[i].pre_state << '\n';
    }
    std::cout << "PC\t" << PC.pre_state << '\n';
}


#endif //CODE_REGISTER_HPP
