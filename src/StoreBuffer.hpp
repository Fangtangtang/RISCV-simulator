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

class SBType {
    InstructionType type;
    Number nrs1;//value of rs1
    Number nrs2;
    Number imme;//immediate number
    Index Q1 = -1;//dependence of nrs1
    Index Q2 = -1;
    Index RoB;//entry in RoB
};

class StoreBuffer {
    Queue<SBType> SBQueue{8};
    Byte timer;//count time
public:
    /*
     * whether SBQueue have spare space for new instruction
     */
    bool Full();

    void AddInstruction(const Instruction &instruction, const Index &entry);

    /*
     * execute in one clock cycle
     * if executed, push into bus
     */
    void Execute(CDB &bus);

    /*
     * remove dependence
     * TODO
     * traverse to modify?
     */
    void Modify(const std::pair<Index, Number> &pair);
};

#endif //CODE_STOREBUFFER_HPP
