
#ifndef CODE_MEMORYBUFFER_HPP
#define CODE_MEMORYBUFFER_HPP

#include "../include/queue.hpp"
#include "../include/type.hpp"
#include "../tool/decoder.hpp"
#include "../tool/memory.hpp"
#include "RegFile.hpp"
#include "bus.hpp"

class MemoryBuffer;

class MemType {
    InstructionType type = LUI;
    Number nrs1{};//value of rs1
    Number nrs2{};
    Number imme{};//immediate number
    Index Q1 = -1;//dependence of nrs1
    Index Q2 = -1;
    Index RoB{};//entry in RoB

    friend class MemoryBuffer;

public:
    MemType() = default;

    MemType(const Instruction &instruction, const RegisterFile &registerFile,
            const Index &entry) : type(instruction.instructionType),
                                  imme(instruction.immediate), RoB(entry) {
        Q1 = registerFile.GetValue(instruction.rs1, nrs1);
        Q2 = registerFile.GetValue(instruction.rs2, nrs2);
    }

    friend std::ostream &operator<<(std::ostream &os, const MemType &obj) {
        std::cout << Convert(obj.type) << ' ' << obj.nrs1 << ' ' << obj.nrs2 << ' ' << obj.imme << ' '
                  << (Number) obj.Q1 << ' ' << (Number) obj.Q2 << ' ' << (Number) obj.RoB;
        return os;
    }
};

class MemoryBuffer {
    Queue<MemType> MemQueue{8};
    Byte timer = 0;//count time
public:
    /*
     * whether MemQueue have spare space for new instruction
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

bool MemoryBuffer::Full() {
    return MemQueue.Full();
}

void MemoryBuffer::AddInstruction(const Instruction &instruction, const RegisterFile &registerFile, const Index &entry) {
    MemType tmp(instruction, registerFile, entry);
    MemQueue.EnQueue(tmp);
}

void MemoryBuffer::Execute(CDB &bus, Memory &memory) {
    Number value;
    if (!MemQueue.Empty()) {
        MemType tmp = MemQueue.GetHead();
        if (timer) {
            if (timer == 1) {
                value = memory.VisitMemory(tmp.type, tmp.nrs1, tmp.nrs2, tmp.imme);
                MemQueue.DeQueue();
                bus.Add(tmp.RoB, value);
            }
            --timer;
            return;
        }
        if (tmp.Q1 == -1 && tmp.Q2 == -1) timer = 3;
    }
}

void MemoryBuffer::Modify(const std::pair<Index, Number> &pair) {
    Index front, rear, head;
    MemType tmp;
    MemQueue.HeadRear(front, rear);
    while (front != rear) {
        head = (front + 1) % 8;
        tmp = MemQueue.GetEle(head);
        if (tmp.Q1 == pair.first) {
            tmp.Q1 = -1;
            tmp.nrs1 = pair.second;
            MemQueue.Modify(tmp, head);
        }
        if (tmp.Q2 == pair.first) {
            tmp.Q2 = -1;
            tmp.nrs2 = pair.second;
            MemQueue.Modify(tmp, head);
        }
        front = head;
    }
}

void MemoryBuffer::Clear() {
    MemQueue.Clear();
}

void MemoryBuffer::Print() {
    std::cout << "MemBuffer:\n";
    MemQueue.Print();
}

#endif //CODE_MEMORYBUFFER_HPP