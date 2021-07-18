#include "communication_center.hpp"

//#####################################################################################################################
CommunicationCenter::CommunicationCenter(attender::http_server* server, Config const& config)
    : server_{server}
    , authenticator_{this, *server, config}
    , workspace_{this, *server, config}
    , toolbar_{this, *server, config}
    , httpStreamer_{this, *server, config}
    , terminal_{this, *server, config}
    , settingsProvider_{this, *server, config}
    , debuggerRouter_{this, *server, config}
    , wsStreamer_{this, server->get_io_context(), config}
{
}
//---------------------------------------------------------------------------------------------------------------------
CommunicationCenter::~CommunicationCenter() = default;
//---------------------------------------------------------------------------------------------------------------------
Routers::DataStreamer& CommunicationCenter::httpStreamer()
{
    return httpStreamer_;
}
//---------------------------------------------------------------------------------------------------------------------
Streaming::WebsocketStreamer& CommunicationCenter::streamer()
{
    return wsStreamer_;
}
//---------------------------------------------------------------------------------------------------------------------
Routers::SettingsProvider& CommunicationCenter::settingsProv()
{
    return settingsProvider_;
}
//#####################################################################################################################
