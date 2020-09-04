#include "debugger.hpp"
#include "../debugger/debugger.hpp"
#include "../workspace/run_config.hpp"

#include <attender/attender/session/uuid_session_cookie_generator.hpp>

#include <optional>
#include <string>

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
    DebuggerRouter::~DebuggerRouter()
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void DebuggerRouter::registerRoutes(attender::tcp_server& server)
    {
        cors_options(server, "/api/debugger/createInstance", "POST", impl_->config.corsOption);
        server.post("/api/debugger/createInstance", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                std::string runProfileName;

                if (body.contains("runProfileName"))
                    runProfileName = body["runProfileName"].get<std::string>();
                else
                    return respondWithError(res, "need runProfileName");

                auto sess = this_session(req);
                if (sess.workspace.root.empty())
                    return respondWithError(res, "open a workspace first");
                if (sess.workspace.activeProject.empty())
                    return respondWithError(res, "no active project");

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

                auto profile = runConf.findProfile(runProfileName);
                if (!profile)
                    return respondWithError(res, "profile with given name not found in active project");

                const auto id = impl_->idGenerator.generate_id();
                sess.save_partial([&](auto& toSave, auto&)
                {
                    toSave.debuggerInstances[id] = Debugger{};
                });

                return jsonResponse(res, json{
                    {"instanceId", id}
                });
            });
        });
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
