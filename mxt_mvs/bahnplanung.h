//
// Created by Johannes on 27.04.2021.
//

#ifndef RV4A_MXT_BAHNPLANUNG_H
#define RV4A_MXT_BAHNPLANUNG_H
#include "strdef.h"
#include <vector>

typedef struct {
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> z;
    std::vector<float> t;
}STEPS;

STEPS Sinoide(POSE start, POSE target,float speed, float acc);



#endif //RV4A_MXT_BAHNPLANUNG_H
