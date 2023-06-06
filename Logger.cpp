
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#include "Logger.h"

using namespace std;

Logger::Logger(string filename)
{
    m_log_filename = filename;

    m_IsValid = false;
    m_bShowDateTime = false;
    m_bLogToConsole = false;
}

Logger::~Logger()
{
}

void Logger::enable(bool bShowDateTime)
{
    if (m_log_filename != "") {
        m_IsValid = true;
        m_bShowDateTime = bShowDateTime;
    }
}

void Logger::disable()
{
    m_IsValid = false;
}

void Logger::refreshFile()
{
    if (m_IsValid == true && m_log_filename != "") {
        if (true == checkFileExists(m_log_filename)) {
           // Empty the file.
           remove(m_log_filename.c_str());
        }
    }
}

void Logger::doLog(string text)
{
    if (m_IsValid == true && m_log_filename != "") {
        ofstream myfile;
        myfile.open( m_log_filename.c_str(), ios::app );
        if (m_bShowDateTime) {
            myfile << getCurrentDateTime() << ":";
        }
        myfile << text << endl;
        myfile.close();

        if (m_bLogToConsole == true) cout << text << endl;
    }
}

void Logger::doLog(const char *text)
{
    string s = text;
    doLog(s);
}

///////////////////

bool Logger::checkFileExists(string filename)
{
    ifstream infile(filename.c_str());
    return infile.good();
}

const string Logger::getCurrentDateTime()
{
    time_t     now = time(0);
    tm*        now_tm = gmtime(&now);
    char       buf[80];
  
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
  
    // UTC
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", now_tm);

    return buf;
}

///////////////////
