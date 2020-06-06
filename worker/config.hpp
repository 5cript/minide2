#pragma once

struct Config
{
    int maxStreamListeners = 100;

    /**
     *  Check that the remote_address of a request is the same as the reciepient address for streaming responses.
     **/
    bool streamIdCheck = true;
};
