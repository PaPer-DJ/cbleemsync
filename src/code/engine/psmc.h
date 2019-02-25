//
// Created by mggk on 22/02/19.
//

#include <string>
#include <vector>
#include "psmcsave.h"

#ifndef PSC_BENCH_PSMEMORYCARD_H
#define PSC_BENCH_PSMEMORYCARD_H

using namespace std;

class psmc {

public:
    psmc();
    void loadFile(string filename);
    psmcsave entries[15];

private:
    vector<unsigned char> data;
    void parseSaveDatas();
};


#endif //PSC_BENCH_PSMEMORYCARD_H
