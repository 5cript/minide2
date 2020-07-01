#pragma once

#include "../../json.hpp"

namespace Streaming::Messages
{
    enum class ErrorTypes
    {
        Other,
        Precondition,
        Lua,
        Execution,
        Io
    };

    enum class SubprocessOutputType
    {
        Other,
        Build,
        CMake
    };

    struct LuaInfoMessage : public JsonSerializable
    {
        std::string message;
        std::string formattedData;

        LuaInfoMessage() = default;
        LuaInfoMessage(std::string m, std::string fm)
            : message{std::move(m)}
            , formattedData{std::move(fm)}
        {
        }
        LuaInfoMessage(LuaInfoMessage&&) = default;

        std::string toJson() const override;
    };

    struct LuaErrorMessage : public JsonSerializable
    {
        std::string message;
        std::string formattedData;
        ErrorTypes type;

        LuaErrorMessage() = default;
        LuaErrorMessage(std::string m, std::string fm, ErrorTypes type)
            : message{std::move(m)}
            , formattedData{std::move(fm)}
            , type{type}
        {
        }
        LuaErrorMessage(LuaErrorMessage&&) = default;

        std::string toJson() const override;
    };

    struct LuaWarningMessage : public JsonSerializable
    {
        std::string message;
        std::string formattedData;

        LuaWarningMessage() = default;
        LuaWarningMessage(std::string m, std::string fm)
            : message{std::move(m)}
            , formattedData{std::move(fm)}
        {
        }
        LuaWarningMessage(LuaWarningMessage&&) = default;

        std::string toJson() const override;
    };

    struct LuaProcessOutputMessage : public JsonSerializable
    {
        bool stdErr;
        std::string processName;
        std::string message;
        SubprocessOutputType type;

        LuaProcessOutputMessage() = default;
        LuaProcessOutputMessage(bool stdErr, std::string name, std::string message, SubprocessOutputType type)
            : stdErr{stdErr}
            , processName{std::move(name)}
            , message{std::move(message)}
            , type{type}
        {
        }
        LuaProcessOutputMessage(LuaProcessOutputMessage&&) = default;

        std::string toJson() const override;
    };

    struct LuaRemoteProcedureCall : public JsonSerializable
    {
        std::string fname;
        std::string formattedData;

        LuaRemoteProcedureCall() = default;
        LuaRemoteProcedureCall(std::string fname, std::string formattedData)
            : fname{std::move(fname)}
            , formattedData{std::move(formattedData)}
        {
        }
        LuaRemoteProcedureCall(LuaRemoteProcedureCall&&) = default;

        std::string toJson() const override;
    };

    struct LuaProcessInfo : public JsonSerializable
    {
        std::string processName;
        std::string formattedData;

        LuaProcessInfo() = default;
        LuaProcessInfo(std::string processName, std::string formattedData)
            : processName{std::move(processName)}
            , formattedData{std::move(formattedData)}
        {
        }
        LuaProcessInfo(LuaProcessInfo&&) = default;

        std::string toJson() const override;
    };
}
