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

class SDB {
    std::vector<std::pair<Index, Number>> storage;

public:

    void Add(const Index &entry, Number &value);

    std::pair<Index, Number> GetEle(const int &i) const;

    void Clear();

};

#endif //CODE_BUS_HPP
