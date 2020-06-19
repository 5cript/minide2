#include "routers.hpp"

//#####################################################################################################################
RouterCollection::RouterCollection(attender::tcp_server* server, Config const& config)
    : server_{server}
    , workspace_{this, *server, config}
    , toolbar_{this, *server, config}
    , streamer_{this, *server, config}
    , terminal_{this, *server, config}
    , settingsProvider_{this, *server, config}
{
}
//---------------------------------------------------------------------------------------------------------------------
void RouterCollection::endStreaming()
{
    streamer_.shutdownAll();
}
//---------------------------------------------------------------------------------------------------------------------
RouterCollection::~RouterCollection()
{
    endStreaming();
}
//---------------------------------------------------------------------------------------------------------------------
Routers::DataStreamer& RouterCollection::streamer()
{
    return streamer_;
}
//#####################################################################################################################
