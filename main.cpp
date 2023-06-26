#include "src/ReorderBuffer.hpp"
#include "src/bus.hpp"
#include "tool/memory.hpp"
#include "src/predictor.hpp"
#include "src/RegFile.hpp"
#include "src/ReservationStation.hpp"
#include "src/MemoryBuffer.hpp"

int main() {
//    freopen("/mnt/f/repo/RISCV-simulator/test.data", "r", stdin);
//    freopen("my.out", "w", stdout);
    Decoder decoder;
    Memory memory{};
    Registers registers;
    ALU alu;
    ReorderBuffer RoB;
    ReservationStation RS;
    PCReservationStation pcRS{};
    MemoryBuffer memoryBuffer;
    RegisterFile registerFile;
    Predictor predictor;
    CDB bus;
    memory.Initialize();//scan all the code
    registerFile.Reset(registers);
    RegisterUnit pc;//pc points at the address of code, used in IF
    MachineCode machineCode = 0;
    Instruction instruction;
    bool process_flag = true, reset_flag = false;//instruction process flag
    Index entry;
    int size_;
    Byte dest;
    uint64_t clock = 0;
    std::pair<Index, Number> pair;
    while (true) {//simulate a clock cycle
        /*
         * IF and ID
         * process together
         * finish after 2 clock cycle
         */
        if (clock & 1 && process_flag && instruction.instructionType == WAIT) {
            memory.InstructionFetch(pc, machineCode);
            decoder.Decode(machineCode, instruction);//decode machine code to get instruction
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
        memoryBuffer.Execute(bus, memory);
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
         * commit instructions
         */
        if (RoB.Commit(registers, reset_flag, pc)) break;
        if (reset_flag) {
            registerFile.Reset(registers);
            pcRS.Clear();
            RS.Clear();
            memoryBuffer.Clear();
            machineCode = 0;
            instruction.instructionType = WAIT;
            process_flag= true;
            reset_flag = false;
        }
        ++clock;
//        registerFile.Print();
//        RoB.Print();
//        registers.Print();
//        RS.Print();
//        memoryBuffer.Print();
//        loadBuffer.Print();
//        storeBuffer.Print();
    }
    std::cout << ((UnsignedNumber) registers.ReadRegister() & 255);
    return 0;
}


//            std::cout << clock << '\t' << std::hex << pc << std::dec << '\t' << Convert(instruction.instructionType)
//                      << ' '
//                      << unsigned(instruction.rs1) << ' ' << unsigned(instruction.rs2) << ' '
//                      << unsigned(instruction.rd) << ' '
//                      << instruction.immediate << '\n';