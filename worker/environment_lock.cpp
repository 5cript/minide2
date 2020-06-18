#include "environment_lock.hpp"

#include <mutex>
#include <cstdio>

#ifdef _WIN32
#   include <windows.h>
#endif

using namespace std::string_literals;

std::mutex globalProcessExecutionLock;

//#####################################################################################################################
void environmentLockedDo(std::function <void()> const& work)
{
    std::lock_guard <std::mutex> {globalProcessExecutionLock};
    work();
}
//---------------------------------------------------------------------------------------------------------------------
void doWithModifiedPath(std::function <void()> const& work, std::string const& path)
{
    std::lock_guard <std::mutex> {globalProcessExecutionLock};
    std::string prevPath{getenv("PATH")};
    auto setPath = [](std::string const& to)
    {
#ifdef _WIN32
        std::string p = "PATH="s + to;
        _putenv(p.c_str());
#else
        setenv("PATH", to.c_str(), 1);
#endif
    };
    setPath(path);
    work();
    setPath(prevPath);
}
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
