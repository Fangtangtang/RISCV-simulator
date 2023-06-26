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


#endif //CODE_PREDICTOR_HPP
