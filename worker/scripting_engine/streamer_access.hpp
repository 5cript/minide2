#pragma once

#include "state.hpp"
#include "../session/session_obtainer.hpp"
#include "../streaming/streamer_base.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace MinIDE::Scripting
{
    /**
     *  Exposed to lua.
     */
    class LuaStreamer
    {
    public:
        LuaStreamer(std::weak_ptr <StateCollection> weakStateRef, SessionObtainer sessionAccess, Streaming::StreamerBase* streamer);
        ~LuaStreamer();

        /**
         *  Use this function to send errors to the client.
         *  @param message A message describing what went wrong. May use localized keys $Something.
         *  @param errorType A type descriptor that shows the stage or step in which the error occured. See ErrorTypes enum
         *  @param formattedData some additional info in formatted form (JSON)
         */
        bool sendError(std::string const& message, int errorType, std::string const& formattedData);

        /**
         *  Warnings are not errors, so a shown warning should not have consequences for success.
         */
        bool sendWarning(std::string const& message, std::string const& formattedData);

        /**
         *  Send info.
         */
        bool sendInformation(std::string const& message, std::string const& formattedData);

        /**
         *  Some tools have special handling in the IDE.
         *  @param programName the name of the program being run "cmake" or "cmake.exe" are valid.
         *         Some programs are known and receive special handling.
         *  @param data The preferably raw data coming from the process.
         *  @param kind The kind of data. Like build data and errors?
         */
        bool sendSubprocessStdout(std::string const& programName, std::string const& data, int kind);

        /**
         *  Some tools have special handling in the IDE. Basically same as stdout.
         *  @param programName the name of the program being run "cmake" or "cmake.exe" are valid.
         *         Some programs are known and receive special handling.
         *  @param data The preferably raw data coming from the process.
         *  @param kind The kind of data. Like build data and errors?
         */
        bool sendSubprocessStderr(std::string const& programName, std::string const& data, int kind);

        /**
         *  Request a remote procedure call passing formattedData (json) to it.
         */
        bool remoteProcedureRequest(std::string const& fname, std::string const& formattedData);

        /**
         *  Request a remote procedure call passing formattedData (json) to it.
         */
        bool sendProcessInfo(std::string const& programName, std::string const& formattedData);

        /**
         *  Create input form on client
         */
        bool createInputForm(std::string const& identification, std::string const& jsonSpecification);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };

    void loadStreamerAccess(std::weak_ptr <StateCollection> state, SessionObtainer sessionAccess, Streaming::StreamerBase* streamer);
}
