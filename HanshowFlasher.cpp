
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
#include "Serial.h"
#include "MCUTools.h"
#include "FileUtils.h"
#include "TelinkFile.h"

#define SHOW_DATE_TIME true
#define NOSHOW_DATE_TIME false

unsigned char buf[512];

static void usage(const char *progname)
{
    cout << endl;
    cout << progname << ": A command line tool to help with flashing the Hanshow Nebular device." << endl;
    cout << "Usage: " << progname << " [OPTIONS] -b460800 -c/dev/ttyUSB3 [-f./filename.bin] [-t500] [-r] [-d1] [-q] [-h]" << endl << endl;
    cout << " -b115200 (required) Set baud rate to 115200." << endl;
    cout << " -c/dev/ttyUSB3 (required) The COM port to use." << endl;
    cout << " -f./filename.bin (optional) The file to upload to the device." << endl;
    cout << " -t500 (optional) Set delay time to 500ms. Default is already 500ms." << endl;
    cout << " -r (optional) Reset MCU." << endl;
    cout << " -u (optional) Unlock flash" << endl;
    cout << " -d1 (optional) Set debug level to 1" << endl;
    cout << " -h (optional) Show this help and exit." << endl << endl;
}

/***********************************************************************
 * You are here !!!
 ***********************************************************************/
int main(int argc, char** argv)
{
    int option;
    uint8_t debugLevel = 0;
    uint32_t inputBaudRate = 0;
    uint32_t delayTimeMS = 500; // half a second delay time.
    bool bOK = true;
    string binFileFullPath;
    string comPort;
    bool bSoftResetMCU;
    bool bShowHelp = false;
    bool bUnlockFlash = false;
    Logger log("./log.txt");
    
    while ((option = getopt(argc, argv,"d:b:t:f:c:ruh")) != -1)
    {
        switch (option) {
            case 'd' : debugLevel = (uint8_t)atoi(optarg); break;
            case 'b' : inputBaudRate = (uint32_t) atoi(optarg); break;
            case 't' : delayTimeMS = (uint32_t) atoi(optarg); break;
            case 'f' : binFileFullPath = optarg; break;   // optional
            case 'c' : comPort = optarg; break;
            case 'r' : bSoftResetMCU = true; break;           // optional
            case 'u' : bUnlockFlash = true; break;           // optional
            case 'h' : bShowHelp = true; break;
            default: usage(argv[0]);
            return -1;
        }
    }

    // -h option, just show help and exit.
    if (bShowHelp) {
        usage(argv[0]);
        return 0;
    }
    
    if (debugLevel > 0) log.enable(NOSHOW_DATE_TIME); // Logger must be enabled before use. Use "true" to see date/time in the log file.

    log << argv[0] << ": START" << endl;

    log.logToConsole(true);
    log << argv[0] << " debugLevel = " << (uint32_t) debugLevel << endl;
    
    // Check baud rate and Com Port. We can't do much without those.
    if (inputBaudRate == 0) {
        // No baud rate set.
        log << "Error, no baud rate set." << endl;
        bOK = false;
    }

    // Check COM port.
    if (comPort == "") {
        log << "Error, no COM port specified." << endl;
        bOK = false;
    }

    if (bOK == true) {
        // ComPort and baud rate have been chosen, now check them.
        Serial sp(inputBaudRate, comPort);
        sp.setLogger(&log);

        if (sp.isBaudRateValid() == false) {
            log << "Error, bad baud rate specified." << endl;
            bOK = false;
        }    

        if (bOK == true) {
            /* That's the mandatory options checked, now we need to check if there is an action to perform.
            * We can do one of:
            *  1) Soft Reset the MCU
            *  2) Flash a bin file.
            *  3) Unlock flash.
            */

            // Check for an action of some kind.
            if (binFileFullPath == "" && bSoftResetMCU == false && bUnlockFlash == false) {
                // There is nothing to do!
                log << "Error, no action specified, nothing to do." << endl;
                bOK = false;
            }
            
            // Settings OK, proceed.
            if (bOK == true) {
                log << "Using baud rate = " << inputBaudRate << " (" << sp.getBaudRate() << ") " << endl;
                if (binFileFullPath != "") {
                    log << "Using file = " << binFileFullPath << endl;
                }
                log << "COM port = " << comPort << endl;
                log << "Proceeding." << endl;
                log << "delayTimeMS = " << (uint32_t)delayTimeMS << "ms" << endl;

                // What are we doing..
                if (binFileFullPath != "") {
                    log << "Programming bin file to flash." << endl;
                }
                if (bSoftResetMCU == true) {
                    log << "Soft reset MCU." << endl;
                }
                if (bUnlockFlash == true) {
                    log << "Unlocking flash." << endl;
                }

                // Opening serial port.
                sp.openSerialPort();
                
                if (sp.getSerial() == -1) {
                    log << "Can't open serial port " << comPort << endl;
                    bOK = false;
                }
                else {
                    log << "Serial port opened OK." << endl;

                    sp.setupSerialPort();    

                    log << "Setup done" << endl;

                    MCUTools mcu(&sp, delayTimeMS);
                    mcu.setLogger(&log);
                    
                    if (bSoftResetMCU == true) {
                        usleep(delayTimeMS*1000);
                        mcu.softResetMCU();
                        //usleep(delayTimeMS*1000);
                    }
                    else if (binFileFullPath != "") {
                        log << "Using bin file:" << binFileFullPath << endl;
                        TelinkFile fu(binFileFullPath);
                        if (fu.fileExists()) {
                            log << "Files exists and size is " << fu.getFileSize() << " bytes" << endl;

                            if (true == fu.checkSignature()) {
                                log << "Signature is correct." << endl;

                                mcu.flashWrite(binFileFullPath);
                            }
                            else log << "The bin file is not a valid telink firmware file!" << endl; 
                        }
                        else log << "Can't find the bin file!" << endl;
                    }
                    else if (bUnlockFlash == true) {
                        log << "Unlocking flash" << endl;
                        mcu.activate();
                        mcu.flashWriteEnable();
                        mcu.flashUnlock();
                        sleep(3);
                    }
                    else log << "Error, nothing to do!" << endl;
                    
                    sp.closeSerialPort();
                    log << "Serial port closed." << endl;
                }
            }
        }
    }

    if (!bOK) usage(argv[0]);

    log.logToConsole(false);
    log << argv[0] << ": END" << endl;
    
    return 0;
}

