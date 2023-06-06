#ifndef _MCUTOOLS_H_
#define _MCUTOOLS_H_

using namespace std;

#include "Serial.h"

class MCUTools
{
public:
    MCUTools(Serial *s, uint32_t delay);

    void setLogger(Logger* l) { m_pLogger = l; }

    void softResetMCU();
    string toHex(uint32_t number, size_t length);

    void flashByteCmd(uint8_t cmd);
    void flashWriteEnable();
    void flashWakeUp();
    void flashEraseAll();
    void flashUnlock();
    bool writeFiFo(uint32_t addr, unsigned char *data, uint32_t length);
    bool writeFlashBlk(uint32_t addr, unsigned char *data, uint32_t length);

    void sectorErase(uint32_t addr);
    void flashWrite(string filePath); // This one does the while flash writing thing.
    void activate();

private:
    uint32_t m_delayTimeMS;

    Serial *m_pSerial;

    unsigned char *sws_wr_addr(uint32_t addr, unsigned char *data, uint32_t length);

    Logger *m_pLogger;
};


#endif