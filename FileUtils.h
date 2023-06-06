#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

using namespace std;

class FileUtils {
protected:
    string m_filefullpath;
    bool bIsValid;
    size_t m_iIndex;

public:
    FileUtils ();
    
    // Methods
    void setFileFullPath(string);
    string getFilename(); // just the filename.
    string getFileFullPath();
    string getSuffix(); // Just the .doc extension.
    bool fileExists();
    uint32_t getFileSize();
    bool getIsValid();
    size_t getBytes(uint32_t, unsigned char *);

    size_t getIndex() { return m_iIndex; }
    void setIndex(size_t pos) { m_iIndex = pos; }
};


#endif