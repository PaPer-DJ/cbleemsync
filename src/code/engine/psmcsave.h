//
// Created by mggk on 23/02/19.
//

#include "vector"
#include <string>
#include <SDL2/SDL_system.h>

#ifndef PSC_BENCH_PSMCDIRECTORY_H
#define PSC_BENCH_PSMCDIRECTORY_H

using namespace std;

class psmcsave {
public:
    psmcsave();
    void setDataBuffer(vector<unsigned char> buff);
    string getGameName();
    SDL_Texture *getGameIcon(SDL_Renderer *renderer);
    void setSaveAddress(int address);

private:
    vector<unsigned char> saveBuffer;
    int SAVE_ADDRESS;

    string shiftJISToUTF8(const string &input);
    uint8_t getshiftJISArray[25088];
};


#endif //PSC_BENCH_PSMCDIRECTORY_H
