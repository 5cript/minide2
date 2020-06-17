#include "toolbar.hpp"

#include "../toolbars/cmake_toolbar.hpp"
#include "../variant.hpp"

#include <nlohmann/json.hpp>
#include <iterator>
#include <algorithm>

using json = nlohmann::json;

using namespace Toolbars;

namespace Routers
{
//#####################################################################################################################
    namespace
    {
        template <typename T>
        void jsonifyActor(json& obj, T actor)
        {
            std::visit([&obj](auto&& act) {
                obj["type"] = act.type;
                obj["id"] = act.id;
                obj["helpText"] = act.helpText;
            }, actor);
        }
    }
//#####################################################################################################################
    Toolbar::Toolbar(RouterCollection* collection, attender::tcp_server& server)
        : BasicRouter{collection}
    {
        registerRoutes(server);

        toolbars.emplace_back(new CMakeToolbar(std::to_string(toolbars.size())));
    }
//---------------------------------------------------------------------------------------------------------------------
    bool Toolbar::parseIds(attender::request_handler* req, attender::response_handler* res, std::size_t& tid, std::size_t& cid)
    {
        std::string tidStr;
        std::string cidStr;

        try
        {
            tidStr = req->param("tid");
            cidStr = req->param("cid");
        }
        catch(std::exception const& exc)
        {
            respondWithError(res, 400, "could not extract path parameters, toolbar id or component id");
            return false;
        }

        try
        {
            tid = std::stol(tidStr);
        }
        catch(std::exception const& exc)
        {
            respondWithError(res, 400, "the toolbar id is not convertible to a number");
            return false;
        }

        if (tid >= toolbars.size() || tid < 0)
        {
            respondWithError(res, 400, "toolbar id is out of range");
            return false;
        }

        try
        {
            cid = std::stol(cidStr);
        }
        catch(std::exception const& exc)
        {
            auto id = toolbars[tid]->actorIndexById(cidStr);
            if (id == -1)
            {
                respondWithError(res, 400, "Component id is either not a number or not a (string) id of this toolbar");
                return false;
            }
            cid = static_cast <std::decay_t <decltype(cid)>> (id);
        }

        try
        {
            if (cid >= toolbars[tid]->getActors().size() || cid < 0)
            {
                respondWithError(res, 400, "component id is out of range");
                return false;
            }
        }
        catch(std::exception const& exc)
        {
            respondWithError(res, 500, exc.what());
            return false;
        }
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Toolbar::registerRoutes(attender::tcp_server& server)
    {
        using namespace Toolbars::Types;

        cors_options(server, "/api/toolbar/enlist", "GET");
        server.get("/api/toolbar/enlist", [this](auto req, auto res)
        {
            enable_cors(res);
            std::vector <decltype(json::object())> bars(toolbars.size());
            std::transform(std::begin(toolbars), std::end(toolbars), std::begin(bars), [](auto const& bar)
            {
                return json::object({
                    {"uuid", bar->uuid()},
                    {"name", bar->name()}
                });
            });

            json response = {
                {"toolbars", bars}
            };

            return sendJson(res, response);
        });

        cors_options(server, "/api/toolbar/:id/get", "GET");
        server.get("/api/toolbar/:id/get", [this](auto req, auto res)
        {
            enable_cors(res);
            try
            {
                std::size_t id = std::stol(req->param("id"));
                if (id >= toolbars.size() || id < 0)
                {
                    return respondWithError(res, 400, "invalid toolbar id");
                }

                auto& bar = toolbars[id];

                json response = {
                    {"error", false},
                    {"toolbar_id", bar->uuid()},
                    {"toolbar_name", bar->name()}
                };
                response["components"] = json::array();

                auto& actors = bar->getActors();

                for (auto const& actor : actors) {
                    json obj;
                    jsonifyActor(obj, actor);
                    std::visit(overloaded{
                        [&](IconButton const& button) { obj["pngbase64"] = button.pngbase64; },
                        [&](ComboBox const& combox)
                        {
                            obj["selected"] = combox.selected;
                            obj["options"] = combox.options;
                        }
                    }, actor);
                    response["components"].push_back(obj);
                }

                return sendJson(res, response);
            }
            catch(std::exception const& exc)
            {
                return respondWithError(res, 500, exc.what());
            }
        });

        cors_options(server, "/api/toolbar/:tid/:cid/click", "GET");
        server.get("/api/toolbar/:tid/:cid/click", [this](auto req, auto res)
        {
            enable_cors(res);
            std::size_t tid{0};
            std::size_t cid{0};

            if (!parseIds(req, res, tid, cid))
                return;

            try
            {
                auto& bar = toolbars[tid];

                auto& actor = bar->getActors()[cid];
                std::visit(overloaded{
                    [&](IconButton const& btn)
                    {
                        bar->onClick(cid);
                    },
                    [&](ComboBox const&)
                    {
                        return respondWithError(res, 400, "component does not have a click action");
                    }
                }, actor);

                if (!res->has_concluded())
                    return sendJson(res, json{{"error", false}});
            }
            catch(std::exception const& exc)
            {
                return respondWithError(res, 500, exc.what());
            }
        });

        cors_options(server, "/api/toolbar/:tid/:cid/setSelected", "POST");
        server.post("/api/toolbar/:tid/:cid/setSelected", [this](auto req, auto res)
        {
            enable_cors(res);
            std::size_t tid{0};
            std::size_t cid{0};

            if (!parseIds(req, res, tid, cid))
                return;

            try
            {
                std::visit(overloaded{
                    [&](IconButton const&)
                    {
                        return respondWithError(res, 400, "component does not have a setSelected action");
                    },
                    [&](ComboBox& box)
                    {
                        try
                        {
                            auto buffer = std::make_shared <std::string>();

                            auto index = req->query("i");
                            if (!index)
                                return respondWithError(res, 400, "Requires a query value called i");

                            int i = std::stol(index.value());
                            if (i < -1 || i >= static_cast <int> (box.options.size()))
                                return respondWithError(res, 400, "selection index out of range");

                            box.selected = i;
                        }
                        catch(std::exception const& exc)
                        {
                            return respondWithError(res, 500, exc.what());
                        }
                    }
                }, toolbars[tid]->getActors()[cid]);

                if (!res->has_concluded())
                    return jsonResponse(res, json{{"error", false}});
            }
            catch(std::exception const& exc)
            {
                return respondWithError(res, 400, exc.what());
            }
        });
    }
//#####################################################################################################################
}
