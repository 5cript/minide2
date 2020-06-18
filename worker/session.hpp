#pragma once

#include <attender/attender/session/session.hpp>
#include <string>

struct Session : public attender::session
{
    std::string dummy;
    int dataId;
    int controlId;

    Session(std::string id = "")
        : attender::session{std::move(id)}
        , dummy{}
        , dataId{-1}
        , controlId{-1}
    {
    }
};
