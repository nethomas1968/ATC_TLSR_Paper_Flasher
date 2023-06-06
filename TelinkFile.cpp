#include <cstdint>
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "TelinkFile.h"

using namespace std;

TelinkFile::TelinkFile(string filePath)
{
    setFileFullPath(filePath);
}

bool TelinkFile::checkSignature()
{
    ssize_t prevIndex;
    unsigned char bts[4];


    // Look for the 0x544C4E4B signature starting at byte 8.
    prevIndex = getIndex();
    setIndex(8); // Skip forward to byte 8, and read 4 bytes.
    getBytes(4, bts);
    if (bts[0] == 0x4B && bts[1] == 0x4E && bts[2] == 0x4C && bts[3] == 0x54) {
        // Signature is correct.
        setIndex(0); 
        return true;
    }
    setIndex(prevIndex); 
    return false;
}