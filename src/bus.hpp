/*
 * CDB
 * ------------------------------------
 * used to broadcast
 * record pair of entry(index in RoB) and value
 * remove dependence in Buffers
 */
#ifndef CODE_BUS_HPP
#define CODE_BUS_HPP

#include "../include/type.hpp"
#include <utility>
#include <vector>

class CDB {
    std::vector<std::pair<Index, Number>> storage;

public:

    void Add(const Index &entry, Number &value);

    int Size() const;

    std::pair<Index, Number> GetEle(const int &i) const;

    void Clear();

};

void CDB::Add(const Index &entry, Number &value) {
    storage.emplace_back(entry, value);
}

int CDB::Size() const {
    return storage.size();
}

std::pair<Index, Number> CDB::GetEle(const int &i) const {
    return storage[i];
}

void CDB::Clear() {
    storage.clear();
}

#endif //CODE_BUS_HPP
