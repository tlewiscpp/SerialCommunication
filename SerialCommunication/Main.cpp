#include <iostream>

#include <CppSerialPort/SerialPort.h>
#include "MessageLogger.h"
#include "ApplicationUtilities.h"
#include "GlobalDefinitions.h"
#include <getopt.h>

using namespace CppSerialPort;
using namespace TMessageLogger;
using namespace ApplicationUtilities;

static const struct option longOptions[] {
        {"help",        no_argument,       nullptr, 'h'},
        {"verbose",     no_argument,       nullptr, 'e'},
        {"version",     no_argument,       nullptr, 'v'},
        {"port",        required_argument, nullptr, 'p'},
        {"baud-rate",   required_argument, nullptr, 'b'},
        {"stop-bits",   required_argument, nullptr, 's'},
        {"data-bits",   required_argument, nullptr, 'd'},
        {"parity",      required_argument, nullptr, 'a'},
        {"line-ending", required_argument, nullptr, 'n'},
        {0, 0, 0, 0}
};

static std::map<std::string, BaudRate> baudRateLookup {
        {"50", BaudRate::BAUD50},
        {"75", BaudRate::BAUD75},
        {"110", BaudRate::BAUD110},
        {"134", BaudRate::BAUD134},
        {"150", BaudRate::BAUD150},
        {"200", BaudRate::BAUD200},
        {"300", BaudRate::BAUD300},
        {"600", BaudRate::BAUD600},
        {"1200", BaudRate::BAUD1200},
        {"1800", BaudRate::BAUD1800},
        {"2400", BaudRate::BAUD2400},
        {"4800", BaudRate::BAUD4800},
        {"9600", BaudRate::BAUD9600},
        {"19200", BaudRate::BAUD19200},
        {"38400", BaudRate::BAUD38400},
        {"57600", BaudRate::BAUD57600},
        {"115200", BaudRate::BAUD115200},
        {"230400", BaudRate::BAUD230400},
        {"460800", BaudRate::BAUD460800},
        {"500000", BaudRate::BAUD500000},
        {"576000", BaudRate::BAUD57600},
        {"921600", BaudRate::BAUD921600},
        {"1000000", BaudRate::BAUD1000000},
        {"1152000", BaudRate::BAUD1152000},
        {"1500000", BaudRate::BAUD1500000},
        {"2000000", BaudRate::BAUD2000000},
        {"2500000", BaudRate::BAUD2500000},
        {"3000000", BaudRate::BAUD3000000},
        {"3500000", BaudRate::BAUD3500000},
        {"4000000", BaudRate::BAUD4000000}
};

static std::map<std::string, StopBits> stopBitsLookup{
        {"one", StopBits::ONE},
        {"two", StopBits::TWO}
};

static std::map<std::string, DataBits> dataBitsLookup {
        {"five", DataBits::FIVE},
        {"six", DataBits::SIX},
        {"seven", DataBits::SEVEN},
        {"eight", DataBits::EIGHT}
};

static std::map<std::string, Parity> parityLookup {
        {"none", Parity::NONE},
        {"even", Parity::EVEN},
        {"odd", Parity::ODD}
};

std::string baudRateToString(BaudRate baudRate);
std::string dataBitsToString(DataBits dataBits);
std::string stopBitsToString(StopBits stopBits);
std::string parityToString(Parity parity);


BaudRate tryParseBaudRate(char *name);
StopBits tryParseStopBits(char *name);
DataBits tryParseDataBits(char *name);
Parity tryParseParity(char *name);
std::string tryParseLineEnding(char *name);


int main(int argc, char *argv[]) {

    MessageLogger::initializeInstance(globalLogHandler);
    opterr = 0;
    int optionIndex{0};
    int currentOption{0};

    std::string portName{""};
    BaudRate baudRate{BaudRate::BAUD9600};
    StopBits stopBits{StopBits::ONE};
    DataBits dataBits{DataBits::EIGHT};
    Parity parity{Parity::NONE};
    std::string lineEnding{"\n"};
    while ( -1 != (currentOption = getopt_long(argc, argv, "vep:b:s:d:a:n:", longOptions, &optionIndex)) ) {
        switch (currentOption) {
            case 'p':
                portName = optarg;
                break;
            case 'b':
                baudRate = tryParseBaudRate(optarg);
                break;
            case 's':
                stopBits = tryParseStopBits(optarg);
                break;
            case 'd':
                dataBits = tryParseDataBits(optarg);
                break;
            case 'a':
                parity = tryParseParity(optarg);
                break;
            case 'n':
                lineEnding = tryParseLineEnding(optarg);
                break;
            case 'h':
                displayHelp();
                exit(EXIT_SUCCESS);
            case 'v':
                displayVersion();
                exit(EXIT_SUCCESS);
            case 'e':
                ApplicationUtilities::verboseLogging = true;
                break;
            default:
                LOG_WARN() << TStringFormat(R"(Unknown option "{0}", skipping)", longOptions[optionIndex].name);
                break;
        }
    }
    for (int i = 1; i < argc; i++) {
        if ( (strlen(argv[i]) > 0) && (argv[i][0] != '-') ) {
            portName = argv[i];
        }
    }
    displayVersion();

    if (portName.empty()) {
        displayHelp();
        LOG_FATAL() << "Please specify serial port with (or without) the -p option";
    }
    LOG_INFO() << TStringFormat("Using LogFile {0}", ApplicationUtilities::getLogFilePath());
    LOG_INFO() << TStringFormat("Using PortName {0}", portName);
    LOG_INFO() << TStringFormat("Using BaudRate {0}", baudRateToString(baudRate));
    LOG_INFO() << TStringFormat("Using DataBits {0}", dataBitsToString(dataBits));
    LOG_INFO() << TStringFormat("Using StopBits {0}", stopBitsToString(stopBits));
    LOG_INFO() << TStringFormat("Using Parity {0}", parityToString(parity));


    std::shared_ptr<SerialPort> serialPort{std::make_shared<SerialPort>(portName, baudRate, dataBits, stopBits, parity)};
    serialPort->setLineEnding(lineEnding);
    serialPort->openPort();




    return 0;
}

BaudRate tryParseBaudRate(char *name)
{
    if (!name) {
        throw std::runtime_error("Empty string not valid for parameter baud rate");
    }
    std::string nameCopy{name};
    toLower(nameCopy);
    if (nameCopy.empty()) {
        throw std::runtime_error("Empty string not valid for parameter stop bits");
    }

    auto foundRate = baudRateLookup.find(nameCopy);
    if (foundRate == baudRateLookup.end()) {
        throw std::runtime_error(TMessageLogger::TStringFormat("{0} is not a valid value for parameter \"baud rate\"", name));
    }
    return foundRate->second;
}

StopBits tryParseStopBits(char *name)
{
    if (!name) {
        throw std::runtime_error("Empty string not valid for parameter stop bits");
    }
    std::string nameCopy{name};
    toLower(nameCopy);
    if (nameCopy.empty()) {
        throw std::runtime_error("Empty string not valid for parameter stop bits");
    }
    if ( (nameCopy[0] == '1') || (startsWith(nameCopy, "one")) ) {
        return StopBits::ONE;
    } else if ( (nameCopy[0] == '2') || (startsWith(nameCopy, "two")) ) {
        return StopBits::TWO;
    } else {
        throw std::runtime_error(TMessageLogger::TStringFormat("{0} is not a valid value for parameter \"stop bits\"", name));
    }
}

DataBits tryParseDataBits(char *name)
{
    if (!name) {
        throw std::runtime_error("Empty string not valid for parameter data bits");
    }
    std::string nameCopy{name};
    toLower(nameCopy);
    if (nameCopy.empty()) {
        throw std::runtime_error("Empty string not valid for parameter data bits");
    }
    if ( (nameCopy[0] == '5') || (startsWith(nameCopy, "five")) ) {
        return DataBits::FIVE;
    } else if ( (nameCopy[0] == '6') || (startsWith(nameCopy, "six")) ) {
        return DataBits::SIX;
    } else if ( (nameCopy[0] == '7') || (startsWith(nameCopy, "seven")) ) {
        return DataBits::SEVEN;
    } else if ( (nameCopy[0] == '8') || (startsWith(nameCopy, "eight")) ) {
        return DataBits::EIGHT;
    } else {
        throw std::runtime_error(TMessageLogger::TStringFormat("{0} is not a valid value for parameter \"data bits\"", name));
    }
}

Parity tryParseParity(char *name)
{
    if (!name) {
        throw std::runtime_error("Empty string not valid for parameter parity");
    }
    std::string nameCopy{name};
    toLower(nameCopy);
    if (nameCopy.empty()) {
        throw std::runtime_error("Empty string not valid for parameter parity");
    }
    if (nameCopy[0] == 'n') {
        return Parity::NONE;
    } else if (nameCopy[0] == 'e') {
        return Parity::EVEN;
    } else if (nameCopy[0] == 'o') {
        return Parity ::ODD;
    } else {
        throw std::runtime_error(TMessageLogger::TStringFormat("{0} is not a valid value for parameter \"parity\"", name));
    }
}

std::string tryParseLineEnding(char *name)
{
    std::string nameCopy{name};
    toLower(nameCopy);
    if (nameCopy.empty()) {
        throw std::runtime_error("Empty string not valid for parameter line ending");
    }
    if ( (nameCopy[0] == 'n') || (startsWith(nameCopy, "new")) ) {
        return "\n";
    } else if ( (nameCopy[0] == 'c') || (startsWith(nameCopy, "cr")) ) {
        if (nameCopy.find("lf") == std::string::npos) {
            return "\r\n";
        } else {
            return "\r";
        }
    } else {
        return std::string{name};
    }
}

std::string baudRateToString(BaudRate baudRate)
{
    for (const auto &it : baudRateLookup) {
        if (it.second == baudRate) {
            return it.first;
        }
    }
    return "";
}

std::string dataBitsToString(DataBits dataBits)
{
    for (const auto &it : dataBitsLookup) {
        if (it.second == dataBits) {
            return it.first;
        }
    }
    return "";
}

std::string stopBitsToString(StopBits stopBits)
{
    for (const auto &it : stopBitsLookup) {
        if (it.second == stopBits) {
            return it.first;
        }
    }
    return "";
}

std::string parityToString(Parity parity)
{
    for (const auto &it : parityLookup) {
        if (it.second == parity) {
            return it.first;
        }
    }
    return "";
}
