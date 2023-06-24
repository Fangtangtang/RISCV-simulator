/*
 * predictor
 * ------------------------------------------------
 * branch predictor
 */
#ifndef CODE_PREDICTOR_HPP
#define CODE_PREDICTOR_HPP
#include "../include/type.hpp"
class Predictor{
   Byte status=0;
   uint32_t total=0;
   uint32_t correct=0;

public:

    bool Predict() const;

    /*
     * whether branch
     * whether correct
     */
    void Judge(const bool &branch,const bool &flag);
};
#endif //CODE_PREDICTOR_HPP
