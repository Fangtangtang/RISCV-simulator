#ifndef CODE_SCANNER_HPP
#define CODE_SCANNER_HPP

#include <iostream>
#include <string>
#include "../include/type.hpp"

class Scanner {
    std::string str;

    inline void StrToAddr(MemoryAddr &memoryAddr);

    inline void StrToCode(MemoryUnit &unit);

public:
    bool HaveMoreTokens();

    //return false if scan addr
    bool Scan(MemoryAddr &memoryAddr, MemoryUnit storage[]);
};


inline void Scanner::StrToAddr(MemoryAddr &memoryAddr) {
    memoryAddr = 0;
    for (int i = 1; i <= 8; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            memoryAddr = (memoryAddr << 4) | (str[i] - '0');
        } else if (str[i] >= 'a' && str[i] <= 'f') {
            memoryAddr = (memoryAddr << 4) | (str[i] - 'a' + 10);
        } else if (str[i] >= 'A' && str[i] <= 'F') {
            memoryAddr = (memoryAddr << 4) | (str[i] - 'A' + 10);
        }
    }
}

inline void Scanner::StrToCode(MemoryUnit &unit) {
    unit = 0;
    for (char i: str) {
        if (i >= '0' && i <= '9') {
            unit = (unit << 4) | (i - '0');
        } else if (i >= 'a' && i <= 'f') {
            unit = (unit << 4) | (i - 'a' + 10);
        } else if (i >= 'A' && i <= 'F') {
            unit = (unit << 4) | (i - 'A' + 10);
        }
    }
}

bool Scanner::HaveMoreTokens() {
    if (std::cin >> str)return true;
    return false;
}

bool Scanner::Scan(MemoryAddr &memoryAddr, MemoryUnit storage[]) {
    if (str[0] == '@') {
        StrToAddr(memoryAddr);
        return false;
    }
    StrToCode(storage[memoryAddr]);
    ++memoryAddr;
    return true;
}

#endif //CODE_SCANNER_HPP
