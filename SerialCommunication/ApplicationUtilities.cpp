
#include "ApplicationUtilities.h"
#include "GlobalDefinitions.h"
#include <csignal>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <forward_list>
#include <fstream>


namespace ApplicationUtilities
{

using namespace TMessageLogger;

bool verboseLogging{false};

bool startsWith(const std::string &str, const std::string &start)
{
    if (str.length() < start.length()) {
        return false;
    }
    return std::equal(start.begin(), start.end(), str.begin());
}

bool endsWith(const std::string &str, const std::string &ending)
{
    if (str.length() < ending.length()) {
        return false;
    }
    return std::equal(ending.rbegin(), ending.rend(), str.rbegin());
}

void toLower(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void toUpper(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void displayVersion()
{
    LOG_INFO() << TStringFormat("{0}, v{1}.{2}.{3}", PROGRAM_NAME, SOFTWARE_MAJOR_VERSION, SOFTWARE_MINOR_VERSION, SOFTWARE_PATCH_VERSION);
    LOG_INFO() << TStringFormat("Written by {0}", AUTHOR_NAME);
    LOG_INFO() << TStringFormat("Built with {0} v{1}.{2}.{3}, {4}", COMPILER_NAME, COMPILER_MAJOR_VERSION, COMPILER_MINOR_VERSION, COMPILER_PATCH_VERSION, __DATE__);
}

void displayHelp()
{
    std::cout << TStringFormat("Usage: {0} Option [=value]", PROGRAM_NAME) << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "    -h, --help: Display this help text" << std::endl;
    std::cout << "    -v, --version: Display the version" << std::endl;
    std::cout << "    -e, --verbose: Enable verbose output" << std::endl;
    std::cout << "    -p, --port: Set the port (Ex: /dev/ttyUSB0)" << std::endl;
    std::cout << "    -b, --baud-rate: Set the baud rate (Ex: 115200)" << std::endl;
    std::cout << "    -s, --stop-bits: Set the stop bits (Ex: 1)" << std::endl;
    std::cout << "    -d, --data-bits: Set the data bits (Ex: 8)" << std::endl;
    std::cout << "    -a, --parity: Set the parity (Ex: even)" << std::endl;
    std::cout << "    -n, --line-ending: Set the line ending (Ex: \\n)" << std::endl;
}


void globalLogHandler(LogLevel logLevel, LogContext logContext, const std::string &str)
{
    using namespace ApplicationUtilities;
    std::string logPrefix{""};
    auto *outputStream = &std::cout;
    switch (logLevel) {
        case LogLevel::Debug:
            if (!ApplicationUtilities::verboseLogging) {
                return;
            }
            logPrefix = "{  Debug }: ";
            outputStream = &std::clog;
            break;
        case LogLevel::Info:
            logPrefix = "{  Info  }: ";
            outputStream = &std::cout;
            break;
        case LogLevel::Warn:
            logPrefix = "{  Warn  }: ";
            outputStream = &std::cout;
            break;
        case LogLevel::Fatal:
            logPrefix = "{  Fatal }: ";
            outputStream = &std::cerr;
    }
    std::string logMessage{""};
    std::string coreLogMessage{str};
    if (coreLogMessage.find('\"') == 0) {
        coreLogMessage = coreLogMessage.substr(1);
    }
    if (coreLogMessage.find_last_of('\"') == coreLogMessage.length() - 1) {
        coreLogMessage = coreLogMessage.substr(0, coreLogMessage.length() - 1);
    }
    if (logLevel == LogLevel::Fatal) {
        std::string logTime{currentTime()};
        logTime.replace(logTime.begin(), logTime.end(), '-', ':');
        logMessage = TStringFormat("[{0}] - {1} {2} ({3}:{4}, {5})", currentTime(), logPrefix, coreLogMessage, logContext.fileName, logContext.sourceFileLine, logContext.functionName);
    } else {
        logMessage = TStringFormat("[{0}] - {1} {2}", currentTime(), logPrefix, coreLogMessage);
    }


    bool addLineEnding{true};
    static const std::forward_list<const char *> LINE_ENDINGS{"\r\n", "\r", "\n", "\n\r"};
    for (const auto &it : LINE_ENDINGS) {
        if (endsWith(logMessage, it)) {
            addLineEnding = false;
        }
    }
    if (addLineEnding) {
        logMessage.append("\n");
    }
    if (outputStream) {
        *outputStream << logMessage;
    }
    if (logLevel != LogLevel::Fatal) {
        logToFile(logMessage, ApplicationUtilities::getLogFilePath());
    }
    outputStream->flush();
    if (logLevel == LogLevel::Fatal) {
        abort();
    }
}


    std::string getTempDirectory() {
#ifdef _MSC_VER
        //return someBullShit;
        return "/tmp";
#else
        return TStringFormat("/tmp/{0}", PROGRAM_NAME);
#endif
    }

    std::string getLogFilePath() {
        static std::string logFileName{""};
        if (logFileName.length() > 0) {
            return logFileName;
        } else {
            auto mkdirResult = createDirectory(getTempDirectory());
            if ( (mkdirResult == -1) && (errno != EEXIST) ) {
                throw std::runtime_error(TStringFormat("Unable to create directory {0}", getTempDirectory()));
            }
            return TStringFormat("{0}/{1}_{2}_{3}", getTempDirectory(), PROGRAM_NAME, currentDate, currentTime());
        }
    }

void installSignalHandlers(void (*signalHandler)(int))
{
    static struct sigaction signalInterruptHandler;
    signalInterruptHandler.sa_handler = signalHandler;
    sigemptyset(&signalInterruptHandler.sa_mask);
    signalInterruptHandler.sa_flags = 0;
    sigaction(SIGHUP, &signalInterruptHandler, NULL);
    sigaction(SIGINT, &signalInterruptHandler, NULL);
    sigaction(SIGQUIT, &signalInterruptHandler, NULL);
    sigaction(SIGILL, &signalInterruptHandler, NULL);
    sigaction(SIGABRT, &signalInterruptHandler, NULL);
    sigaction(SIGFPE, &signalInterruptHandler, NULL);
    sigaction(SIGPIPE, &signalInterruptHandler, NULL);
    sigaction(SIGALRM, &signalInterruptHandler, NULL);
    sigaction(SIGTERM, &signalInterruptHandler, NULL);
    sigaction(SIGUSR1, &signalInterruptHandler, NULL);
    sigaction(SIGUSR2, &signalInterruptHandler, NULL);
    sigaction(SIGCHLD, &signalInterruptHandler, NULL);
    sigaction(SIGCONT, &signalInterruptHandler, NULL);
    sigaction(SIGTSTP, &signalInterruptHandler, NULL);
    sigaction(SIGTTIN, &signalInterruptHandler, NULL);
    sigaction(SIGTTOU, &signalInterruptHandler, NULL);
}

std::vector<std::string> split(std::string str, const std::string &delimiter)
{
    std::vector<std::string> returnContainer;
    auto splitPosition = str.find(delimiter);
    if (splitPosition == std::string::npos) {
        returnContainer.push_back(str);
        return returnContainer;
    }
    while (splitPosition != std::string::npos) {
        returnContainer.push_back(str.substr(0, splitPosition));
        str = str.substr(splitPosition + delimiter.size());
        splitPosition = str.find(delimiter);
    }
    return returnContainer;
}

int createDirectory(const std::string &filePath, mode_t mode)
{
    return mkdir(filePath.c_str(), mode);
}

int deleteFile(const std::string &filePath)
{
    return std::remove(filePath.c_str());
}

std::string currentTime() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    return dynamic_cast<std::ostringstream &>(std::ostringstream{} << std::put_time(&tm, "%H-%M-%S")).str();
}

std::string currentDate() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    return dynamic_cast<std::ostringstream &>(std::ostringstream{} << std::put_time(&tm, "%d-%m-%Y")).str();
}

} //namespace ApplicationUtilities