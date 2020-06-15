#include "termination_handler.hpp"
#include "log.hpp"

#include <cxxabi.h>
#include <thread>
#include <chrono>

#ifndef WINDOWS
#   include <signal.h>
#endif

#define BOOST_STACKTRACE_USE_BACKTRACE
#include <boost/stacktrace.hpp>

//#####################################################################################################################
[[noreturn]] void onTerminate() noexcept
{
    if (auto exc = std::current_exception())
    {
        // we have an exception
        LOG() << "uncaught exception in terminate handler: \n";
        int status;
        LOGEX(false) << '\t' << abi::__cxa_demangle(abi::__cxa_current_exception_type()->name(), 0, 0, &status) << "\n";
        std::stringstream sstr;
        LOGEX(false) << "-------------\n";
        sstr << boost::stacktrace::stacktrace();
        sstr << sstr.str() << "\n";
        LOGEX(false) << "-------------\n";
        try
        {
            rethrow_exception(exc); // throw to recognize the type
        }
        catch (std::exception const& exc)
        {
            LOGEX(false) << '\t' << exc.what() << "\n";
        }
        catch (...)
        {
            LOGEX(false) << "WARNING! This is not a derivative of std::exception" << "\n";
            LOGEX(false) << "This is a severe error" << "\n";
            // additional action
        }
        std::_Exit(EXIT_FAILURE);
    }

    std::_Exit(EXIT_FAILURE);
}
//---------------------------------------------------------------------------------------------------------------------
[[noreturn]] void onBadSignal(int signal) noexcept
{
    LOG() << "handling signal: ";
    switch (signal)
    {
        case SIGABRT:
            LOGEX(false) << "SIGABRT\n";
            break;
        case SIGFPE:
            LOGEX(false) << "SIGFPE\n";
            break;
        case SIGILL:
            LOGEX(false) << "SIGILL\n";
            break;
        case SIGSEGV:
            LOGEX(false) << "SIGSEGV\n";
            break;
    }

    std::stringstream sstr;
    LOGEX(false) << "-------------\n";
    sstr << boost::stacktrace::stacktrace();
    LOGEX(false) << sstr.str();
    LOGEX(false) << "-------------\n";

    std::_Exit(EXIT_FAILURE);
}
//#####################################################################################################################
