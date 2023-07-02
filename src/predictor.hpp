/*
 * predictor
 * ------------------------------------------------
 * branch predictor
 */
#ifndef CODE_PREDICTOR_HPP
#define CODE_PREDICTOR_HPP

#include "../include/type.hpp"

class Predictor {
    Byte status = 0;
    uint32_t total = 0;
    uint32_t correct = 0;

public:

    inline bool Predict();

    /*
     * whether branch
     * whether correct
     */
    inline void Correct();

    void ChangeStatus(const bool &flag);

    void ShowRate() const;
};

bool Predictor::Predict() {
    ++total;
    return false;
    if ((status >> 1) & 1) return true;
    return false;
}

void Predictor::Correct() {
    ++correct;
}

void Predictor::ChangeStatus(const bool &flag) {
    Byte tmp = status & 3;
    if (flag) {
        if (tmp != 3)++status;
        return;
    }
    if (tmp)--status;
}

void Predictor::ShowRate() const {
    std::cout << correct << ' ' << total << ' ' << (double) correct / total << '\n';
}


class Predictor1 {
    Byte status[16] = {0};
    int map[8] = {0};
    uint32_t total = 0;
    uint32_t correct = 0;

public:

    inline bool Predict(const RegisterUnit &pc);

    inline void Record(const Index &entry, const RegisterUnit &pc);

    /*
     * whether branch
     * whether correct
     */
    inline void Correct();

    void ChangeStatus(const bool &flag, const Index &entry);

    void ShowRate() const;
};

bool Predictor1::Predict(const RegisterUnit &pc) {
    ++total;
//    return false;
    if ((status[(pc & 15)] >> 1) & 1) return true;
    return false;
}

void Predictor1::Record(const Index &entry, const RegisterUnit &pc) {
    map[entry] = (pc & 15);
}

void Predictor1::Correct() {
    ++correct;
}

void Predictor1::ChangeStatus(const bool &flag, const Index &entry) {
//    map[entry]=0;
    Byte tmp = status[map[entry]] & 3;
    if (flag) {
        if (tmp != 3)++status[map[entry]];
        return;
    }
    if (tmp)--status[map[entry]];
}

void Predictor1::ShowRate() const {
    std::cout << correct << ' ' << total << ' ' << (double) correct / total << '\n';
}

#endif //CODE_PREDICTOR_HPP
