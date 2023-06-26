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
#include "RegFile.hpp"
#include "ReservationStation.hpp"
#include "MemoryBuffer.hpp"
#include "predictor.hpp"

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

    friend std::ostream &operator<<(std::ostream &os, const RoBType &obj) {
        std::cout << Convert(obj.type) << ' ' << obj.ready << ' ' << (UnsignedNumber) obj.dest << ' ' << obj.value;
        return os;
    }


};

class ReorderBuffer {
    int counter = 0;
    Queue<RoBType> RoBQueue{8};

    Index AddInstruction(const Instruction &instruction);

    Index AddInstruction(const Instruction &instruction, RegisterUnit &pc, Number &value);

    Index AddInstruction(const Instruction &instruction, const bool &predict, RegisterUnit &pc);

public:

    /*
     * try to add Instruction into RoB
     * if RoBQueue is full return -1
     * else return entry(index in RoBQueue.storage)
     * RegFile need to be modified
     */
    Index AddInstruction(const Instruction &instruction, RegisterFile &registerFile, Predictor &predictor,
                         ReservationStation &RS, MemoryBuffer &memoryBuffer, PCReservationStation &pcReservationStation,
                         RegisterUnit &pc);

    /*
     * modify ele in RoB
     * return dest
     */
    Byte Modify(const std::pair<Index, Number> &pair, Predictor &predictor);

    /*
     * commit all the ready ones
     * if commit EXIT return true
     * if incorrect predict flag=true
     * R[0]=0
     */
    bool Commit(Registers &registers, bool &flag, RegisterUnit &pc);

    void Print();
};

/*
 * private
 */

Index ReorderBuffer::AddInstruction(const Instruction &instruction) {
    RoBType tmp(instruction);
    return RoBQueue.EnQueue(tmp);
}

Index ReorderBuffer::AddInstruction(const Instruction &instruction, RegisterUnit &pc, Number &value) {
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
    value = tmp.value;
    return RoBQueue.EnQueue(tmp);
}

Index ReorderBuffer::AddInstruction(const Instruction &instruction, const bool &predict, RegisterUnit &pc) {
    RoBType tmp(instruction, predict);
    switch (instruction.instructionType) {
        case BEQ:
        case BNE:
        case BLT:
        case BGE:
        case BLTU:
        case BGEU:
            if (!predict) {
                tmp.value = pc + instruction.immediate;
                pc += 4;
            } else {
                tmp.value = pc + 4;
                pc += instruction.immediate;
            }
            return RoBQueue.EnQueue(tmp);
        default:
            pc += 4;
            return RoBQueue.EnQueue(tmp);
    }
}

/*
 * public
 */
Index ReorderBuffer::AddInstruction(const Instruction &instruction, RegisterFile &registerFile,
                                    Predictor &predictor,
                                    ReservationStation &RS, MemoryBuffer &memoryBuffer,
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
            //StoreBuffer
        case SB:
        case SH:
        case SW:
            if (memoryBuffer.Full())
                return -1;
            pc += 4;
            entry = AddInstruction(instruction);
            memoryBuffer.AddInstruction(instruction, registerFile, entry);
            registerFile.Modify(instruction.rd, entry);
            return entry;
        case BEQ:
        case BNE:
        case BLT:
        case BGE:
        case BLTU:
        case BGEU:
            ind = RS.GetSpace();
            if (ind < 0)return -1;
            predict = predictor.Predict();
            entry = AddInstruction(instruction, predict, pc);
            RS.AddInstruction(ind, instruction, registerFile, entry);
            if (instruction.rd < 32) registerFile.Modify(instruction.rd, entry);
            return entry;
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
            entry = AddInstruction(instruction, false, pc);
            RS.AddInstruction(ind, instruction, registerFile, entry);
            if (instruction.rd < 32) registerFile.Modify(instruction.rd, entry);
            return entry;
        case LUI:
        case AUIPC:
        case JAL:
            entry = AddInstruction(instruction, pc, value);
            registerFile.Modify(instruction.rd, value);
            return entry;
        case JALR://special pc depend on nrs1
            entry = AddInstruction(instruction);
            pcReservationStation.AddInstruction(instruction, registerFile, entry);
            return entry;
        case EXIT:
            return AddInstruction(instruction, pc, value);
        default://no need to add into buffer(WAIT)
            return -1;
    }
}

Byte ReorderBuffer::Modify(const std::pair<Index, Number> &pair, Predictor &predictor) {
    RoBType tmp = RoBQueue.GetEle(pair.first);
    bool flag;
    switch (tmp.type) {
        case BEQ:
        case BNE:
        case BLT:
        case BGE:
        case BLTU:
        case BGEU:
            flag = (pair.second == 1);
            predictor.ChangeStatus(flag);
            if (flag == tmp.predict) {
                predictor.Correct();
                tmp.predict = true;
            } else {
                tmp.predict = false;//incorrect predict
            }
            break;
        default:
            tmp.value = pair.second;
    }
    tmp.ready = true;
    RoBQueue.Modify(tmp, pair.first);
    if (tmp.type == SB || tmp.type == SH || tmp.type == SW) return 32;
    return tmp.dest;
}

bool ReorderBuffer::Commit(Registers &registers, bool &flag, RegisterUnit &pc) {
    RoBType tmp;
    if (!RoBQueue.Empty()) {
        tmp = RoBQueue.GetHead();
        if (tmp.ready) {
            if (tmp.type == EXIT) {
                return true;
            }
            if (tmp.type == BEQ ||
                tmp.type == BNE ||
                tmp.type == BLT ||
                tmp.type == BGE ||
                tmp.type == BLTU ||
                tmp.type == BGEU) {
                if (!tmp.predict) {
                    flag = true;
                    pc = tmp.value;
                    RoBQueue.Clear();
                    ++counter;
                    return false;
                }
            }
            RoBQueue.DeQueue();
            registers.Update(tmp.dest, tmp.value);
            ++counter;
        }
    }
    return false;
}

void ReorderBuffer::Print() {
    std::cout << "RoB:\n";
    RoBQueue.Print();
}


#endif //CODE_REORDERBUFFER_HPP
