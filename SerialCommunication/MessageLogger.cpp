#include "MessageLogger.h"

#include <sstream>
#include <iostream>

namespace TMessageLogger {

MessageLogger *messageLogger{nullptr};

namespace {
    void defaultLogFunction(LogLevel logLevel, LogContext logContext, const std::string &str) {
        std::ostream *outputStream{nullptr};
        switch (logLevel) {
            case LogLevel::Debug:
                outputStream = &std::clog;
                break;
            case LogLevel::Info:
                outputStream = &std::cout;
                break;
            case LogLevel::Warn:
                outputStream = &std::cout;
                break;
            case LogLevel ::Fatal:
                outputStream = &std::cerr;
                break;
            default:
                outputStream = &std::cout;
                break;
        }
        *outputStream << str << std::endl;
    }
} //Global namespace

MessageLogger::MessageLogger() :
        m_logHandler{defaultLogFunction}
{ }



void MessageLogger::initializeInstance()
{
    if (messageLogger == nullptr) {
        messageLogger = new MessageLogger{};
    }
}


LogFunction MessageLogger::initializeInstance(const LogFunction &logHandler)
{
    if (messageLogger == nullptr) {
        messageLogger = new MessageLogger{};
    }
    LogFunction function{messageLogger->m_logHandler};
    messageLogger->installLogHandler(logHandler);
    return function;
}

LogFunction MessageLogger::installLogHandler(const LogFunction &logHandler) {
    auto tempLogger = messageLogger->m_logHandler;
    messageLogger->m_logHandler = logHandler;
    return tempLogger;
}

void MessageLogger::log(const LogMessage &logger) {
    messageLogger->m_logHandler.operator()(logger.logLevel(), logger.logContext(), logger.logMessage());
}


} //namespace TMessageLogger;