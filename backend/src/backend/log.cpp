#include <backend/log.hpp>

#include <boost/filesystem.hpp>
#include <chrono>
#include <sstream>
#include <cstdio>
#include <iostream>

//#####################################################################################################################
static Logger logger;
//---------------------------------------------------------------------------------------------------------------------
std::string makeLogName(std::string const& filename, int rotation)
{
    return filename + "_" + std::to_string(rotation) + ".txt";
}
//---------------------------------------------------------------------------------------------------------------------
void shiftLog(std::string const& filename, int rotation)
{
    if (rotation <= 0)
        return;

    for (int i = 0; i != rotation; ++i)
    {
        int fromEnd = rotation - i - 1;
        auto from = makeLogName(filename, fromEnd);
        auto to = makeLogName(filename, fromEnd + 1);

        if (boost::filesystem::exists(from))
            boost::filesystem::copy_file(from, to, boost::filesystem::copy_option::overwrite_if_exists);
    }
}
//---------------------------------------------------------------------------------------------------------------------
void setLogTerminalEnabled(bool enabled)
{
    logger.setTerminalEnabled(enabled);
}
//#####################################################################################################################
void Logger::open(std::string const& filename, int rotation)
{
    if (rotation < 0)
        return;

    shiftLog(filename, rotation);

    auto parentDir = boost::filesystem::path{filename}.parent_path();
    if (!boost::filesystem::exists(parentDir))
        boost::filesystem::create_directories(parentDir);

    file_.open(makeLogName(filename, 0));
}
//---------------------------------------------------------------------------------------------------------------------
void Logger::configureProjectMainFile(std::string const& directory)
{
    root_ = boost::filesystem::path{directory}.parent_path().string();
}
//---------------------------------------------------------------------------------------------------------------------
void Logger::stamp(char const* file, char const* func, int line)
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::string fileStr = file;
    if (!root_.empty() && fileStr.size() > root_.size() && fileStr.substr(0, root_.size()) == root_)
        fileStr = fileStr.substr(root_.size(), fileStr.size() - root_.size());

    std::stringstream prefixStream;
    prefixStream
        << "[" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << "] "
        << "(" << fileStr  << ":" << line << ") "
        << "{" << func << "} "
    ;

    write(prefixStream.str());
}
//---------------------------------------------------------------------------------------------------------------------
void Logger::manipulate(std::ios_base&(*manip)(std::ios_base&))
{
    if (file_.is_open())
        file_ << manip;
    if (terminal_)
        std::cout << manip;
}
//---------------------------------------------------------------------------------------------------------------------
void Logger::setTerminalEnabled(bool enabled)
{
    terminal_ = enabled;
}
//---------------------------------------------------------------------------------------------------------------------
bool Logger::terminalEnabled()
{
    return terminal_;
}
//#####################################################################################################################
Logger& LogProxy::log()
{
    return logger;
}
//#####################################################################################################################
LogProxy logImpl(char const* file, char const* func, int line, bool stamp)
{
    if (stamp)
        logger.stamp(file, func, line);
    return {};
}
//---------------------------------------------------------------------------------------------------------------------
LogProxy operator<<(LogProxy&& proxy, std::ios_base&(*manip)(std::ios_base&))
{
    logger.manipulate(manip);
    return proxy;
}
//---------------------------------------------------------------------------------------------------------------------
LogProxy operator<<(LogProxy&& proxy, std::_Setw)
{
    return proxy;
}
//---------------------------------------------------------------------------------------------------------------------
LogProxy operator<<(LogProxy&& proxy, std::string const& value)
{
    logger.write(value);
    return proxy;
}
//---------------------------------------------------------------------------------------------------------------------
LogProxy operator<<(LogProxy&& proxy, char value)
{
    logger.write(value);
    return proxy;
}
//---------------------------------------------------------------------------------------------------------------------
LogProxy operator<<(LogProxy&& proxy, std::string_view const& value)
{
    logger.write(value);
    return proxy;
}
//---------------------------------------------------------------------------------------------------------------------
LogProxy operator<<(LogProxy&& proxy, char const* value)
{
    logger.write(value);
    return proxy;
}
//#####################################################################################################################
