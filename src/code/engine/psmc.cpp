//
// Created by mggk on 22/02/19.
//

#include "psmc.h"
#include "fstream"
#include <vector>
#include <iterator>

psmc::psmc() {
}

void psmc::loadFile(string filename) {
    ifstream input(filename,ios_base::binary);
    vector<unsigned char> buffer(istreambuf_iterator<char>(input), {});
    data = buffer;
    parseSaveDatas();
}

void psmc::parseSaveDatas() {
    const int FIRST_SAVE_ADDRESS = 0x2000;
    const int SAVE_NUMBER = 15;
    const int BLOCK_SIZE = 8192;
    const int FRAME_SIZE = 128;
    psmcsave tmpSave;

    int savePos = FIRST_SAVE_ADDRESS;
    for(int i = 0; i < SAVE_NUMBER; i++){
        vector<unsigned char>::const_iterator first = data.begin() + savePos;
        vector<unsigned char>::const_iterator last = data.begin() + savePos + BLOCK_SIZE;
        vector<unsigned char> currentSaveBuffer(first,last);
        tmpSave.setDataBuffer(currentSaveBuffer);
        tmpSave.setSaveAddress(savePos);
        entries[i] = tmpSave;
        savePos+=BLOCK_SIZE;
    }
}