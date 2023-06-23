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

class RoBType {
//    bool busy = false;
    bool ready = false;//ready to commit?
    InstructionType type;
    Byte dest;//dest reg
    Number value = 0; //if instruction is conditional branch, record the other address
    bool predict = false; //predict whether to branch
};

class ReorderBuffer {
    Queue<RoBType> RoBQueue{8};

    /*
     * try to add Instruction into RoB
     * if RoBQueue is full return -1
     * else return entry(index in RoBQueue.storage)
     */
    Index AddInstruction(const Instruction &instruction);

    /*
     * modify ele in RoB
     * return dest
     */
    Byte Modify(const Index &entry, const Number &value);

    /*
     * commit all the ready ones
     */
    bool Commit();
};

#endif //CODE_REORDERBUFFER_HPP
