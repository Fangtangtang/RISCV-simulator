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

    /*
     * whether SBQueue have spare space for new instruction
     */
    bool Full();

    void AddInstruction(const Instruction &instruction, const Index &entry);

    /*
     * execute in one clock cycle
     * return entry in RoB
     * if nothing finish executing return -1
     */
    Index Execute();

    /*
     * remove dependence
     * TODO
     * traverse to modify?
     */
    void Modify(const Index &entry, const Number &value);
};

#endif //CODE_STOREBUFFER_HPP
