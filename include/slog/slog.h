#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <format>
#include <print>

/**
 * @brief simple logging library
 * 
 * @author Chams Eddine Hachani (agamenon) <chams.eddine.hachanii@gmail.com>
 */
namespace slog {
/**
    * @brief color values of ascii escape codes
    */
enum class Decorator {
    Black = 30, Red, Green, Yellow, Blue, Magenta, Cyan, White, Default = 39
};

/**
    * @brief base logging class
    * 
    * This is class can be used as is or inherited from.
    * It contains formatting properties and a functor to log the messages
    */
class Logger {
public:
    Logger() {
        _decorator = Decorator::Default;
        _prefix = "";
        _displayTime = false;
        _outputFile = "stdout";
    }

    /**
     * @brief set a prefix to the log message
     * 
     * @param prefix the prefix to be prepended
    */
    void setPrefix(std::string prefix) { _prefix = prefix; }
    /**
     * @brief set color of the text
     * 
     * @param decorator a Decorator value
    */
    void setDecorator(Decorator decorator) { _decorator = decorator; }
    /**
     * @brief set whether to prepend the time of logging or not
     * 
     * @param val true if you want to set the time of logging. false otherwise
    */
    void displayTime(bool val) { _displayTime = val; }
    /**
     * @brief set the stream or the name of the file to write to
     * 
     * @param filename the name of the stream or file
     * 
     * - if filename is set to "stdout", the message will be logged to stdout stream
     * - if filename is set to "stderr", the message will be logged to stderr stream
     * - if filename is set to something else, the message will be logged to a file with that name
     * 
     * @note if the message is printed to a file, no color formatting will be applied (because it's a text file duh)
    */
    void setOutputFile(std::string filename) { _outputFile = filename; }

    /**
     * @brief logs the message
     * 
     * @param message message to be logged
    */
    template<typename... Args>
    void operator()(std::string fmt, Args&&... args) {
        _print(_createPrintable(fmt, args...));
    }
protected:
    std::string _prefix;
    Decorator _decorator;
    bool _displayTime;
    std::string _outputFile;

    template<typename... Args>
    std::string _createPrintable(std::string fmt, Args&&... args) {
        // create time string
        std::string timeStr = "";
        if(_displayTime) {
            time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            timeStr = ctime(&time);
            timeStr = timeStr.substr(11, 8);
            timeStr += " == ";
        }

        std::string decoPrefix = "", decoSuffix = "";
        if(_outputFile == "stdout" || _outputFile == "stderr") {
            decoPrefix = "\e[";
            decoPrefix += std::to_string((int)_decorator);
            decoPrefix += "m";
            decoSuffix = "\e[0m";
        }
        
        return decoPrefix + timeStr + _prefix + decoSuffix + std::vformat(fmt, std::make_format_args(args...));
    }

    void _print(std::string message) {
        if(_outputFile == "stdout") {
            // printf("%s\n", message.c_str());
            std::println("{}", message);
            // printf("\e[%dm%s%s%s\n\e[0m", (int)_decorator, timeStr.c_str(), _prefix.c_str(), message.c_str());
        } else if(_outputFile == "stderr") {
            // fprintf(stderr, "%s\n", message.c_str());
            std::println(stderr, "{}", message);
            // fprintf(stderr, "\e[%dm%s%s%s\n\e[0m", (int)_decorator, timeStr.c_str(), _prefix.c_str(), message.c_str());
        } else {
            FILE *file = fopen(_outputFile.c_str(), "a");
            if(file) {
                std::println(file, "{}", message);
                // fprintf(file, "%s\n", message.c_str());
                // fprintf(file, "\e[%dm%s%s%s\n\e[0m", (int)_decorator, timeStr.c_str(), _prefix.c_str(), message.c_str());
                fclose(file);
            }
        }
    }
};

/**
 * @brief Error class
 * 
 * This class is a predefined error message configuration
 * The message will be printed to stderr in red and with "Error: " as prefix
*/
class Error : public Logger {
public:
    Error() : Logger() {
        _prefix = "Error:   ";
        _decorator = Decorator::Red;
        _outputFile = "stderr";
    }
};

/**
 * @brief Warning class
 * 
 * This class is a predefined warning message configuration
 * The message will be printed to stderr in yellow and with "Warning: " as prefix
*/
class Warning : public Logger {
public:
    Warning() : Logger() {
        _prefix = "Warning: ";
        _decorator = Decorator::Yellow;
        _outputFile = "stderr";
    }
};

/**
 * @brief Info class
 * 
 * This class is a predefined info message configuration
 * The message will be printed to stdout in blue and with "Info: " as prefix
*/
class Info : public Logger {
public:
    Info() : Logger() {
        _prefix = "Info:    ";
        _decorator = Decorator::Blue;
    }
};

/**
 * @brief Debug class
 * 
 * This class is a predefined debug message configuration
 * The message will be printed to stdout in green and with "Debug: " as prefix
*/
class Debug : public Logger {
public:
    Debug() : Logger() {
        _prefix = "Debug:   ";
        _decorator = Decorator::Green;
    }
};

extern Error error;
extern Warning warning;
extern Info info;
extern Debug debug;
extern Logger message;

// Make sure to define SLOG_IMPLEMENTATION in one of your source files
#ifdef SLOG_IMPLEMENTATION
Error error;
Warning warning;
Info info;
Debug debug;
Logger message;
#endif // SLOG_IMPLEMENTATION
}
