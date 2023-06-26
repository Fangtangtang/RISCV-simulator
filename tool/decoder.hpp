#ifndef CODE_DECODER_HPP
#define CODE_DECODER_HPP

#include <iostream>
#include "../include/type.hpp"

enum InstructionType {
    /*
     * large constant number
     */
    LUI, // Load Upper Immediate
    AUIPC, // Add Upper Immediate

    /*
     * unconditional jump
     */
    JAL, // Jump and Link
    JALR, // Jump and Link Register

    /*
     * conditional branch
     */
    BEQ, // Branch Equal
    BNE, // Branch Not Equal
    BLT, // Branch Less Than
    BGE, // Branch Greater Than or Equal
    BLTU, // Branch Less Than Unsigned
    BGEU, // Branch Greater Than or Equal Unsigned

    /*
     * load
     */
    LB, // Load Byte
    LH, // Load Half Word
    LW, // Load Word
    LBU, // Load Byte Unsigned
    LHU, // Load Half Word Unsigned

    /*
     * store
     */
    SB, // Store Byte
    SH, // Store Half Word
    SW, // Store Word

    /*
     * calculate using immediate number
     */
    ADDI, // Add Immediate
    SLTI, // Set Less Than Immediate
    SLTIU, // Set Less Than Immediate Unsigned
    XORI, // Exclusive OR Immediate
    ORI, // OR Immediate
    ANDI, // AND Immediate
    SLLI, // Shift Left Immediate
    SRLI, // Shift Right Logical Immediate
    SRAI, // Shift Right Arithmetic Immediate

    /*
     * calculate
     */
    ADD, // Add
    SUB, // Subtract
    SLL, // Shift Left
    SLT, // Set Less Than
    SLTU, // Set Less Than Unsigned
    XOR, // Exclusive OR
    SRL, // Shift Right Logical
    SRA, // Shift Right Arithmetic
    OR, // OR
    AND, // AND

    /*
     * end of program
     */
    EXIT, // 0x0ff00513
    WAIT
};

/*
 * 指令信息
 */
struct Instruction {
    InstructionType instructionType = WAIT;
    Number immediate = 0;//immediate number
    Byte rs1{}, rs2{};//register source
    Byte rd{};//register destination
};

/*
 * 将机器码解码
 */
class Decoder {
public:
    static void Decode(const MachineCode &machineCode, Instruction &instruction);

public:
    //get opcode of instruction
    static Byte GetOpcode(const MachineCode &machineCode);

    static Byte Funct3(const MachineCode &machineCode);

    static TwoBytes Funct7(const MachineCode &machineCode);

    //根据Funct3判定的B-type
    static void DetermineBTypeFunct3(const MachineCode &machineCode, Instruction &instruction);

    //根据Funct3判定的I-type(load族)
    static void DetermineILTypeFunct3(const MachineCode &machineCode, Instruction &instruction);

    //根据Funct3判定的I-type(算数族)
    static bool DetermineIATypeFunct3(const MachineCode &machineCode, Instruction &instruction);

    //根据Funct3判定的S-type
    static void DetermineSTypeFunct3(const MachineCode &machineCode, Instruction &instruction);

    //根据Funct3和Funct7判定的I-type(imme族)
    static void DetermineIImmeType(const MachineCode &machineCode, Instruction &instruction);

    //根据Funct3和Funct7判定的R-type
    static void DetermineRType(const MachineCode &machineCode, Instruction &instruction);

    //get immediate number of instruction
    //extent sign bit if needed
    //5 types
    static Number ImmI(const MachineCode &machineCode);

    static Number ImmS(const MachineCode &machineCode);

    static Number ImmB(const MachineCode &machineCode);

    static Number ImmU(const MachineCode &machineCode);

    static Number ImmJ(const MachineCode &machineCode);

    static Number Shamt(const MachineCode &machineCode);

    //register
    static Byte RegisterSource1(const MachineCode &machineCode);

    static Byte RegisterSource2(const MachineCode &machineCode);

    static Byte RegisterDestination(const MachineCode &machineCode);

};

/*
 * Public
 * ----------------------------------------------------------------------------------
 */
void Decoder::Decode(const MachineCode &machineCode, Instruction &instruction) {
    if (machineCode == 0) {
        instruction.instructionType = WAIT;
        return;
    }
    Byte opcode = GetOpcode(machineCode);
    instruction.rs1 = instruction.rs2 = instruction.rd = 32;//not used
    switch (opcode) {
        case 0b0110111:
            instruction.instructionType = LUI;
            instruction.rd = RegisterDestination(machineCode);
            instruction.immediate = ImmU(machineCode);
            return;
        case 0b0010111:
            instruction.instructionType = AUIPC;
            instruction.rd = RegisterDestination(machineCode);
            instruction.immediate = ImmU(machineCode);
            return;
        case 0b1101111:
            instruction.instructionType = JAL;
            instruction.rd = RegisterDestination(machineCode);
            instruction.immediate = ImmJ(machineCode);
            return;
        case 0b1100111:
            instruction.instructionType = JALR;
            instruction.rd = RegisterDestination(machineCode);
            instruction.rs1 = RegisterSource1(machineCode);
            instruction.immediate = ImmI(machineCode);
            return;
        case 0b1100011:
            DetermineBTypeFunct3(machineCode, instruction);
            instruction.rs1 = RegisterSource1(machineCode);
            instruction.rs2 = RegisterSource2(machineCode);
            instruction.immediate = ImmB(machineCode);
            return;
        case 0b0000011:
            DetermineILTypeFunct3(machineCode, instruction);
            instruction.rd = RegisterDestination(machineCode);
            instruction.rs1 = RegisterSource1(machineCode);
            instruction.immediate = ImmI(machineCode);
            return;
        case 0b0100011:
            DetermineSTypeFunct3(machineCode, instruction);
            instruction.rs1 = RegisterSource1(machineCode);
            instruction.rs2 = RegisterSource2(machineCode);
            instruction.immediate = ImmS(machineCode);
            return;
        case 0b0010011:
            if (DetermineIATypeFunct3(machineCode, instruction)) {
                instruction.immediate = ImmI(machineCode);
            } else {
                DetermineIImmeType(machineCode, instruction);
                instruction.immediate = Shamt(machineCode);
            }
            instruction.rd = RegisterDestination(machineCode);
            instruction.rs1 = RegisterSource1(machineCode);
            if (instruction.instructionType == ADDI && instruction.rs1 == 0 && instruction.rd == 10 &&
                instruction.immediate == 255)
                instruction.instructionType = EXIT;
            return;
        case 0b0110011:
            DetermineRType(machineCode, instruction);
            instruction.rd = RegisterDestination(machineCode);
            instruction.rs1 = RegisterSource1(machineCode);
            instruction.rs2 = RegisterSource2(machineCode);
            return;
        default:
            instruction.instructionType = EXIT;
            std::cout << "UNEXPECTED!\n";
    }
}

/*
 * Private
 * ---------------------------------------------------------------------------------
 */
Byte Decoder::GetOpcode(const MachineCode &machineCode) {
    return Byte(machineCode & ((1 << 7) - 1));
}

Byte Decoder::Funct3(const MachineCode &machineCode) {
    return Byte((machineCode >> 12) & ((1 << 3) - 1));
}

TwoBytes Decoder::Funct7(const MachineCode &machineCode) {
    return Byte((machineCode >> 25) & ((1 << 7) - 1));
}

void Decoder::DetermineBTypeFunct3(const MachineCode &machineCode, Instruction &instruction) {
    Byte funct3 = Funct3(machineCode);
    switch (funct3) {
        case 0b000:
            instruction.instructionType = BEQ;
            break;
        case 0b001:
            instruction.instructionType = BNE;
            break;
        case 0b100:
            instruction.instructionType = BLT;
            break;
        case 0b101:
            instruction.instructionType = BGE;
            break;
        case 0b110:
            instruction.instructionType = BLTU;
            break;
        case 0b111:
            instruction.instructionType = BGEU;
            break;
        default:
            break;
    }
}

void Decoder::DetermineILTypeFunct3(const MachineCode &machineCode, Instruction &instruction) {
    Byte funct3 = Funct3(machineCode);
    switch (funct3) {
        case 0b000:
            instruction.instructionType = LB;
            break;
        case 0b001:
            instruction.instructionType = LH;
            break;
        case 0b010:
            instruction.instructionType = LW;
            break;
        case 0b100:
            instruction.instructionType = LBU;
            break;
        case 0b101:
            instruction.instructionType = LHU;
            break;
        default:
            break;
    }
}

bool Decoder::DetermineIATypeFunct3(const MachineCode &machineCode, Instruction &instruction) {
    Byte funct3 = Funct3(machineCode);
    switch (funct3) {
        case 0b000:
            instruction.instructionType = ADDI;
            return true;
        case 0b010:
            instruction.instructionType = SLTI;
            return true;
        case 0b011:
            instruction.instructionType = SLTIU;
            return true;
        case 0b100:
            instruction.instructionType = XORI;
            return true;
        case 0b110:
            instruction.instructionType = ORI;
            return true;
        case 0b111:
            instruction.instructionType = ANDI;
            return true;
        default:
            break;
    }
    return false;
}

void Decoder::DetermineSTypeFunct3(const MachineCode &machineCode, Instruction &instruction) {
    Byte funct3 = Funct3(machineCode);
    switch (funct3) {
        case 0b000:
            instruction.instructionType = SB;
            break;
        case 0b001:
            instruction.instructionType = SH;
            break;
        case 0b010:
            instruction.instructionType = SW;
            break;
        default:
            break;
    }
}

void Decoder::DetermineIImmeType(const MachineCode &machineCode, Instruction &instruction) {
    Byte funct3 = Funct3(machineCode);
    switch (funct3) {
        case 0b001:
            instruction.instructionType = SLLI;
            break;
        case 0b101: {
            TwoBytes funct7 = Funct7(machineCode);
            instruction.instructionType = (funct7 == 0b0) ? SRLI : SRAI;
        }
            break;
        default:
            break;
    }
}

void Decoder::DetermineRType(const MachineCode &machineCode, Instruction &instruction) {
    Byte funct3 = Funct3(machineCode);
    switch (funct3) {
        case 0b000: {
            TwoBytes funct7 = Funct7(machineCode);
            instruction.instructionType = (funct7 == 0b0) ? ADD : SUB;
        }
            break;
        case 0b001:
            instruction.instructionType = SLL;
            break;
        case 0b010:
            instruction.instructionType = SLT;
            break;
        case 0b011:
            instruction.instructionType = SLTU;
            break;
        case 0b100:
            instruction.instructionType = XOR;
            break;
        case 0b101: {
            TwoBytes funct7 = Funct7(machineCode);
            instruction.instructionType = (funct7 == 0b0) ? SRL : SRA;
        }
            break;
        case 0b110:
            instruction.instructionType = OR;
            break;
        case 0b111:
            instruction.instructionType = AND;
            break;
        default:
            break;
    }
}

Number Decoder::ImmI(const MachineCode &machineCode) {
    Number tmp((machineCode >> 20) & ((1 << 12) - 1));
    return SignExtend(tmp, 12);
}

Number Decoder::ImmS(const MachineCode &machineCode) {
    Number tmp = (machineCode >> 25) & ((1 << 7) - 1);
    tmp <<= 5;
    tmp |= ((machineCode >> 7) & ((1 << 5) - 1));
    return SignExtend(tmp, 12);
}

Number Decoder::ImmB(const MachineCode &machineCode) {
    Number tmp = (machineCode >> 31) & 1;
    tmp <<= 1;
    tmp |= ((machineCode >> 7) & 1);
    tmp <<= 6;
    tmp |= ((machineCode >> 25) & ((1 << 6) - 1));
    tmp <<= 4;
    tmp |= ((machineCode >> 8) & ((1 << 4) - 1));
    tmp <<= 1;
    return SignExtend(tmp, 13);
}

Number Decoder::ImmU(const MachineCode &machineCode) {
    Number tmp(((UnsignedNumber) machineCode >> 12) & ((1 << 20) - 1));
    tmp <<= 12;
    return tmp;
}

Number Decoder::ImmJ(const MachineCode &machineCode) {
    Number tmp = (machineCode >> 31) & 1;
    tmp <<= 8;
    tmp |= ((machineCode >> 12) & ((1 << 8) - 1));
    tmp <<= 1;
    tmp |= ((machineCode >> 20) & 1);
    tmp <<= 10;
    tmp |= ((machineCode >> 21) & ((1 << 10) - 1));
    tmp <<= 1;
    return SignExtend(tmp, 21);
}

Number Decoder::Shamt(const MachineCode &machineCode) {
    return Number((machineCode >> 20) & ((1 << 5) - 1));
}

Byte Decoder::RegisterSource1(const MachineCode &machineCode) {
    return Byte((machineCode >> 15) & ((1 << 5) - 1));
}

Byte Decoder::RegisterSource2(const MachineCode &machineCode) {
    return Byte((machineCode >> 20) & ((1 << 5) - 1));
}

Byte Decoder::RegisterDestination(const MachineCode &machineCode) {
    return Byte((machineCode >> 7) & ((1 << 5) - 1));
}

std::string Convert(const InstructionType &instructionType) {
    switch (instructionType) {
        case LUI:
            return "LUI";
        case AUIPC:
            return "AUIPC";
        case JAL:
            return "JAL";
        case JALR:
            return "JALR";
        case BEQ:
            return "BEQ";
        case BNE:
            return "BNE";
        case BLT:
            return "BLT";
        case BGE:
            return "BGE";
        case BLTU:
            return "BLTU";
        case BGEU:
            return "BGEU";
        case LB:
            return "LB";
        case LH:
            return "LH";
        case LW:
            return "LW";
        case LBU:
            return "LBU";
        case LHU:
            return "LHU";
        case SB:
            return "SB";
        case SH:
            return "SH";
        case SW:
            return "SW";
        case ADDI:
            return "ADDI";
        case SLTI:
            return "SLTI";
        case SLTIU:
            return "SLTIU";
        case XORI:
            return "XORI";
        case ORI:
            return "ORI";
        case ANDI:
            return "ANDI";
        case SLLI:
            return "SLLI";
        case SRLI:
            return "SRLI";
        case SRAI:
            return "SRAI";
        case ADD:
            return "ADD";
        case SUB:
            return "SUB";
        case SLL:
            return "SLL";
        case SLT:
            return "SLT";
        case SLTU:
            return "SLTU";
        case XOR:
            return "XOR";
        case SRL:
            return "SRL";
        case SRA:
            return "SRA";
        case OR:
            return "OR";
        case AND:
            return "AND";
        case EXIT:
            return "EXIT";
    }
    return "ERROR";
}

#endif //CODE_DECODER_HPP
