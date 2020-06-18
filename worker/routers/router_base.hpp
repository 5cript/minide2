#pragma once

#include "../routers_fwd.hpp"
#include "../json.hpp"
#include "../session.hpp"

#include <attender/attender.hpp>

#include <memory>
#include <string>
#include <optional>

namespace Routers
{
    inline void enable_cors(attender::response_handler* res)
    {
        res->set("Access-Control-Allow-Origin", "*");
        res->set("Access-Control-Allow-Methods", "GET,PUT,POST,HEAD,OPTIONS");
    }

    /**
     * @param allow Example "POST, HEAD"
     **/
    template <typename T>
    inline void cors_options(T& server, std::string const& path, std::string const& allow)
    {
        server.options(path, [allow](auto req, auto res)
        {
            res->set("Allow", allow + ", OPTIONS");
            res->set("Connection", "keep-alive");
            res->set("Access-Control-Allow-Headers", "*");
            enable_cors(res);
            res->status(204).end();
        });
    }

    template <typename T>
    void jsonResponse(attender::response_handler* res, T&& resp)
    {
        res->type("application/json").send(resp.dump());
    }

    class BasicRouter
    {
    public:
        BasicRouter(RouterCollection* collection);
        ~BasicRouter() = default;

    protected:
        void respondWithError(attender::response_handler* res, int status, char const* msg);
        void readExcept(boost::system::error_code ec);

        RouterCollection* collection_;
    };

    // helpers
    template <typename ReqT, typename ResT, typename ActionT>
    void readJsonBody(ReqT req, ResT res, ActionT&& action)
    {
        auto data = std::make_shared <std::string>();
        req->read_body(*data).then(
            [data, res, action{std::move(action)}]()
            {
                try
                {
                    action(json::parse(*data));
                }
                catch(std::exception const& exc)
                {
                    res->status(400).send(exc.what());
                }
            }
        );
    }

    template <typename ServerT, typename ReqT>
    std::optional <Session> getSession(ServerT& server, ReqT* req)
    {
        auto* manager = server.get_session_manager();
        Session s;
        if (manager != nullptr)
        {
            auto status = manager->load_session("aSID", &s, req);
            if (status != attender::session_state::live)
                return std::nullopt;
        }
        else
            throw std::runtime_error("session control not installed");
        return s;
    }

    template <typename ServerT>
    void setSession(ServerT& server, Session const& s)
    {
        auto* manager = server.get_session_manager();
        if (manager != nullptr)
            manager->save_session(s);
        else
            throw std::runtime_error("session control not installed");
    }

    template <typename ResT>
    void sendJson(ResT res, json const& j)
    {
        res->status(200);
        jsonResponse(res, j);
    }

    /**
     *  Only reads size indicated json and then returns without reading the rest.
     */
    template <typename ReqT, typename ResT, typename ActionT>
    void readPartialJson(ReqT req, ResT res, ActionT&& action)
    {
        auto data = std::make_shared <std::string>();
        req->read_body(*data).then(
            [data, res, action{std::move(action)}]()
            {
                try
                {
                    action(json::parse(*data));
                }
                catch(std::exception const& exc)
                {
                    res->status(400).send(exc.what());
                }
            }
        );
    }

    template <typename ReqT, typename ResT, typename ActionT>
    void readTextBody(ReqT req, ResT res, ActionT&& action)
    {
        auto data = std::make_shared <std::string>();
        req->read_body(*data).then(
            [data, res, action{std::move(action)}]()
            {
                try
                {
                    action(*data);
                }
                catch(std::exception const& exc)
                {
                    res->status(400).send(exc.what());
                }
            }
        );
    }
}
