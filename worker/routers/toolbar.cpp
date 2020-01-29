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
    Toolbar::Toolbar(attender::tcp_server& server)
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
            respondWithError(res, "could not extract path parameters, toolbar id or component id");
            return false;
        }

        try
        {
            tid = std::stol(tidStr);
        }
        catch(std::exception const& exc)
        {
            respondWithError(res, "the toolbar id is not convertible to a number");
            return false;
        }

        if (tid >= toolbars.size() || tid < 0)
        {
            respondWithError(res, "toolbar id is out of range");
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
                respondWithError(res, "Component id is either not a number or not a (string) id of this toolbar");
                return false;
            }
            cid = static_cast <std::decay_t <decltype(cid)>> (id);
        }

        try
        {
            if (cid >= toolbars[tid]->getActors().size() || cid < 0)
            {
                respondWithError(res, "component id is out of range");
                return false;
            }
        }
        catch(std::exception const& exc)
        {
            respondWithError(res, exc.what());
            return false;
        }
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Toolbar::registerRoutes(attender::tcp_server& server)
    {
        using namespace Toolbars::Types;

        server.get("/api/toolbar/enlist", [this](auto req, auto res)
        {
            std::vector <decltype(json::object())> bars(toolbars.size());
            std::transform(std::begin(toolbars), std::end(toolbars), std::begin(bars), [](auto const& bar)
            {
                return json::object({
                    {"uuid", bar->uuid()},
                    {"name", bar->name()}
                });
            });

            json response = {
                {"error", false},
                {"toolbars", bars}
            };

            json_response(res, response);
        });

        server.get("/api/toolbar/:id/get", [this](auto req, auto res)
        {
            try
            {
                std::size_t id = std::stol(req->param("id"));
                if (id >= toolbars.size() || id < 0)
                {
                    return respondWithError(res, "invalid toolbar id");
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

                return json_response(res, response);
            }
            catch(std::exception const& exc)
            {
                return respondWithError(res, exc.what());
            }
        });

        server.get("/api/toolbar/:tid/:cid/click", [this](auto req, auto res)
        {
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
                        return respondWithError(res, "component does not have a click action");
                    }
                }, actor);

                if (!res->has_concluded())
                    return json_response(res, json{{"error", false}});
            }
            catch(std::exception const& exc)
            {
                return respondWithError(res, exc.what());
            }
        });

        server.post("/api/toolbar/:tid/:cid/setSelected", [this](auto req, auto res)
        {
            std::size_t tid{0};
            std::size_t cid{0};

            if (!parseIds(req, res, tid, cid))
                return;

            try
            {
                std::visit(overloaded{
                    [&](IconButton const&)
                    {
                        return respondWithError(res, "component does not have a setSelected action");
                    },
                    [&](ComboBox& box)
                    {
                        try
                        {
                            auto buffer = std::make_shared <std::string>();

                            auto index = req->query("i");
                            if (!index)
                                return respondWithError(res, "Requires a query value called i");

                            int i = std::stol(index.value());
                            if (i < -1 || i >= static_cast <int> (box.options.size()))
                                return respondWithError(res, "selection index out of range");

                            box.selected = i;
                        }
                        catch(std::exception const& exc)
                        {
                            return respondWithError(res, exc.what());
                        }
                    }
                }, toolbars[tid]->getActors()[cid]);

                if (!res->has_concluded())
                    return json_response(res, json{{"error", false}});
            }
            catch(std::exception const& exc)
            {
                return respondWithError(res, exc.what());
            }
        });
    }
//#####################################################################################################################
}
