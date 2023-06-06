#include <cstdint>
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "FileUtils.h"

using namespace std;

FileUtils::FileUtils() {
    m_filefullpath = "";
    bIsValid = false;
    m_iIndex = 0;
}

// Specify the full path to the file.
void FileUtils::setFileFullPath(string s) 
{
    m_filefullpath = s;
    
    if (fileExists() && getFileSize() > 0) {
        bIsValid = true;
    }
    
    m_iIndex = 0;
}

string FileUtils::getFileFullPath()
{
    return m_filefullpath;
}

string FileUtils::getSuffix()
{
    if (m_filefullpath.find_last_of(".") != std::string::npos) {
        return m_filefullpath.substr(m_filefullpath.find_last_of(".")+1);
    }
    return "";
}

// Return just the filename from the full path.
string FileUtils::getFilename()
{
  return m_filefullpath.substr(m_filefullpath.find_last_of("/\\") + 1);
}

bool FileUtils::fileExists()
{
    bool bRet = false;

    if (m_filefullpath != "" ) {
        ifstream infile(m_filefullpath.c_str());
        bRet = infile.good();
    }

    return bRet;
}

uint32_t FileUtils::getFileSize()
{
    uint32_t size = 0;
    if (fileExists())
    {
        ifstream in(m_filefullpath.c_str(), ifstream::ate | ifstream::binary);
	    size = (uint32_t)in.tellg();
    }
    return size;
}

bool FileUtils:: getIsValid()
{
    return bIsValid;
}

size_t FileUtils::getBytes(uint32_t num, unsigned char *buf)
{
    size_t iNumCopied = 0;
    FILE* fp = fopen(m_filefullpath.c_str(), "rb");
    if (fp) {
        fseek(fp, m_iIndex, SEEK_SET);
        iNumCopied = fread(buf, sizeof(unsigned char), num, fp);
        fclose(fp);
        m_iIndex += iNumCopied;
    }
    
    return iNumCopied;
}

