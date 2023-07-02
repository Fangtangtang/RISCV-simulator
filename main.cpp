#include "src/ReorderBuffer.hpp"
#include "src/bus.hpp"
#include "tool/memory.hpp"
#include "src/predictor.hpp"
#include "src/RegFile.hpp"
#include "src/ReservationStation.hpp"
#include "src/MemoryBuffer.hpp"

int main() {
//    freopen("out","w",stdout);
    Decoder decoder;
    Memory memory{};
    Registers registers;
    ALU alu;
    ReorderBuffer RoB;
    ReservationStation RS;
    PCReservationStation pcRS{};
    MemoryBuffer memoryBuffer;
    RegisterFile registerFile;
//    Predictor predictor;
    Predictor1 predictor;
    CDB bus;
    memory.Initialize();//scan all the code
    registerFile.Reset(registers);
    RegisterUnit pc;//pc points at the address of code, used in IF
    MachineCode machineCode = 0;
    Instruction instruction;
    bool process_flag = true, reset_flag = false;//instruction process flag
    bool store_flag = false;
    Index entry;
    int size_;
    Byte dest;
    Byte clock = 2;
    std::pair<Index, Number> pair;
    while (true) {//simulate a clock cycle
        /*
         * IF and ID
         * process together
         * finish after 2 clock cycle
         */
        if (clock)--clock;
        if (clock == 0 && process_flag && instruction.instructionType == WAIT) {
            memory.InstructionFetch(pc, machineCode);
            decoder.Decode(machineCode, instruction);//decode machine code to get instruction
            clock = 2;
        }
        /*
         * EXE
         * try to add instruction into RoB
         * process in every buffer
         * broadcast and remove dependence
         */
        if (instruction.instructionType != WAIT) {
            entry = RoB.AddInstruction(instruction, registerFile, predictor, RS, memoryBuffer, pcRS, pc);
            if (instruction.instructionType == JALR ||
                instruction.instructionType == EXIT) {
                process_flag = false;//pause
            }
            if (entry >= 0) instruction.instructionType = WAIT;//instruction added
        }
        if (!process_flag && pcRS.Execute(bus, pc)) process_flag = true;
        RS.Execute(bus, alu);
        memoryBuffer.Execute(store_flag, bus, memory);
        size_ = bus.Size();
        for (int i = 0; i < size_; ++i) {
            pair = bus.GetEle(i);
            RS.Modify(pair);
            memoryBuffer.Modify(pair);
            if (!process_flag)pcRS.Modify(pair);
            dest = RoB.Modify(pair, predictor);
            registerFile.Update(dest, pair);
        }
        bus.Clear();
        /*
         * WB
         * commit one instruction at most
         */
        if (RoB.Commit(registers, reset_flag, pc)) break;
        store_flag = RoB.NextIsStore();
        if (reset_flag) {
            registerFile.Reset(registers);
            pcRS.Clear();
            RS.Clear();
            memoryBuffer.Clear();
            machineCode = 0;
            instruction.instructionType = WAIT;
            process_flag = true;
            reset_flag = false;
        }
    }
//    predictor.ShowRate();
    std::cout << ((UnsignedNumber) registers.ReadRegister() & 255);
    return 0;
}