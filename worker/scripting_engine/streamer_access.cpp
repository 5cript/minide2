#include "streamer_access.hpp"

#include "../streaming/common_messages/messages_from_lua.hpp"
#include "../streaming/streamer_base.hpp"

namespace MinIDE::Scripting
{
    using namespace Routers;

//#####################################################################################################################
    struct LuaStreamer::Implementation
    {
        std::weak_ptr <StateCollection> weakStateRef;
        SessionObtainer sessionAccess;
        Streaming::StreamerBase* streamer;

        Implementation
        (
            std::weak_ptr <StateCollection>&& stateRef,
            SessionObtainer&& sessionAccess,
            Streaming::StreamerBase* streamer
        )
            : weakStateRef{std::move(stateRef)}
            , sessionAccess{std::move(sessionAccess)}
            , streamer{streamer}
        {
        }
    };
//#####################################################################################################################
    LuaStreamer::LuaStreamer
    (
        std::weak_ptr <StateCollection> weakStateRef,
        SessionObtainer sessionAccess,
        Streaming::StreamerBase* streamer
    )
        : impl_{new LuaStreamer::Implementation(std::move(weakStateRef), std::move(sessionAccess), streamer)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    LuaStreamer::~LuaStreamer() = default;
//---------------------------------------------------------------------------------------------------------------------
    bool LuaStreamer::sendError(std::string const& message, int errorType, std::string const& formattedData)
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return false;
        impl_->streamer->send
        (
            Streaming::StreamChannel::Control,
            s.value().remoteAddress,
            s.value().controlId,
            Streaming::makeMessage<Streaming::Messages::LuaErrorMessage>
            (
                message,
                formattedData,
                static_cast <Streaming::Messages::ErrorTypes>(errorType)
            )
        );
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool LuaStreamer::sendWarning(std::string const& message, std::string const& formattedData)
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return false;
        impl_->streamer->send
        (
            Streaming::StreamChannel::Control,
            s.value().remoteAddress,
            s.value().controlId,
            Streaming::makeMessage<Streaming::Messages::LuaWarningMessage>
            (
                message,
                formattedData
            )
        );
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool LuaStreamer::sendInformation(std::string const& message, std::string const& formattedData)
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return false;
        impl_->streamer->send
        (
            Streaming::StreamChannel::Control,
            s.value().remoteAddress,
            s.value().controlId,
            Streaming::makeMessage<Streaming::Messages::LuaInfoMessage>
            (
                message,
                formattedData
            )
        );
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool LuaStreamer::sendSubprocessStdout(std::string const& programName, std::string const& data, int kind)
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return false;
        impl_->streamer->send
        (
            Streaming::StreamChannel::Control,
            s.value().remoteAddress,
            s.value().controlId,
            Streaming::makeMessage<Streaming::Messages::LuaProcessOutputMessage>
            (
                false,
                programName,
                data,
                static_cast <Streaming::Messages::SubprocessOutputType>(kind)
            )
        );
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool LuaStreamer::sendSubprocessStderr(std::string const& programName, std::string const& data, int kind)
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return false;
        impl_->streamer->send
        (
            Streaming::StreamChannel::Control,
            s.value().remoteAddress,
            s.value().controlId,
            Streaming::makeMessage<Streaming::Messages::LuaProcessOutputMessage>
            (
                true,
                programName,
                data,
                static_cast <Streaming::Messages::SubprocessOutputType>(kind)
            )
        );
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool LuaStreamer::remoteProcedureRequest(std::string const& fname, std::string const& formattedData)
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return false;
        impl_->streamer->send
        (
            Streaming::StreamChannel::Control,
            s.value().remoteAddress,
            s.value().controlId,
            Streaming::makeMessage<Streaming::Messages::LuaRemoteProcedureCall>
            (
                fname,
                formattedData
            )
        );
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool LuaStreamer::sendProcessInfo(std::string const& programName, std::string const& formattedData)
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return false;
        impl_->streamer->send
        (
            Streaming::StreamChannel::Control,
            s.value().remoteAddress,
            s.value().controlId,
            Streaming::makeMessage<Streaming::Messages::LuaProcessInfo>
            (
                programName,
                formattedData
            )
        );
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool LuaStreamer::createInputForm(std::string const& identification, std::string const& jsonSpecification)
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return false;

        impl_->streamer->send
        (
            Streaming::StreamChannel::Control,
            s.value().remoteAddress,
            s.value().controlId,
            Streaming::makeMessage<Streaming::Messages::CreateInputForm>
            (
                identification,
                jsonSpecification
            )
        );
        return true;
    }
//#####################################################################################################################
    void loadStreamerAccess
    (
        std::weak_ptr <StateCollection> state,
        SessionObtainer sessionAccess,
        Streaming::StreamerBase* streamer
    )
    {
        auto strongRef = state.lock();
        if (!strongRef)
            return;
        std::lock_guard <StateCollection::mutex_type> {strongRef->globalMutex};

        auto usertype = strongRef->lua.new_usertype<LuaStreamer>
        (
            "Streamer",
            "new", sol::initializers
            (
                [state, sessionAccess{std::move(sessionAccess)}, streamer](LuaStreamer& p) -> void
                {
                    new (&p) LuaStreamer(state, std::move(sessionAccess), streamer);
                }
            ),
            "send_error", &LuaStreamer::sendError,
            "send_warning", &LuaStreamer::sendWarning,
            "send_info", &LuaStreamer::sendInformation,
            "send_subprocess_stdout", &LuaStreamer::sendSubprocessStdout,
            "send_subprocess_stderr", &LuaStreamer::sendSubprocessStderr,
            "send_subprocess_info", &LuaStreamer::sendProcessInfo,
            "remote_call", &LuaStreamer::remoteProcedureRequest,
            "create_input_form", &LuaStreamer::createInputForm
        );

        strongRef->lua.new_enum(
            "ErrorTypes",
            "other", Streaming::Messages::ErrorTypes::Other,
            "precondition", Streaming::Messages::ErrorTypes::Precondition,
            "lua", Streaming::Messages::ErrorTypes::Lua,
            "execution", Streaming::Messages::ErrorTypes::Execution,
            "io", Streaming::Messages::ErrorTypes::Io
        );

        strongRef->lua.new_enum(
            "OutputType",
            "other", Streaming::Messages::SubprocessOutputType::Other,
            "build", Streaming::Messages::SubprocessOutputType::Build,
            "cmake", Streaming::Messages::SubprocessOutputType::CMake
        );
    }
//#####################################################################################################################
}
