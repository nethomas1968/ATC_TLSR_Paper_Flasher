
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <unistd.h>
#include <vector>

using namespace std;

class Logger {
private:

  std::stringstream m_out_stream;

public:
  bool m_IsValid;
  std::string m_log_filename;

  Logger(std::string filename);	
  ~Logger();
  
  // Overload for std::endl only.
  Logger& operator<<(ostream& (*fun)(ostream&)) {
    doLog(m_out_stream.str());
    m_out_stream.str(string());
    return *this;
  }

  //Overloads
  Logger& operator<<(string s) { m_out_stream << s; return *this; }
  Logger& operator<<(uint32_t i) { m_out_stream << to_string(i); return *this; }
  Logger& operator<<(long l) { m_out_stream << to_string(l); return *this; }
  Logger& operator<<(unsigned long l) { m_out_stream << to_string(l); return *this; }
  Logger& operator<<(int i) { m_out_stream << to_string(i); return *this; }
  Logger& operator<<(double d) { m_out_stream << to_string(d); return *this; }

  void enable(bool bShowDateTime);
  void disable();
  void refreshFile();
  void doLog(string text);
  void doLog(const char *text);
  void logToConsole(bool b) { m_bLogToConsole = b; }

  void setLogLevel(uint8_t l) { m_LogLevel = l; }
  uint8_t getLogLevel() { return m_LogLevel; }

private:
  bool checkFileExists(string filename);
  const string getCurrentDateTime();
  
  bool m_bShowDateTime;
  bool m_bLogToConsole;
  uint8_t m_LogLevel;
};

#endif
