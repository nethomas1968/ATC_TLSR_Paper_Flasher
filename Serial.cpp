
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
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

using namespace std;

#include "Logger.h"
#include "Serial.h"

unsigned baudMapping[][2] = {
    {0  , B0  },
    {50  , B50  },
    {75  , B75  },
    {110  , B110  },
    {134  , B134  },
    {150  , B150  },
    {200  , B200  },
    {300  , B300  },
    {600  , B600  },
    {1200  , B1200  },
    {1800  , B1800  },
    {2400  , B2400  },
    {4800  , B4800  },
    {9600  , B9600  },
    {19200  , B19200  },
    {38400  , B38400  },
    {57600  , B57600  },
    {115200  , B115200  },
    // {128000  , B128000  },
    {230400  , B230400  },
    // {256000  , B256000  },
    {460800  , B460800 },
};
int baudMappingCount = sizeof(baudMapping) / sizeof(baudMapping[0]);

Serial::Serial(uint32_t baudrate, string comPort)
{
    int k;

    m_InputBaudRate = baudrate;
    m_szComPort = comPort;
    m_iSerial = -1;
    m_pLogger = nullptr;

    bBaudIsOK = false;

    for(k = 0; k < baudMappingCount; k++) {
        if(baudMapping[k][0] == m_InputBaudRate) {
            m_BaudRate = baudMapping[k][1];
            bBaudIsOK = true;
            break;
        }
    }
        
    if (k == baudMappingCount) bBaudIsOK = false;
}

bool Serial::openSerialPort()
{
    bool bRet = true;

    //m_iSerial = open(m_szComPort.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    //m_iSerial = open(m_szComPort.c_str(), O_RDWR | O_NONBLOCK);
    m_iSerial = open(m_szComPort.c_str(), O_RDWR);
    //m_iSerial = open(m_szComPort.c_str(), O_RDWR | O_NOCTTY);

    if (m_iSerial < 0) bRet = false;

    return bRet;
}

void Serial::setupSerialPort()
{   
    struct termios  options; // Get the current options 
    uint32_t speed;
    
    tcgetattr(m_iSerial, &options);

    cfmakeraw(&options);
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 1;

    cfsetispeed(&options, m_BaudRate);
    speed = cfgetispeed(&options);
    if (speed != m_BaudRate) {
        if (m_pLogger != nullptr) *m_pLogger << "Set tty input to speed " << speed << " expected " << m_BaudRate << endl;
    }
    cfsetospeed(&options, m_BaudRate);
    speed = cfgetospeed(&options);
    if (speed != m_BaudRate) {
        if (m_pLogger != nullptr) *m_pLogger << "Set tty output to speed " << speed << " expected " << m_BaudRate << endl;
    }

    /*Clear the line */
    tcflush(m_iSerial, TCIFLUSH);

    if (tcsetattr(m_iSerial, TCSANOW, &options) != 0) {
        if (m_pLogger != nullptr) *m_pLogger << "Error setting serial attributes." << endl;
        restoreSerial();
    }
    tcflush(m_iSerial, TCIFLUSH);

    // DTR and RTS.
    int RTS_flag, DTR_flag;
    RTS_flag = TIOCM_RTS;
    DTR_flag = TIOCM_DTR;
    ioctl(m_iSerial, TIOCMBIC, &RTS_flag); //Set RTS pin   C = ON
    ioctl(m_iSerial, TIOCMBIC, &DTR_flag);
}

void Serial::restoreSerial()
{
    // Needed?
}


void Serial::reset(uint32_t delay)
{
    // DTR isn't connected anyway!

    // DTR and RTS low, then high.
    int RTS_flag, DTR_flag;

    if (m_pLogger != nullptr) *m_pLogger << "Serial::reset START" << endl;

    RTS_flag = TIOCM_RTS;
    DTR_flag = TIOCM_DTR;
    ioctl(m_iSerial, TIOCMBIS, &RTS_flag);// Clear RTS pin   S = OFF.
    ioctl(m_iSerial, TIOCMBIS, &DTR_flag);
    
    usleep(delay*1000);

    ioctl(m_iSerial, TIOCMBIC, &RTS_flag); //Set RTS pin   C = ON
    ioctl(m_iSerial, TIOCMBIC, &DTR_flag);

    if (m_pLogger != nullptr) *m_pLogger << "Serial::reset END" << endl;
}

void Serial::NOTreset(uint32_t delay)
{
    // DTR isn't connected anyway!

    // DTR and RTS low, then high.
    int RTS_flag, DTR_flag;

    RTS_flag = TIOCM_RTS;
    DTR_flag = TIOCM_DTR;
    ioctl(m_iSerial, TIOCMBIC, &RTS_flag);// Clear RTS pin   S = OFF.
    ioctl(m_iSerial, TIOCMBIC, &DTR_flag);
    
    usleep(delay*1000);

    ioctl(m_iSerial, TIOCMBIS, &RTS_flag); //Set RTS pin   C = ON
    ioctl(m_iSerial, TIOCMBIS, &DTR_flag);

    //usleep(delay*1000);
}