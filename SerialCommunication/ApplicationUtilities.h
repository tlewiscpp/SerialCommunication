#ifndef PROJECTTEMPLATE_APPLICATIONUTILITIES_H
#define PROJECTTEMPLATE_APPLICATIONUTILITIES_H

#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <utility>
#include <tuple>
#include <fstream>
#include "MessageLogger.h"

namespace ApplicationUtilities
{
void installSignalHandlers(void (*signalHandler)(int));
std::string getLogFilePath();
extern bool verboseLogging;

bool startsWith(const std::string &str, const std::string &start);
bool endsWith(const std::string &str, const std::string &ending);
void toLower(std::string &str);
void toUpper(std::string &str);

void displayVersion();
void displayHelp();

std::string currentTime();
std::string currentDate();

bool endsWith(const std::string &str, const std::string &ending);
bool endsWith(const std::string &str, char ending);
void globalLogHandler(TMessageLogger::LogLevel logLevel, TMessageLogger::LogContext logContext, const std::string &str);

template <typename StringType, typename FileStringType>
void logToFile(const StringType &str, const FileStringType &filePath)
{
    using namespace ApplicationUtilities;
    std::ofstream writeToFile{};
    writeToFile.open(filePath.c_str(), std::ios::app);
    if (!writeToFile.is_open()) {
        throw std::runtime_error(TMessageLogger::TStringFormat(R"(Failed to log data "{0}" to file "{1}" (could not open file))", str, filePath));
    } else {
        writeToFile << str << std::endl;
        if (!writeToFile.good()) {
            throw std::runtime_error(TMessageLogger::TStringFormat(
                    R"(Failed to log data "{0}" to file "{1}" (file was opened, but not writable, permission problem?))", str, filePath));
        }
        writeToFile.close();
    }

}


template <typename T> static inline std::string toStdString(T t) { return dynamic_cast<std::ostringstream &>(std::ostringstream{} << t).str(); }

template <char Delimiter>
std::vector<std::string> split(const std::string &str)
{
    std::string tempString{""};
    std::vector<std::string> returnVector{};
    std::stringstream istr{str};
    for(std::string s{""}; std::getline(istr, s, Delimiter); ( (s.length() > 0) ?  returnVector.push_back(s) : (void)returnVector.size() ));
    return returnVector;
}

std::vector<std::string> split(std::string str, const std::string &delimiter);

int createDirectory(const std::string &filePath, mode_t mode = 0777);
int deleteFile(const std::string &filePath);

};


#endif //PROJECTTEMPLATE_APPLICATIONUTILITIES_H
