/*
 * Store Buffer
 * -----------------------------------------------
 * buffer for store instructions
 * execute in seq
 */
#ifndef CODE_STOREBUFFER_HPP
#define CODE_STOREBUFFER_HPP

#include "../include/queue.hpp"
#include "../include/type.hpp"
#include "../tool/decoder.hpp"
#include "../tool/memory.hpp"
#include "bus.hpp"

class StoreBuffer;

class SBType {
    InstructionType type = LUI;
    Number nrs1{};//value of rs1
    Number nrs2{};
    Number imme{};//immediate number
    Index Q1 = -1;//dependence of nrs1
    Index Q2 = -1;
    Index RoB{};//entry in RoB

    friend class StoreBuffer;

public:
    SBType() = default;

    SBType(const Instruction &instruction, const RegisterFile &registerFile,
           const Index &entry) : type(instruction.instructionType),
                                 imme(instruction.immediate), RoB(entry) {
        Q1 = registerFile.GetValue(instruction.rs1, nrs1);
        Q2 = registerFile.GetValue(instruction.rs2, nrs2);
    }

    friend std::ostream &operator<<(std::ostream &os, const SBType &obj) {
        std::cout << Convert(obj.type) << ' ' << obj.nrs1 << ' ' << obj.nrs2 << ' ' << obj.imme << ' '
                  << (Number) obj.Q1 << ' ' << (Number) obj.Q2 << ' ' << (Number) obj.RoB;
        return os;
    }
};

class StoreBuffer {
    Queue<SBType> SBQueue{8};
    Byte timer = 0;//count time
public:
    /*
     * whether SBQueue have spare space for new instruction
     */
    bool Full();

    void AddInstruction(const Instruction &instruction, const RegisterFile &registerFile, const Index &entry);

    /*
     * execute in one clock cycle
     * if executed, push into bus
     */
    void Execute(CDB &bus, Memory &memory);

    /*
     * remove dependence
     */
    void Modify(const std::pair<Index, Number> &pair);

    void Clear();

    void Print();
};

bool StoreBuffer::Full() {
    return SBQueue.Full();
}

void StoreBuffer::AddInstruction(const Instruction &instruction, const RegisterFile &registerFile, const Index &entry) {
    SBType tmp(instruction, registerFile, entry);
    SBQueue.EnQueue(tmp);
}

void StoreBuffer::Execute(CDB &bus, Memory &memory) {
    Number value;
    if (!SBQueue.Empty()) {
        SBType tmp = SBQueue.GetHead();
        if (timer) {
            if (timer == 1) {
                value = memory.VisitMemory(tmp.type, tmp.nrs1, tmp.nrs2, tmp.imme);
                SBQueue.DeQueue();
                bus.Add(tmp.RoB, value);
            }
            --timer;
            return;
        }
        if (tmp.Q1 == -1 && tmp.Q2 == -1) timer = 3;
    }
}

void StoreBuffer::Modify(const std::pair<Index, Number> &pair) {
    Index front, rear, head;
    SBType tmp;
    SBQueue.HeadRear(front, rear);
    while (front != rear) {
        head = (front + 1) % 8;
        tmp = SBQueue.GetEle(head);
        if (tmp.Q1 == pair.first) {
            tmp.Q1 = -1;
            tmp.nrs1 = pair.second;
            SBQueue.Modify(tmp, head);
        }
        if (tmp.Q2 == pair.first) {
            tmp.Q2 = -1;
            tmp.nrs2 = pair.second;
            SBQueue.Modify(tmp, head);
        }
        front = head;
    }
}

void StoreBuffer::Clear() {
    SBQueue.Clear();
}

void StoreBuffer::Print() {
    std::cout << "STBuffer:\n";
    SBQueue.Print();
}


#endif //CODE_STOREBUFFER_HPP
