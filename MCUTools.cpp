#include <iostream>
#include <cstdlib>
#include <sstream>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <stdlib.h>
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <errno.h>   /* Error number definitions */
#include <algorithm>
#include <sys/types.h>
#include <sys/time.h>

using namespace std;

#include "Logger.h"
#include "TelinkFile.h"
#include "Serial.h"
#include "MCUTools.h"

MCUTools::MCUTools(Serial *s, uint32_t delay)
{
    m_pSerial = s;
    m_delayTimeMS = delay;
    m_pLogger = nullptr;
}

void MCUTools::flashWriteEnable() // send flash cmd 0x06 write enable to flash
{
    if (m_pLogger != nullptr && m_pLogger->getLogLevel() > 0) *m_pLogger << "flashWriteEnable" << endl;
	flashByteCmd(0x06);
}

void MCUTools::flashWakeUp() // send flash cmd 0xab to wakeup flash
{ 
    if (m_pLogger != nullptr && m_pLogger->getLogLevel() > 0) *m_pLogger << "flashWakeUp" << endl;
    flashByteCmd(0xab);
}

void MCUTools::flashEraseAll()
{
    if (m_pLogger != nullptr && m_pLogger->getLogLevel() > 0) *m_pLogger << "flashEraseAll" << endl;
    flashByteCmd(0x60);
}

bool MCUTools::writeFiFo(uint32_t addr, unsigned char *data, uint32_t length)
{
    // send all data to one register (no increment address - fifo mode)
    uint32_t numWritten = 0;
    uint32_t expectedLen = 0;
    unsigned char val;
    unsigned char *pkt;

    val = 0x80;
	pkt = sws_wr_addr(0x00b3, &val, 1); // [0xb3]=0x80 ext.SWS into fifo mode
    numWritten += (uint32_t)write(m_pSerial->getSerial(), pkt, (1+6)*10);
    expectedLen += ((1+6)*10);
    free(pkt);

	pkt = sws_wr_addr(addr, data, length); // send all data to one register (no increment address - fifo mode)
	numWritten += (uint32_t)write(m_pSerial->getSerial(), pkt, (length+6)*10);
    expectedLen += ((length+6)*10);
    free(pkt);

    val = 0x00;   
    pkt = sws_wr_addr(0x00b3, &val, 1); // [0xb3]=0x00 ext.SWS into normal(ram) mode
    numWritten += (uint32_t)write(m_pSerial->getSerial(), pkt, (1+6)*10);
    expectedLen += ((1+6)*10);
    free(pkt);

    return ((numWritten == expectedLen));
}

bool MCUTools::writeFlashBlk(uint32_t addr, unsigned char *data, uint32_t length)
{
    uint32_t numWritten = 0;
    unsigned char val[2];
    unsigned char *pkt;

    if (m_pLogger != nullptr && m_pLogger->getLogLevel() > 0) *m_pLogger << "writeFlashBlk" << endl;

	flashWriteEnable();

    val[0] = 0x00;
    pkt = sws_wr_addr(0x0d, val, 1); // cns low
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    unsigned char *blk = (unsigned char*) malloc(4 + length);
    if (blk != nullptr) {

    	blk[0] = 0x02;
	    blk[1] = (addr >> 16) & 0xff;
	    blk[2] = (addr >> 8) & 0xff;
	    blk[3] = addr & 0xff;
        memcpy(&blk[4], data, length);
	    
	    if (true == writeFiFo(0x0c, blk, (4+length))) { // send all data to SPI data register
            numWritten += length;
        } 
        free(blk);

        val[0] = 0x01;
        pkt = sws_wr_addr(0x0d, val, 1); // cns high
        write(m_pSerial->getSerial(), pkt, (1+6)*10);
        free(pkt);
        usleep(10*1000); 
    }
    else {
        if (m_pLogger != nullptr) *m_pLogger << "Error, malloc failed!" << endl;
    }

    return (numWritten == length);
}

void MCUTools::sectorErase(uint32_t addr)
{
    unsigned char data[2];
    unsigned char *pkt;

    if (m_pLogger != nullptr && m_pLogger->getLogLevel() > 0) *m_pLogger << "sectorErase" << endl;

	flashWriteEnable();

    data[0] = 0x00;
    pkt = sws_wr_addr(0x0d, data, 1); // cns low
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    data[0] = 0x20;
	pkt = sws_wr_addr(0x0c, data, 1); // Flash cmd erase sector
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    data[0] = (addr >> 16) & 0xff;
	pkt = sws_wr_addr(0x0c, data, 1); // Faddr hi
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    data[0] = (addr >> 8) & 0xff;
	pkt = sws_wr_addr(0x0c, data, 1); // Faddr mi
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    data[0] = addr & 0xff;
    data[1] = 0x01;
	pkt = sws_wr_addr(0x0c, data, 2); // Faddr lo + cns high
    write(m_pSerial->getSerial(), pkt, (2+6)*10);
    free(pkt);

	usleep(300*1000);
}

void MCUTools::flashUnlock()
{ 
    // send flash cmd 0x01 unlock flash
    unsigned char data[2];
    unsigned char *pkt;

    if (m_pLogger != nullptr && m_pLogger->getLogLevel() > 0) *m_pLogger << "flashUnlock" << endl;

    data[0] = 0x00; // cns low
    pkt = sws_wr_addr(0x0d, data, 1);
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    data[0] = 0x01; // Flash cmd
    pkt = sws_wr_addr(0x0c, data, 1);
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    data[0] = 0x00; // Unlock all
    pkt = sws_wr_addr(0x0c, data, 1);
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    data[0] = 0x00; // Unlock all + cns high
    data[1] = 0x01;
    pkt = sws_wr_addr(0x0c, data, 2);
    write(m_pSerial->getSerial(), pkt, (2+6)*10);
    free(pkt);
}


void MCUTools::softResetMCU()
{
    unsigned char data = 0x20;

    if (m_pLogger != nullptr && m_pLogger->getLogLevel() > 0) *m_pLogger << "softResetMCU" << endl;

    if (m_pSerial->getSerial() >= 0) {
        unsigned char *pkt = sws_wr_addr(0x06f, &data, 1);  // Must be free'd when done.
        write(m_pSerial->getSerial(), pkt, (1+6)*10);
        free(pkt);
    }
}

void MCUTools::flashByteCmd(uint8_t cmd) { 
    unsigned char data[2];
	unsigned char *pkt;
    
    data[0] = 0x00;
    pkt = sws_wr_addr(0x0d, data, 1); // cns low
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    data[0] = cmd;
    data[1] = 0x01;
	pkt = sws_wr_addr(0x0c, data, 2); // Flash cmd + cns high
    write(m_pSerial->getSerial(), pkt, (2+6)*10);
    free(pkt);
}

#define MAX_BLOCK_SIZE 256
void MCUTools::flashWrite(string filePath)
{
    uint32_t binFileSize, len, sblk;
    uint32_t addr;
    uint32_t totalBytesWritten = 0;
    TelinkFile tf(filePath);
    unsigned char buf[MAX_BLOCK_SIZE];

    if (tf.fileExists()) {
        if (true == tf.checkSignature()) {
            activate();

            len = binFileSize = tf.getFileSize();
            addr = 0;
            sblk = MAX_BLOCK_SIZE; // max spi-flash fifo = 256.

            if (m_pLogger != nullptr) *m_pLogger << "Len = " << len << endl;

            while (len > 0) {
                if ((addr & 0x0FFF) == 0) {
                    if (m_pLogger != nullptr) *m_pLogger << "Erasing sector at " << addr << endl;
                    sectorErase(addr);
                }
                if (len < sblk) sblk = len;
                tf.getBytes(sblk, buf);
                if (m_pLogger != nullptr) *m_pLogger << "Writing " << sblk << " bytes at " << addr << endl;

                if (true == writeFlashBlk(addr, buf, sblk)) totalBytesWritten += sblk;
                addr += sblk;
                len -= sblk;
            }
            softResetMCU();

            if (m_pLogger != nullptr) *m_pLogger << "Total bytes expected = " << binFileSize << ". Total bytes written = " << totalBytesWritten << endl;
        }
    }
}

void MCUTools::activate()
{
    unsigned char data[2];
	unsigned char *pkt;
    struct timeval start, now;
    long int tDiff;
    uint32_t loopsDone = 0;

    if (m_pLogger != nullptr) *m_pLogger << "activate START" << endl;

    // DTR & RTS.
    m_pSerial->reset(100); // DTR & RTS.

    // 1 Soft Reset MSU.
    softResetMCU();

    gettimeofday(&start, NULL);

    data[0] = 0x05; // CPU Stop
    pkt = sws_wr_addr(0x0602, data, 1);

    do {
        // 2 CPU Stop.    
        //ssize_t num = write(m_pSerial->getSerial(), pkt, (1+6)*10);
        write(m_pSerial->getSerial(), pkt, (1+6)*10);
        gettimeofday(&now, NULL);
        tDiff = ((now.tv_sec - start.tv_sec) * 1000000 + now.tv_usec - start.tv_usec) / 1000;
        loopsDone++;
    } while (tDiff < m_delayTimeMS);

    if (m_pLogger != nullptr) *m_pLogger << "activate, CPU Stop done " << loopsDone << " times." << endl;

    free(pkt);

    // 3 Set SWS Speed.
    data[0] = 55;
    pkt = sws_wr_addr(0x00b2, data, 1); // Set SWS speed.
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    // 4 CPU Stop again.
    data[0] = 0x05; // CPU Stop
    pkt = sws_wr_addr(0x0602, data, 1); // cns low
    write(m_pSerial->getSerial(), pkt, (1+6)*10);
    free(pkt);

    // 5. Flash Wakeup.
    flashWakeUp();

    if (m_pLogger != nullptr) *m_pLogger << "activate END" << endl;
}


string MCUTools::toHex(uint32_t number, size_t length)
{
    stringstream stream;
    stream << hex << number;
    string result( stream.str() );
    auto new_str = string(length - min(length, result.length()), '0') + result;
    return new_str;
}

/*
 * NOTE: Caller must free the returned pointer.
 */
unsigned char *MCUTools::sws_wr_addr(uint32_t addr, unsigned char *data, uint32_t length)
{
    // log(addr + ':['+data+']');
    int l;
    unsigned char d[10]; // word swire 10 bits = 10 bytes UART
    unsigned char h[5];
    
    h[0] = 0x5a;
    h[1] = (addr>>16)&0xFF;
    h[2] = (addr>>8)&0xFF;
    h[3] = addr&0xFF;
    h[4] = 0x00;
    
    unsigned char *pkt = (unsigned char *)malloc((length+6)*10);
    
    d[0] = 0x80; // start bit byte cmd swire = 1
    d[9] = 0xfe; // stop bit swire = 0
  
    uint32_t k;
    unsigned char m = 0x80;
    uint32_t idx = 1;
    for (k = 0; k < 5; k++) {
        idx=1;
        m = 0x80;
        do {
            if ((h[k] & m) != 0) d[idx] = 0x80;
            else d[idx] = 0xfe;
            idx += 1;
            m = (unsigned char)(m >> 1);
        } while(m != 0);
        
        for (l = 0; l < 10; l++) {
            pkt[(k*10)+l] = d[l];
        }
        d[0] = 0xfe; // start bit next byte swire = 0
    }
    
    for (k = 0; k < length; k++) {
        m = 0x80;
        idx = 1;
        do {
            if ((data[k] & m) != 0) d[idx] = 0x80;
            else d[idx] = 0xfe;
            idx+=1;
            m = (unsigned char)(m >> 1);
        } while (m != 0);
        

        for (l = 0; l < 10; l++) {
            pkt[((k+5)*10)+l] = d[l];
        }
    }
  
    // swire stop cmd = 0xff 
    for (l = 0; l < 10; l++) { 
        d[l] = 0x80;
        pkt[((length+5)*10)+l] = 0x80;
    }
    
    return pkt;
}