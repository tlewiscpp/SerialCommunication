#ifndef PROJECTTEMPLATE_GLOBALDEFINITIONS_H
#define PROJECTTEMPLATE_GLOBALDEFINITIONS_H
#include "MessageLogger.h"

const char * const PROGRAM_NAME{"SerialCommunication"};
const char * const COMPANY_NAME{"Tyler Lewis"};
const char * const PROGRAM_LONG_NAME{"SerialCommunication"};
const char * const PROGRAM_DESCRIPTION{"Communication with RS232 serial ports"};
const char * const REMOTE_URL{"https://github.com/tlewiscpp/"};
const char * const AUTHOR_NAME{"Tyler Lewis"};
const int SOFTWARE_MAJOR_VERSION{0};
const int SOFTWARE_MINOR_VERSION{1};
const int SOFTWARE_PATCH_VERSION{0};

#if defined(__GNUC__)
const char * const COMPILER_NAME{"g++"};
const int COMPILER_MAJOR_VERSION{__GNUC__};
const int COMPILER_MINOR_VERSION{__GNUC_MINOR__};
const int COMPILER_PATCH_VERSION{__GNUC_PATCHLEVEL__};
#elif defined(_MSC_VER)
const char *COMPILER_NAME{"msvc"};
        const int COMPILER_MAJOR_VERSION{_MSC_VER};
        const int COMPILER_MINOR_VERSION{0};
        const int COMPILER_PATCH_VERSION{0};
    #else
        const char *COMPILER_NAME{"unknown"};
        const int COMPILER_MAJOR_VERSION{0};
        const int COMPILER_MINOR_VERSION{0};
        const int COMPILER_PATCH_VERSION{0};
    #endif


#ifndef LOG_DEBUG
#    define LOG_DEBUG(x) logMessage::createInstance(LogLevel::Debug, __FILE__, __LINE__, __func__)
#endif //LOG_FATAL
#ifndef LOG_WARN
#    define LOG_WARN(x) LogMessage::createInstance(LogLevel::Debug, __FILE__, __LINE__, __func__)
#endif //LOG_WARN
#ifndef LOG_INFO
#    define LOG_INFO(x) LogMessage::createInstance(LogLevel::Info, __FILE__, __LINE__, __func__)
#endif //LOG_INFO
#ifndef LOG_FATAL
#    define LOG_FATAL(x) LogMessage::createInstance(LogLevel::Fatal, __FILE__, __LINE__, __func__)
#endif //LOG_FATAL

#ifndef STRING_TO_INT
#    if defined(__ANDROID__)
#        define STRING_TO_INT(x) std::atoi(x.c_str())
#    else
#        define STRING_TO_INT(x) std::stoi(x)
#    endif
#endif //STRING_TO_INT

#ifndef INT_TO_STRING
#   define INT_TO_STRING(x) std::to_string(x)
#endif //INT_TO_STRING


#endif //PROJECTTEMPLATE_GLOBALDEFINITIONS_H
