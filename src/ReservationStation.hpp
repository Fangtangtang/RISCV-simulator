/*
 * Reservation Station
 * --------------------------------------------------------
 * buffer for ALU
 * execute if parameters are available
 */
#ifndef CODE_RESERVATIONSTATION_HPP
#define CODE_RESERVATIONSTATION_HPP

#include "../include/type.hpp"
#include "../tool/decoder.hpp"
#include "../tool/ALU.hpp"

class RSType {
    bool busy = false;
    InstructionType type;
    Number nrs1, nrs2;
    Number Imme;
    Index Q1, Q2;
    Index RoB;
};

class ReservationStation {
    RSType RS[8];

    /*
     * return index of RS if is not busy
     * return -1 if full
     */
    Index GetSpace();

    /*
     * add instruction to the spare space
     */
    void AddInstruction(const Index &index, const Instruction &instruction, const Index &entry);

    /*
     * execute in one clock cycle
     * return entry in RoB and the result
     * if nothing finish executing return -1
     */
    Index Execute(Number &result);

    /*
     * remove dependence
     * traverse RS to modify
     */
    void Modify(const Index &entry, const Number &value);

};

#endif //CODE_RESERVATIONSTATION_HPP
