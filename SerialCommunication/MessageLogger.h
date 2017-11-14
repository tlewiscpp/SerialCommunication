#ifndef PROJECTTEMPLATE_MESSAGELOGGER_H
#define PROJECTTEMPLATE_MESSAGELOGGER_H

#include <sstream>

#include <string>
#include <functional>
#include <ctime>
#include <iomanip>
#include <regex>

namespace TMessageLogger {

enum class LogLevel {
    Debug,
    Info,
    Warn,
    Fatal
};

struct LogContext
{
    const char *fileName;
    const char *functionName;
    int sourceFileLine;
};

class LogMessage;
using LogFunction = std::function<void(LogLevel, LogContext, const std::string &)>;

class MessageLogger
{
    friend class LogMessage;
public:
    MessageLogger(const MessageLogger &) = delete;
    MessageLogger(MessageLogger &&) = delete;
    MessageLogger &operator=(const MessageLogger &) = delete;
    MessageLogger &operator=(MessageLogger &&) = delete;

    static void initializeInstance();
    static LogFunction initializeInstance(const LogFunction &logHandler);
    static LogFunction installLogHandler(const LogFunction &logHandler);

private:
    std::function<void(LogLevel, LogContext, const std::string &)> m_logHandler;

    MessageLogger();
    static void log(const LogMessage &logger);

};

extern MessageLogger *messageLogger;


class LogMessage
{
public:
    inline ~LogMessage() {
        MessageLogger::log(*this);
    }

    static inline LogMessage createInstance(LogLevel logLevel, const char *fileName, int sourceFileLine, const char *functionName) {
        return LogMessage{logLevel, fileName, sourceFileLine, functionName};
    }


    template <typename T>
    inline LogMessage &operator<<(const T &t) {
        this->m_logMessage += toStdString(t);
        return *this;
    }

    inline const LogLevel &logLevel() const { return this->m_logLevel; }
    inline const std::string &logMessage() const { return this->m_logMessage; }
    inline const LogContext &logContext() const {return this->m_logContext; }

private:
    LogLevel m_logLevel;
    std::string m_logMessage;
    LogContext m_logContext;

    template <typename T> static inline std::string toStdString(T t) { return dynamic_cast<std::ostringstream &>(std::ostringstream{} << t).str(); }

    template <typename T>
    inline LogMessage(LogLevel logLevel, const char *fileName, int sourceFileLine, const char *functionName, const T &t) :
        m_logLevel{logLevel},
        m_logMessage{toStdString(t)},
        m_logContext{} {
        this->m_logContext.fileName = fileName;
        this->m_logContext.sourceFileLine = sourceFileLine;
        this->m_logContext.functionName = functionName;
    }

    inline LogMessage(LogLevel logLevel, const char *fileName, int sourceFileLine, const char *functionName) :
            m_logLevel{logLevel},
            m_logMessage{""},
            m_logContext{} {
        this->m_logContext.fileName = fileName;
        this->m_logContext.sourceFileLine = sourceFileLine;
        this->m_logContext.functionName = functionName;
    }


};

template <typename T> std::string toTStringFormatString(T t) { return dynamic_cast<std::ostringstream &>(std::ostringstream{} << t).str(); }

/*Base case to break recursion*/
inline std::string TStringFormat(const char *formatting) { return formatting; }

/*C# style String.Format()*/
template <typename First, typename ... Args>
std::string TStringFormat(const char *formatting, First &&first, Args&& ... args)
{
    /* Match exactly one opening brace, one or more numeric digit,
    * then exactly one closing brace, identifying a token */
    static const std::regex targetRegex{R"(\{[0-9]+\})"};
    std::smatch match;

    /* Copy the formatting string to a std::string, to
    * make for easier processing, which will eventually
    * be used (the .c_str() method) to pass the remainder
    * of the formatting recursively */
    std::string returnString{formatting};

    /* Copy the formatting string to another std::string, which
    * will get modified in the regex matching loop, to remove the
    * current match from the string and find the next match */
    std::string copyString{formatting};

    /* std::tuple to hold the current smallest valued brace token,
    * wrapped in a std::vector because there can be multiple brace
    * tokens with the same value. For example, in the following format string:
    * "There were {0} books found matching the title {1}, {0}/{2}",
    * this pass will save the locations of the first and second {0} */
    using TokenInformation = std::tuple<int, size_t, size_t>;
    std::vector<TokenInformation> smallestValueInformation{std::make_tuple(-1, 0, 0)};

    /*Iterate through string, finding position and lengths of all matches {x}*/
    while(std::regex_search(copyString, match, targetRegex)) {
        /*Get the absolute position of the match in the original return string*/
        size_t foundPosition{match.position() + (returnString.length() - copyString.length())};
        int regexMatchNumericValue{0};
        /*Convert the integer value between the opening and closing braces to an int to compare */
        regexMatchNumericValue = std::stoi(returnString.substr(foundPosition + 1, (foundPosition + match.str().length())));

        /*Do not allow negative numbers, although this should never get picked up the regex anyway*/
        if (regexMatchNumericValue < 0) {
            throw std::runtime_error(TStringFormat("ERROR: In TStringFormat() - Formatted string is invalid (formatting = {0})", formatting));
        }
        /* If the numeric value in the curly brace token is smaller than
        * the current smallest (or if the smallest value has not yet been set,
        * ie it is the first match), set the corresponding smallestX variables
        * and wrap them up into a TokenInformation and add it to the std::vector */
        int smallestValue{std::get<0>(smallestValueInformation.at(0))};
        if ((smallestValue == -1) || (regexMatchNumericValue < smallestValue)) {
            smallestValueInformation.clear();
            smallestValueInformation.push_back(std::make_tuple(regexMatchNumericValue,
                                                               foundPosition,
                                                               match.str().length()));
        } else if (regexMatchNumericValue == smallestValue) {
            smallestValueInformation.push_back(std::make_tuple(regexMatchNumericValue,
                                                               foundPosition,
                                                               match.str().length()));
        }
        copyString = match.suffix();
    }
    int smallestValue{std::get<0>(smallestValueInformation.at(0))};
    if (smallestValue == -1) {
        throw std::runtime_error(TStringFormat("ERROR: In TStringFormat() - Formatted string is invalid (formatting = {0})", formatting));
    }
    /* Set the returnString to be up to the brace token, then the string
    * representation of current argument in line (first), then the remainder
    * of the format string, effectively removing the token and replacing it
    * with the requested item in the final string, then pass it off recursively */

    std::string firstString{toTStringFormatString(first)};
    int index{0};
    for (const auto &it : smallestValueInformation) {
        size_t smallestValueLength{std::get<2>(it)};

        /* Since the original string will be modified, the adjusted position must be
        calculated for any repeated brace tokens, kept track of by index.
        The length of string representation of first mutiplied by which the iterationn count
        is added, and the length of the brace token multiplied by the iteration count is
        subtracted, resulting in the correct starting position of the current brace token */
        size_t lengthOfTokenBracesRemoved{index * smallestValueLength};
        size_t lengthOfStringAdded{index * firstString.length()};
        size_t smallestValueAdjustedPosition{std::get<1>(it) + lengthOfStringAdded - lengthOfTokenBracesRemoved};
        returnString = returnString.substr(0, smallestValueAdjustedPosition)
                       + firstString
                       + returnString.substr(smallestValueAdjustedPosition + smallestValueLength);
        index++;
    }
    return TStringFormat(returnString.c_str(), std::forward<Args>(args)...);
}

} //namespace TMessageLogger


#endif //PROJECTTEMPLATE_MESSAGELOGGER_H
