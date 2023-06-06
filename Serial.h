#ifndef _SERIAL_H_
#define _SERIAL_H_


class Serial 
{
public:
    Serial(uint32_t baudrate, string comPort);

    void setLogger(Logger* l) { m_pLogger = l; }

    bool openSerialPort();
    void setupSerialPort();
    void closeSerialPort() { close(m_iSerial); }

    void restoreSerial();

    bool isBaudRateValid() { return bBaudIsOK; }
    uint32_t getBaudRate() { return m_BaudRate; }
    int getSerial() { return m_iSerial; }

    void reset(uint32_t delay);
    void NOTreset(uint32_t delay);


private:
    uint32_t m_BaudRate;
    uint32_t m_InputBaudRate;

    bool bBaudIsOK;
    int m_iSerial;
    string m_szComPort;

    Logger *m_pLogger;

};


#endif