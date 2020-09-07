#include "debugger.hpp"
#include "../debugger/debugger.hpp"
#include "../workspace/run_config.hpp"
#include "../routers.hpp"

#include <boost/algorithm/string/replace.hpp>

#include <attender/attender/session/uuid_session_cookie_generator.hpp>

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
    DebuggerRouter::DebuggerRouter(RouterCollection* collection, attender::tcp_server& server, Config const& config)
        : BasicRouter(collection, &server)
        , impl_{new DebuggerRouter::Implementation(config)}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    DebuggerRouter::~DebuggerRouter() = default;
//---------------------------------------------------------------------------------------------------------------------
    void DebuggerRouter::registerRoutes(attender::tcp_server& server)
    {
        addCreateInstanceRoute(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    void DebuggerRouter::addCreateInstanceRoute(attender::tcp_server& server)
    {
        cors_options(server, "/api/debugger/createInstance", "POST", impl_->config.corsOption);
        server.post("/api/debugger/createInstance", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                std::string runProfileName;

                auto sess = this_session(req);
                if (sess.workspace.root.empty())
                    return respondWithError(res, "open a workspace first");
                if (sess.workspace.activeProject.empty())
                    return respondWithError(res, "no active project");

                if (body.contains("runProfileName"))
                    runProfileName = body["runProfileName"].get<std::string>();
                else
                    return respondWithError(res, "need runProfileName");

                bool allowDuplicateSession = false;
                if (body.contains("allowDuplicate"))
                    allowDuplicateSession = body["allowDuplicate"].get<bool>();

                RunConfig runConf{sess.workspace.activeProject};
                try
                {
                    runConf.load();
                    auto raw = runConf.raw();
                    if (raw.empty())
                        return respondWithError(res, "run config is empty");
                }
                catch(std::exception const& exc)
                {
                    return res->status(500).send(exc.what());
                }

                auto maybeProfile = runConf.findProfile(runProfileName);
                if (!maybeProfile)
                    return respondWithError(res, "profile with given name not found in active project");

                auto profile = maybeProfile.value();

                // replace vars in exec.
                boost::algorithm::replace_all(profile.executeable, "${ProjectRoot}"s, sess.workspace.activeProject.generic_string());
                boost::algorithm::replace_all(profile.executeable, "${WorkspaceRoot}"s, sess.workspace.root.generic_string());

                boost::algorithm::replace_all(profile.debugger, "${ProjectRoot}"s, sess.workspace.activeProject.generic_string());
                boost::algorithm::replace_all(profile.debugger, "${WorkspaceRoot}"s, sess.workspace.root.generic_string());

                // TODO: correct substitutes from a debugger settings file or something:
                boost::algorithm::replace_all(profile.debugger, "${lldb}"s, "lldb-mi"s);
                boost::algorithm::replace_all(profile.debugger, "${gdb}"s, "gdb"s);

                // extract environment for use
                auto publicSettings = collection_->settingsProv().settings();
                std::optional <std::unordered_map <std::string, std::string>> env;
                if (profile.environment.empty() || profile.environment == "inherit" || profile.environment == "default")
                {
                    env = publicSettings.compileEnvironment(profile.environment);
                }

                if (!allowDuplicateSession)
                {
                    auto result = std::find_if
                    (
                        std::begin(sess.debuggerInstances),
                        std::end(sess.debuggerInstances),
                        [&runProfileName](auto const& elem)
                        {
                            return elem.second.runConfigName() == runProfileName;
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
                    toSave.debuggerInstances[id] = Debugger{profile, id};
                    auto& debugger = toSave.debuggerInstances[id];

                    debugger.start(env);
                });

                return jsonResponse(res, json{
                    {"instanceId", id}
                });
            });
        });
    }
//#####################################################################################################################
}
