#include "src/ReorderBuffer.hpp"
#include "src/bus.hpp"
#include "tool/memory.hpp"
#include "src/predictor.hpp"



int main() {
    Decoder decoder;
    Memory memory{};
    Registers registers;
    ALU alu;
    ReorderBuffer RoB;
    ReservationStation RS;
    StoreBuffer storeBuffer;
    LoadBuffer loadBuffer;
    RegisterFile registerFile;
    Predictor predictor;
    CDB bus;
    memory.Initialize();//scan all the code
    registerFile.Reset();
    Register pc;//pc points at the address of code, used in IF
    MachineCode pre_machineCode = 0, after_machineCode = 0;
    Instruction instruction;
    bool IP_flag = true;//instruction process flag
    Index entry;
    int size_;
    Byte dest;
    std::pair<Index, Number> pair;
    while (true) {
        if (IP_flag) {
            //IF
            pre_machineCode = after_machineCode;
            memory.InstructionFetch(pc.pre_state, after_machineCode);
            pc.after_state = pc.pre_state + 4;
            //ID
            decoder.Decode(pre_machineCode, instruction);//decode machine code to get instruction
        }
        //try to add into buffers
        entry = RoB.AddInstruction(instruction, registerFile, predictor, RS, storeBuffer, loadBuffer,pc);
        if (entry < 0) {//fail to add
            IP_flag = false;
        } else {
            IP_flag = true;
        }
        //EX MEM
        RS.Execute(bus);
        loadBuffer.Execute(bus);
        storeBuffer.Execute(bus);
        size_ = bus.Size();
        for (int i = 0; i < size_; ++i) {
            pair = bus.GetEle(i);
            RS.Modify(pair);
            loadBuffer.Modify(pair);
            storeBuffer.Modify(pair);
            dest = RoB.Modify(pair);
            registerFile.Update(dest, pair);
        }
        bus.Clear();
        //WB
        if (RoB.Commit(registers)) break;
    }
    std::cout << ((UnsignedNumber) registers.ReadRegister() & 255);
    return 0;
}