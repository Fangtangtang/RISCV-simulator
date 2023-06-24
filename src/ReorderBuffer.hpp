/*
 * Reorder Buffer
 * ---------------------------------------------------------
 * add all the instructions in seq
 * ensure correct commit order
 */
#ifndef CODE_REORDERBUFFER_HPP
#define CODE_REORDERBUFFER_HPP

#include "../include/queue.hpp"
#include "../tool/decoder.hpp"
#include "src/RegFile.hpp"
#include "src/ReservationStation.hpp"
#include "src/StoreBuffer.hpp"
#include "src/LoadBuffer.hpp"
#include "src/predictor.hpp"

class ReorderBuffer;

class RoBType {
    bool ready = false;//ready to commit?
    InstructionType type = WAIT;
    Byte dest{};//dest reg
    Number value = 0; //if instruction is conditional branch, record the other address
    bool predict = false; //predict whether to branch
    friend class ReorderBuffer;

public:
    RoBType() = default;

    explicit RoBType(const Instruction &instruction) : type(instruction.instructionType),
                                                       dest(instruction.rd) {}

    RoBType(const Instruction &instruction, const bool &predict_) : type(instruction.instructionType),
                                                                    dest(instruction.rd),
                                                                    predict(predict_) {}
};

class ReorderBuffer {
    Queue<RoBType> RoBQueue{8};

    Index AddInstruction(const Instruction &instruction);

    Index AddInstruction(const Instruction &instruction, RegisterUnit &pc);

    Index AddInstruction(const Instruction &instruction, const bool &predict, const Number &value);

    /*
     * update pc to the addr of next fetched instruction
     */
    void UpdatePC(const Instruction &instruction, RegisterUnit &pc);

    Number UpdatePC(const Instruction &instruction, const bool &predict, RegisterUnit &pc);

public:
    /*
     * try to add Instruction into RoB
     * if RoBQueue is full return -1
     * else return entry(index in RoBQueue.storage)
     */
    Index AddInstruction(const Instruction &instruction, const RegisterFile &registerFile, const Predictor &predictor,
                         ReservationStation &RS, StoreBuffer &storeBuffer,
                         LoadBuffer &loadBuffer, PCReservationStation &pcReservationStation,
                         RegisterUnit &pc);

    /*
     * modify ele in RoB
     * return dest
     */
    Byte Modify(const std::pair<Index, Number> &pair);

    /*
     * commit all the ready ones
     * if commit EXIT return true
     * R[0]=0
     */
    bool Commit(Registers &registers);
};

/*
 * private
 */

Index ReorderBuffer::AddInstruction(const Instruction &instruction) {
    RoBType tmp(instruction);
    return RoBQueue.EnQueue(tmp);
}

Index ReorderBuffer::AddInstruction(const Instruction &instruction, RegisterUnit &pc) {
    RoBType tmp(instruction);
    tmp.ready = true;
    switch (instruction.instructionType) {
        case LUI:
            tmp.value = instruction.immediate;
            pc += 4;
            break;
        case AUIPC:
            tmp.value = instruction.immediate + pc;
            pc += 4;
            break;
        case JAL:
            tmp.value = pc + 4;
            pc += instruction.immediate;
            break;
        default:
            break;
    }
    return RoBQueue.EnQueue(tmp);
}

Index ReorderBuffer::AddInstruction(const Instruction &instruction, const bool &predict, const Number &value) {

}

void ReorderBuffer::UpdatePC(const Instruction &instruction, RegisterUnit &pc) {

}

Number ReorderBuffer::UpdatePC(const Instruction &instruction, const bool &predict, RegisterUnit &pc) {

}

/*
 * public
 */
Index ReorderBuffer::AddInstruction(const Instruction &instruction, const RegisterFile &registerFile,
                                    const Predictor &predictor,
                                    ReservationStation &RS, StoreBuffer &storeBuffer, LoadBuffer &loadBuffer,
                                    PCReservationStation &pcReservationStation,
                                    RegisterUnit &pc) {
    if (RoBQueue.Full())
        return -1;
    Index ind, entry;
    Number value;
    bool predict;
    switch (instruction.instructionType) {
        //LoadBuffer
        case LB:
        case LH:
        case LW:
        case LBU:
        case LHU:
            if (loadBuffer.Full())
                return -1;
            pc += 4;
            entry = AddInstruction(instruction);
            loadBuffer.AddInstruction(instruction, entry);
            return entry;
        //StoreBuffer
        case SB:
        case SH:
        case SW:
            if (storeBuffer.Full())
                return -1;
            pc += 4;
            entry = AddInstruction(instruction);
            storeBuffer.AddInstruction(instruction, entry);
            return entry;
        case BEQ:
        case BNE:
        case BLT:
        case BGE:
        case BLTU:
        case BGEU:
        case ADDI:
        case SLTI:
        case SLTIU:
        case XORI:
        case ORI:
        case ANDI:
        case SLLI:
        case SRLI:
        case SRAI:
        case ADD:
        case SUB:
        case SLL:
        case SLT:
        case SLTU:
        case XOR:
        case SRL:
        case SRA:
        case OR:
        case AND:
            ind = RS.GetSpace();
            if (ind < 0)return -1;
            predict = predictor.Predict();
            value = UpdatePC(instruction, predict, pc);
            entry = AddInstruction(instruction, predict, value);
            RS.AddInstruction(ind, instruction, entry);
            return entry;
        case LUI:
        case AUIPC:
        case JAL:
            return AddInstruction(instruction, pc);
        case JALR://special pc depend on nrs1
            entry = AddInstruction(instruction);
            pcReservationStation.AddInstruction(instruction,entry);
            return entry;
        default://no need to add into buffer(WAIT)
            return -1;
    }
}


#endif //CODE_REORDERBUFFER_HPP
