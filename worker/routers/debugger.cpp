#include "debugger.hpp"
#include "../debugger/debugger.hpp"
#include "../workspace/run_config.hpp"
#include "../communication_center.hpp"

#include <boost/algorithm/string/replace.hpp>

#include <attender/session/uuid_session_cookie_generator.hpp>

#include <optional>
#include <string>

using namespace std::string_literals;

namespace Routers
{
//#####################################################################################################################
    struct DebuggerRouter::Implementation
    {
        Config config;
        attender::uuid_generator idGenerator;

        Implementation(Config const& config)
            : config{config}
            , idGenerator{}
        {
        }
    };
//#####################################################################################################################
    DebuggerRouter::DebuggerRouter(CommunicationCenter* collection, attender::http_server& server, Config const& config)
        : BasicRouter(collection, &server)
        , impl_{new DebuggerRouter::Implementation(config)}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    DebuggerRouter::~DebuggerRouter() = default;
//---------------------------------------------------------------------------------------------------------------------
    void DebuggerRouter::registerRoutes(attender::http_server& server)
    {
        addCreateInstanceRoute(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    void DebuggerRouter::addCreateInstanceRoute(attender::http_server& server)
    {
        cors_options(server, "/api/debugger/createInstance", "POST", impl_->config.corsOption);
        server.post("/api/debugger/createInstance", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                RunConfig runConf;

                auto sess = this_session(req);
                if (sess.workspace.root.empty())
                    return respondWithError(res, "open a workspace first");
                if (sess.workspace.activeProject.empty())
                    return respondWithError(res, "no active project");

                if (body.contains("runProfile"))
                    runConf = body["runProfile"].get<RunConfig>();
                else
                    return respondWithError(res, "need runProfile");

                [[maybe_unused]] bool allowDuplicateSession = false;
                if (body.contains("allowDuplicate"))
                    allowDuplicateSession = body["allowDuplicate"].get<bool>();

                auto replaceVariables = [&](std::string& str)
                {
                    boost::algorithm::replace_all(str, "${ProjectRoot}"s, sess.workspace.activeProject.generic_string());
                    boost::algorithm::replace_all(str, "${WorkspaceRoot}"s, sess.workspace.root.generic_string());
                };

                auto replaceVariablesOpt = [&](std::optional <std::string>& opt)
                {
                    if (opt)
                        replaceVariables(opt.value());
                };

                // replace vars in exec.

                replaceVariables(runConf.executeable);
                replaceVariables(runConf.debugger.path);
                replaceVariables(runConf.arguments);

                replaceVariablesOpt(runConf.directory);
                replaceVariablesOpt(runConf.debugger.commandFile);
                replaceVariablesOpt(runConf.debugger.initCommandFile);

                // extract environment for use
                auto publicSettings = collection_->settingsProv().settings();
                std::optional <std::unordered_map <std::string, std::string>> env;
                if (runConf.environment.empty() || runConf.environment == "inherit" || runConf.environment == "default")
                {
                    env = publicSettings.compileEnvironment(runConf.environment);
                }

                if (!allowDuplicateSession)
                {
                    auto result = std::find_if
                    (
                        std::begin(sess.debuggerInstances),
                        std::end(sess.debuggerInstances),
                        [&runConf](auto const& elem)
                        {
                            return elem.second->runConfigName() == runConf.name;
                        }
                    );

                    if (result != std::end(sess.debuggerInstances))
                        return respondWithError(res, "duplicate debugger session was prevented. pass allowDuplicate to allow it.");
                }

                const auto id = impl_->idGenerator.generate_id();
                sess.save_partial([&](auto& toSave, auto&)
                {
                    // this is potentially a bug.
                    // save_partial was not meant to contain logic.
                    // breaking this assumption might have side effects.
                    toSave.debuggerInstances[id] = std::make_shared <Debugger>
                    (
                        &collection_->streamer(),
                        sess.remoteAddress,
                        sess.controlId,
                        runConf,
                        id
                    );
                    auto& debugger = toSave.debuggerInstances[id];

                    debugger->start(env);
                });

                return jsonResponse(res, json{
                    {"instanceId", id}
                });
            });
        });

        cors_options(server, "/api/debugger/rawCommand", "POST", impl_->config.corsOption);
        server.post("/api/debugger/rawCommand", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                std::string instanceId;
                if (body.contains("instanceId"))
                    instanceId = body["instanceId"].get<std::string>();
                else
                    return respondWithError(res, "need instance id");

                std::string command;
                if (body.contains("command"))
                    command = body["command"].get<std::string>();
                else
                    return respondWithError(res, "need command string");

                auto sess = this_session(req);
                auto instance = sess.debuggerInstances.find(instanceId);
                if (instance == std::end(sess.debuggerInstances))
                    return respondWithError(res, "no debugger instance with that id found for current session");

                instance->second->command(command);
                res->send_status(204);
            });
        });


        cors_options(server, "/api/debugger/command", "POST", impl_->config.corsOption);
        server.post("/api/debugger/command", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                std::string instanceId;
                if (body.contains("instanceId"))
                    instanceId = body["instanceId"].get<std::string>();
                else
                    return respondWithError(res, "need instance id");

                DebuggerInterface::MiCommand miCommand;

                json command;
                if (body.contains("command"))
                    command = body["command"];
                else
                    return respondWithError(res, "need command object");

                if (command.contains("token"))
                    miCommand.token(command["token"].get<long long>());

                if (command.contains("operation"))
                    miCommand.operation(command["operation"].get<std::string>());
                else
                    return respondWithError(res, "command needs operation");

                if (command.contains("params") && command["params"].is_array())
                {
                    std::vector<std::string> params = command["params"].get<std::vector <std::string>>();
                    for (auto const& p : params)
                        miCommand.param(p);
                }

                if (command.contains("options"))
                {
                    for (auto const& item : command["options"].items())
                    {
                        miCommand.option(item.key(), item.value());
                    }
                }

                auto sess = this_session(req);
                auto instance = sess.debuggerInstances.find(instanceId);
                if (instance == std::end(sess.debuggerInstances))
                    return respondWithError(res, "no debugger instance with that id found for current session");

                instance->second->command(miCommand);
                res->send_status(204);
            });
        });
    }
//#####################################################################################################################
}
