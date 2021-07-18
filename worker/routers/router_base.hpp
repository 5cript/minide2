#pragma once

#include "../routers_fwd.hpp"
#include "../json.hpp"
#include "../session/session.hpp"
#include "../session/temporary_session.hpp"

#include <attender/attender.hpp>

#include <memory>
#include <string>
#include <optional>

namespace Routers
{
    /**
     *  Allow CORS on everything.
     */
    inline void enable_cors(attender::request_handler* req, attender::response_handler* res, std::string const& originString)
    {
        auto origin = req->get_header_field("Origin");
        if (origin)
        {
            res->set("Access-Control-Allow-Origin", origin.value());
            res->set("Access-Control-Allow-Methods", "GET,PUT,POST,HEAD,OPTIONS");
            res->set("Access-Control-Allow-Credentials", "true");
            res->set("Access-Control-Allow-Headers", "Authorization, Content-Type");
        }
    }

    /**
     * @param allow Example "POST, HEAD"
     **/
    template <typename T>
    inline void cors_options(T& server, std::string const& path, std::string const& allow, std::string const& originString)
    {
        server.options(path, [allow, originString](auto req, auto res)
        {
            res->set("Allow", allow + ", OPTIONS");
            res->set("Connection", "keep-alive");
            enable_cors(req, res, originString);
            res->status(204).end();
        });
    }

    template <typename T>
    void jsonResponse(attender::response_handler* res, T&& resp)
    {
        res->type("application/json").send(resp.dump());
    }

    /**
     *  Every router has this as a base class.
     */
    class BasicRouter
    {
    public:
        using server_type = attender::http_server;

    public:
        BasicRouter(CommunicationCenter* collection, server_type* server);
        ~BasicRouter() = default;

    protected:
        void respondWithError(attender::response_handler* res, int status, char const* msg);
        void respondWithError(attender::response_handler* res, std::string msg);
        void respondWithError(attender::response_handler* res, std::string msg, json additionalInfo);
        void readExcept(boost::system::error_code ec);

        /**
         *  Obtaining the session this way is safer than manually getting it.
         */
        TemporarySession this_session(attender::request_handler* req);

        CommunicationCenter* collection_;
        server_type* server_;
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
        return std::optional <Session>{s};
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
