/*
 * Register File
 * -----------------------------------
 * real-time value and dependence
 * initialize get all the values from registers
 *
 * if Reg have dependence, record dependence
 * else get value
 *
 * remove dependence in Register when depended RoB executed
 *
 */
#ifndef CODE_REGFILE_HPP
#define CODE_REGFILE_HPP

#include <utility>
#include "../include/type.hpp"
#include "../tool/register.hpp"

class RFType {
    Number value{};
    Index dependence = -1;

    friend class RegisterFile;
};

class RegisterFile {
    RFType storage[32];

public:
    /*
     * initialize or reset value in RF
     * dependence set to -1
     */
    void Reset(const Registers &registers);

    void Modify(const Byte &ind, const Index &entry);

    void Modify(const Byte &ind, const Number &value);

    /*
     * update dest reg if dependence need to be removed
     */
    void Update(const Byte &dest, const std::pair<Index, Number> &pair);

    /*
     * return dependence or value of RF[ind]
     */
    Index GetValue(const Byte &ind, Number &value) const;

    void Print();
};

void RegisterFile::Reset(const Registers &registers) {
    for (int i = 0; i < 32; ++i) {
        storage[i].value = registers.aRegister[i];
        storage[i].dependence = -1;
    }
}

void RegisterFile::Modify(const Byte &ind, const Index &entry) {
    storage[ind].dependence = entry;
}

void RegisterFile::Modify(const Byte &ind, const Number &value) {
    storage[ind].value = value;
}

void RegisterFile::Update(const Byte &dest, const std::pair<Index, Number> &pair) {
    if (dest && dest != 32 && storage[dest].dependence == pair.first) {
        storage[dest].value = pair.second;
        storage[dest].dependence = -1;
    }
}

Index RegisterFile::GetValue(const Byte &ind, Number &value) const {
    if (ind == 32)return -1;
    if (storage[ind].dependence == -1) value = storage[ind].value;
    return storage[ind].dependence;
}

void RegisterFile::Print() {
    std::cout << "RegFile:\n";
    for (auto &i: storage) {
        std::cout << i.value << ' ' << (Number) i.dependence << '\n';
    }
}


#endif //CODE_REGFILE_HPP
