#ifndef _TELINKFILE_H_
#define _TELINKFILE_H_

using namespace std;

#include "FileUtils.h"

class TelinkFile: public FileUtils
{

public:
    TelinkFile (string filePath);
    
    // Methods
    bool checkSignature();
};


#endif