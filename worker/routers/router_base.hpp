#pragma once

#include "../routers_fwd.hpp"

#include <attender/attender.hpp>

namespace Routers
{
    inline void enable_cors(attender::response_handler* res)
    {
        res->set("Access-Control-Allow-Origin", "*");
        res->set("Access-Control-Allow-Methods", "GET,PUT,POST,HEAD");
    }

    template <typename T>
    void json_response(attender::response_handler* res, T&& resp)
    {
        enable_cors(res);
        res->type("application/json").send(resp.dump());
    }

    class BasicRouter
    {
    public:
        BasicRouter(RouterCollection* collection);
        ~BasicRouter() = default;

    protected:
        void respondWithError(attender::response_handler* res, char const* msg);
        void readExcept(boost::system::error_code ec);

        RouterCollection* collection_;
    };
}
