#pragma once

struct Config
{
    int maxStreamListeners = 100;

    /**
     *  Check that the remote_address of a request is the same as the reciepient address for streaming responses.
     **/
    bool streamIdCheck = true;

    long long maxFileReadSize = 1 * 1024 * 1024; // 1 MB max file.
    long long maxFileReadSizeUnforceable = 100 * 1024 * 1024; // 100 MB max forceable file.
    long long fileChunkSize = 10 * 1024;

    // maybe make this user dependent in the future.
    long long maxFileWriteSize = 10 * 1024 * 1024;

    unsigned short port = 43255;

    unsigned short httpThreadCount = 4;

    // regex to verify Origin
    std::string corsOption = ".*";
};
