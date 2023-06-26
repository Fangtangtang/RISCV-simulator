/*
 * Load Buffer
 * -----------------------------------------------
 * buffer for load instructions
 * execute in seq
 */
#ifndef CODE_LOADBUFFER_HPP
#define CODE_LOADBUFFER_HPP

#include "../include/queue.hpp"
#include "../include/type.hpp"
#include "../tool/decoder.hpp"
#include "../tool/memory.hpp"
#include "bus.hpp"

class LoadBuffer;

class LBType {
    InstructionType type = LUI;
    Number nrs1{};//value of rs1
    Number imme{};//immediate number
    Index Q = -1;//dependence of nrs1
    Index RoB{};//entry in RoB

    friend class LoadBuffer;

public:
    LBType() = default;

    LBType(const Instruction &instruction, const RegisterFile &registerFile,
           const Index &entry) : type(instruction.instructionType),
                                 imme(instruction.immediate), RoB(entry) {
        Q = registerFile.GetValue(instruction.rs1, nrs1);
    }

    friend std::ostream &operator<<(std::ostream &os, const LBType &obj) {
        std::cout << Convert(obj.type) << ' ' << obj.nrs1 << ' ' << obj.imme << ' ' << (Number) obj.Q << ' '
                  << (Number) obj.RoB;
        return os;
    }
};

class LoadBuffer {
    Queue<LBType> LBQueue{8};
    Byte timer = 0;//count time
public:
    /*
     * whether LBQueue have spare space for new instruction
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

bool LoadBuffer::Full() {
    return LBQueue.Full();
}

void LoadBuffer::AddInstruction(const Instruction &instruction, const RegisterFile &registerFile, const Index &entry) {
    LBType tmp(instruction, registerFile, entry);
    LBQueue.EnQueue(tmp);
}

void LoadBuffer::Execute(CDB &bus, Memory &memory) {
    Number value;
    if (!LBQueue.Empty()) {
        LBType tmp = LBQueue.GetHead();
        if (timer) {
            if (timer == 1) {
                value = memory.VisitMemory(tmp.type, tmp.nrs1, 0, tmp.imme);
                LBQueue.DeQueue();
                bus.Add(tmp.RoB, value);
            }
            --timer;
            return;
        }
        if (tmp.Q == -1) timer = 3;
    }
}

void LoadBuffer::Modify(const std::pair<Index, Number> &pair) {
    Index front, rear, head;
    LBType tmp;
    LBQueue.HeadRear(front, rear);
    while (front != rear) {
        head = (front + 1) % 8;
        tmp = LBQueue.GetEle(head);
        if (tmp.Q == pair.first) {
            tmp.Q = -1;
            tmp.nrs1 = pair.second;
            LBQueue.Modify(tmp,head);
        }
        front = head;
    }
}

void LoadBuffer::Clear() {
    LBQueue.Clear();
}

void LoadBuffer::Print() {
    std::cout << "LBBuffer:\n";
    LBQueue.Print();
}


#endif //CODE_LOADBUFFER_HPP
