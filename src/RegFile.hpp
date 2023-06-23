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
 * TODO PC included?
 */
#ifndef CODE_REGFILE_HPP
#define CODE_REGFILE_HPP

#include <utility>
#include "../include/type.hpp"
#include "../tool/register.hpp"

class RFType {
    Number value{};
    Index dependence = -1;
};

class RegisterFile {
    RFType storage[32];
    RFType PC;

public:
    /*
     * initialize or reset value in RF
     * dependence set to -1
     */
    void Reset();

    /*
     * update dest reg if dependence need to be removed
     */
    void Update(const Byte dest, const std::pair<Index, Number> &pair);

    /*
     * return dependence or value of RF[ind]
     */
    Index GetValue(const Byte &ind, Number &value);
};

#endif //CODE_REGFILE_HPP
