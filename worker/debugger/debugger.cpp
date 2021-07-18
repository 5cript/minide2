#include "debugger.hpp"
#include "../environment_lock.hpp"

#include "../streaming/streamer_base.hpp"
#include "../streaming/common_messages/inline_message.hpp"

#include <debugger-interface/debugger.hpp>
#include <memory>
#include <functional>

using namespace std::string_literals;

//#####################################################################################################################
json unpackValue(std::unique_ptr <DebuggerInterface::Value> const& val)
{
    json j = {};
    if (DebuggerInterface::isResult(val))
    {
        auto* res = static_cast <DebuggerInterface::Result*> (val.get());
        j["variable"] = res->variable;
        j["value"] = unpackValue(res->value);
    }
    else if (DebuggerInterface::isConst(val))
    {
        auto* res = static_cast <DebuggerInterface::Const*> (val.get());
        j["value"] = res->data;
    }
    return j;
}
//---------------------------------------------------------------------------------------------------------------------
std::vector <json> unpackResults(std::vector <DebuggerInterface::Result> const& results)
{
    std::vector <json> res;
    for (auto const& i : results)
    {
        if (i.value)
            res.push_back(unpackValue(i.value));
        else
        {
            json j{
                {"variable", i.variable}
            };
            res.push_back(std::move(j));
        }
    }
    return res;
}
//#####################################################################################################################
Debugger::Debugger
(
    Streaming::StreamerBase* streamer,
    std::string const& remoteAddress,
    int controlId,
    RunConfig const& usedConfig,
    std::string instanceId
)
    : streamer_{streamer}
    , remoteAddress_{remoteAddress}
    , controlId_{controlId}
    , runConfig_{usedConfig}
    , instanceId_{std::move(instanceId)}
    , debugInterface_{}
{
}
//---------------------------------------------------------------------------------------------------------------------
Debugger::~Debugger()
{
    std::cerr << "debugger instance destroyed\n";
}
//---------------------------------------------------------------------------------------------------------------------
std::string_view Debugger::runConfigName() const
{
    return runConfig_.name;
}
//---------------------------------------------------------------------------------------------------------------------
RunConfig Debugger::runConfig() const
{
    return runConfig_;
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::command(DebuggerInterface::MiCommand const& command) const
{
    if (debugInterface_)
        debugInterface_->sendCommand(command);
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::command(std::string const& command) const
{
    if (debugInterface_)
        debugInterface_->sendCommand(command + "\n");
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onRawData(std::string const& raw)
{
    /*
    for (auto const& r : raw)
    {
        if (r > 32)
            std::cout.put(r);
        else
        {
            if (r == '\n')
                std::cout << "\\n";
            else if (r == '\r')
                std::cout << "\\r";
            else if (r == '\t')
                std::cout << "\\t";
            else if (r == ' ')
                std::cout << "\\_";
            else if (r == '\v')
                std::cout << "\\v";
            else
                std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)r;
        }
    }
    std::cout << "<EOR>\n";
    */
    std::cout << raw;
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onLogStream(std::string const& message)
{
    relayMessage(json{
        {"messageType"s, "log_stream"s},
        {"data"s, message}
    });
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onConsoleStream(std::string const& message)
{
    relayMessage(json{
        {"messageType"s, "console_stream"s},
        {"data"s, message}
    });
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onParserError(std::string const& message)
{
    relayMessage(json{
        {"messageType"s, "parser_error"s},
        {"data"s, message}
    });
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onStdErr(std::string const& text)
{
    relayMessage(json{
        {"messageType"s, "stderr"s},
        {"data"s, text}
    });
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onResult(DebuggerInterface::ResultRecord const& result)
{
    relayMessage(json{
        {"messageType"s, "result"s},
        {"status"s, static_cast <int> (result.status)},
        {"results"s, unpackResults(result.results)}
    });
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onExec(DebuggerInterface::AsyncRecord const& record)
{
    relayAsyncRecord("exec_record", record);
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onStatus(DebuggerInterface::AsyncRecord const& record)
{
    relayAsyncRecord("status_record", record);
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onNotify(DebuggerInterface::AsyncRecord const& record)
{
    relayAsyncRecord("notify_record", record);
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onPartialRemain(std::string const& remain, std::string const& subject)
{
    relayMessage(json{
        {"messageType"s, "partial_remain"s},
        {"data"s, remain},
        {"subject"s, subject}
    });
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::relayAsyncRecord(std::string const& type, DebuggerInterface::AsyncRecord const& record)
{
    json j{
        {"messageType"s, type}
    };

    if (record.token)
        j["token"] = record.token.value();

    j["type"] = static_cast <int>(record.type);
    j["status"] = record.status;
    j["results"] = unpackResults(record.results);

    relayMessage(j);
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::relayMessage(json j)
{
    if (!streamer_)
        return;

    j["instanceId"] = instanceId_;

    streamer_->send
    (
        Streaming::StreamChannel::Control,
        remoteAddress_,
        controlId_,
        Streaming::makeMessage<Streaming::Messages::InlineMessage>
        (
            "debugger",
            j
        )
    );
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::start(std::optional <std::unordered_map <std::string, std::string>> const& env)
{
    DebuggerInterface::UserDefinedArguments args;
    args.debuggerExecuteable = runConfig_.debugger.path;
    args.commandline = runConfig_.arguments;
    if (env)
        args.environment = env.value();
    args.program = runConfig_.executeable;
    args.directory = runConfig_.directory;

    auto envDo = [this, &args]()
    {
        debugInterface_  = std::make_shared <DebuggerInterface::Debugger>(args);
        debugInterface_->registerListener(this);
        debugInterface_->start();
    };

    if (!env)
        environmentLockedDo(envDo);
    else
        doWithModifiedPath(envDo, env.value().at("PATH"));
}
//#####################################################################################################################
