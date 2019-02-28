//
// Created by mggk on 23/02/19.
//
#include "fstream"
#include <algorithm>
#include <array>
#include "psmcsave.h"

psmcsave::psmcsave() {

}

void psmcsave::setDataBuffer(vector<unsigned char> buff) {
    saveBuffer = buff;
}

void psmcsave::setSaveAddress(int address) {
    SAVE_ADDRESS = address;
}

string psmcsave::getGameName() {
    const int GN_FB = 0x4;
    const int GN_LB = 0x43;
    vector<unsigned char>::const_iterator first = saveBuffer.begin() + GN_FB;
    vector<unsigned char>::const_iterator last = saveBuffer.begin() + GN_LB;
    vector<unsigned char> gameNameEncoded(first,last);
    string gameNameDecoded(gameNameEncoded.begin(), gameNameEncoded.end());
    return shiftJISToUTF8(gameNameDecoded);
}

SDL_Texture *psmcsave::getGameIcon(SDL_Renderer *renderer) {
    SDL_Texture *icon = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STATIC,16,16);
    const int PALETTE_BEGIN = 0x60;
    const int ICON_BEGIN = 0x80;
    const int FRAME_SIZE = 0x7F;
    unsigned char *color[4];
    unsigned char colorPalette[16][4];
    uint8_t pixels[16*16*4];
    int pixPos = 0;
    for(int i = 0; i < 32; i+=2){
        uint16_t fB = (uint16_t)saveBuffer[PALETTE_BEGIN + i];
        uint16_t lB = (uint16_t)saveBuffer[PALETTE_BEGIN + i+1];
        uint16_t word = (lB << 8) + fB;
        unsigned char R = (word & 0x3F) << 3;
        unsigned char G = (word & 0x3E0) >> 2;
        unsigned char B = (word & 0x7C00) >> 7;
        unsigned char A = ((word & 32768) >> 15 )==1 ? true:false;
        /*color[0] = &R;
        color[1] = &G;
        color[2] = &B;
        color[3] = &A;*/
        colorPalette[i/2][0] = R;
        colorPalette[i/2][1] = G;
        colorPalette[i/2][2] = B;
        colorPalette[i/2][3] = A;
    }
    for(int i = ICON_BEGIN; i < ICON_BEGIN+FRAME_SIZE+1;i++){
        uint16_t bytes = saveBuffer[i];
        uint16_t color2 = (bytes & 0xF);
        uint16_t color1 = (bytes & 0xF0) >> 4;
        pixels[pixPos] = 1;
        pixels[pixPos+1] = colorPalette[color2][2];
        pixels[pixPos+2] = colorPalette[color2][1];
        pixels[pixPos+3] = colorPalette[color2][0];
        pixels[pixPos+4] = 1;
        pixels[pixPos+5] = colorPalette[color1][2];
        pixels[pixPos+6] = colorPalette[color1][1];
        pixels[pixPos+7] = colorPalette[color1][0];
        pixPos+=8;
    }
    SDL_UpdateTexture(icon, NULL, pixels,16*4);
    return icon;
}

string psmcsave::shiftJISToUTF8(const string &input) {

    ifstream convTableFile("shiftjis.dat",ios_base::binary);
    vector<unsigned char> convTableVec(istreambuf_iterator<char>(convTableFile), {});

    std::string output(3 * input.length(), ' '); //ShiftJis won't give 4byte UTF8, so max. 3 byte per input char are needed
    size_t indexInput = 0, indexOutput = 0;

    while(indexInput < input.length())
    {
        char arraySection = ((uint8_t)input[indexInput]) >> 4;
        size_t arrayOffset;
        if(arraySection == 0x8) arrayOffset = 0x100; //these are two-byte shiftjis
        else if(arraySection == 0x9) arrayOffset = 0x1100;
        else if(arraySection == 0xE) arrayOffset = 0x2100;
        else arrayOffset = 0; //this is one byte shiftjis

        //determining real array offset
        if(arrayOffset)
        {
            arrayOffset += (((uint8_t)input[indexInput]) & 0xf) << 8;
            indexInput++;
            if(indexInput >= input.length()) break;
        }
        arrayOffset += (uint8_t)input[indexInput++];
        arrayOffset <<= 1;

        //unicode number is...
        uint16_t unicodeValue = (convTableVec[arrayOffset] << 8) | convTableVec[arrayOffset + 1];

        //converting to UTF8
        if(unicodeValue > 0){
            if(unicodeValue < 0x80)
            {
                output[indexOutput++] = unicodeValue;
            }
            else if(unicodeValue < 0x800)
            {
                output[indexOutput++] = 0xC0 | (unicodeValue >> 6);
                output[indexOutput++] = 0x80 | (unicodeValue & 0x3f);
            }
            else
            {
                output[indexOutput++] = 0xE0 | (unicodeValue >> 12);
                output[indexOutput++] = 0x80 | ((unicodeValue & 0xfff) >> 6);
                output[indexOutput++] = 0x80 | (unicodeValue & 0x3f);
            }
        }


    }

    output.resize(indexOutput); //remove the unnecessary bytes
    return output;
}