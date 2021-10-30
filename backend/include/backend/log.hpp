#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <utility>
#include <string_view>
#include <optional>

class Logger
{
public:
    Logger();

    /**
     *  @param filename The file name of the log(s) WITHOUT extension.
     *  @param rotation The amount of log files to keeep. Pass 0 to disable log.
     */
    void open(std::string const& filename, int rotation);

    /**
     *  Short logging for easier readability in debug scenarios?
     */
    void setConcise(bool concise);

    /**
     *  Enable / Disable outputing to terminal.
     */
    void setTerminalEnabled(bool enabled);

    /**
     *  Write a stamp with time, file, function, line.
     */
    void stamp(char const* file, char const* func, int line);

    /**
     *  Logs do save their path to the file, which may become quite long.
     *  Use this to to remove everything from the path that is indicated by the main.cpp file. No source file
     *  should be above main.cpp or cutting is not performed.
     */
    void configureProjectMainFile(std::string const& directory);

    /**
     *  Also output to terminal?
     */
    bool terminalEnabled();

    template <typename T>
    void write(T const& value)
    {
        if (file_.is_open())
            file_ << value << std::flush;
        if (terminal_)
            std::cout << value;
    }

    void manipulate(std::ios_base&(*manip)(std::ios_base&));

private:
    bool concise_;
    std::ofstream file_;
    std::string root_;
    bool terminal_ = true;
};

class LogProxy
{
public:
    Logger& log();

    LogProxy() = default;
    ~LogProxy() = default;

    LogProxy& operator=(LogProxy const&) = delete;
    LogProxy(LogProxy const&) = delete;

    LogProxy(LogProxy&&) = default;
    LogProxy& operator=(LogProxy&&) = default;
};

LogProxy operator<<(LogProxy&& proxy, std::string const& value);
LogProxy operator<<(LogProxy&& proxy, std::string_view const& value);
LogProxy operator<<(LogProxy&& proxy, char value);
LogProxy operator<<(LogProxy&& proxy, char const* value);
LogProxy operator<<(LogProxy&& proxy, std::ios_base&(*manip)(std::ios_base&));
LogProxy operator<<(LogProxy&& proxy, std::_Setw);

template <
    typename T,
    class = typename std::enable_if<
        (std::is_arithmetic<T>::value && !std::is_same<T, char>::value && !std::is_same<T, wchar_t>::value) &&
        !std::is_enum<T>::value
    >::type
>
LogProxy operator<<(LogProxy&& proxy, T value)
{
    proxy.log().write(value);
    return proxy;
}

template <typename T>
LogProxy operator<<(LogProxy&& proxy, std::optional <T> const& opt)
{
    if (opt)
        return operator<<(std::move(proxy), opt.get());
    else
        return operator<<(std::move(proxy), "[nullopt]");
}

LogProxy logImpl(char const* file, char const* func, int line, bool stamp = true);

void setLogTerminalEnabled(bool enabled);

#ifndef NO_LOG_FILE
#   ifdef __GNUC__
#      define LOG() logImpl(__FILE__, __PRETTY_FUNCTION__, __LINE__, true)
#      define LOGEX(stamp) logImpl(__FILE__, __PRETTY_FUNCTION__, __LINE__, stamp)
#   else
#      define LOG() logImpl(__FILE__, __func__, __LINE__, true)
#      define LOGEX(stamp) logImpl(__FILE__, __func__, __LINE__, stamp)
#   endif
#else
#   define LOG() std::cout
#   define LOGEX(X) std::cout
#endif

