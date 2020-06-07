#include "streamer.hpp"

#include "../streaming/common_messages/welcome.hpp"
#include "../streaming/common_messages/binary_data.hpp"

#include <attender/encoding/streaming_producer.hpp>
#include <attender/encoding/brotli.hpp>

#include <thread>
#include <chrono>
#include <tuple>

using namespace std::chrono_literals;
using namespace std::string_literals;

namespace Routers
{
//#####################################################################################################################
    DataStreamer::DataStreamer(RouterCollection* collection, attender::tcp_server& server, Config const& config)
        : BasicRouter{collection}
        , endAllStreams_{false}
        , controlStream_{}
        , dataStream_{}
        , maxStreamListeners_{config.maxStreamListeners}
        , safeIdChecks_{config.streamIdCheck}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    DataStreamer::~DataStreamer()
    {
        shutdownAll();
    }
//---------------------------------------------------------------------------------------------------------------------
    void DataStreamer::shutdownAll()
    {
        endAllStreams_.store(true);
    }
//---------------------------------------------------------------------------------------------------------------------
    void DataStreamer::broadcast(StreamChannel channel, Streaming::Message&& msg)
    {
        switch (channel)
        {
            case(StreamChannel::Control): return controlStream_.queue.broadcastMessage(std::move(msg));
            case(StreamChannel::Data): return dataStream_.queue.broadcastMessage(std::move(msg));
            default: return;
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    int DataStreamer::send(StreamChannel channel, std::string const& addr, int id, Streaming::Message&& msg)
    {
        auto doWithChannel = [&](auto& stream)
        {
            if (safeIdChecks_)
            {
                std::lock_guard <std::mutex> guard{stream.addressRegisterMutex};
                auto iter = stream.remoteAddresses.find(id);
                if (iter == std::end(stream.remoteAddresses))
                    return -2;

                if (iter->second != addr)
                    return -1;
            }

            stream.queue.sendMessage(id, std::move(msg));

            return 0;
        };

        switch (channel)
        {
            case(StreamChannel::Control): return doWithChannel(controlStream_);
            case(StreamChannel::Data): return doWithChannel(dataStream_);
            default: return -3;
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void DataStreamer::registerRoutes(attender::tcp_server& server)
    {
        using namespace attender;

        /**
         *  Setup function for stream requests
         */
        auto commonStreamSetup = [this](auto req, auto res, auto& stream) -> int
        {
            enable_cors(res);

            if (stream.idProvider.usedIdCount() >= static_cast <std::size_t> (maxStreamListeners_))
            {
                res->status(409).send("Too much active listeners");
                return -1;
            }

            auto id = stream.idProvider.acquireId();
            {
                std::lock_guard <std::mutex> guard{stream.addressRegisterMutex};
                std::string addr = req->ip();
                stream.remoteAddresses.insert(std::pair{id, addr});
            }
            stream.queue.insertRetriever(id);

            return static_cast <int> (id);
        };

        /**
         *  Consume Loop for stream requests
         */
        auto consumeLoop = [this](auto id, auto& stream, auto& produ)
        {
            while(produ->has_consumer_attached())
            {
                if (endAllStreams_.load())
                {
                    // this ends the stream.
                    produ->finish();
                    return;
                }

                if (stream.queue.consumeableCount(id) == 0)
                {
                    std::this_thread::sleep_for(100ms);
                    continue;
                }

                auto iter = stream.queue.popMessage(id);
                writeMessage(*produ, iter->msg);
                stream.queue.unrefMessage(iter);
            }
        };

        /**
         *  Common cleanup function for stream requests.
         */
        auto commonCleanup = [this](auto& stream, auto id, auto& connectionBasedStreamer)
        {
            stream.queue.eraseRetriever(id);
            stream.idProvider.freeId(id);

            {
                std::lock_guard <std::mutex> guard{stream.addressRegisterMutex};
                stream.remoteAddresses.erase(id);
            }

            // on end
            if (connectionBasedStreamer->joinable())
                connectionBasedStreamer->join();
        };
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        server.get("/api/streamer/control", [this, commonStreamSetup, consumeLoop, commonCleanup](auto req, auto res)
        {
            auto id = commonStreamSetup(req, res, controlStream_);
            if (id == -1) return;

            std::shared_ptr <streaming_producer> produ;
            produ.reset(new streaming_producer
                (
                    "identity",
                    [](){},
                    [](auto){}
                )
            );

            std::shared_ptr <std::thread> connectionBasedStreamer{new std::thread([produ, id, this, consumeLoop]()
            {
                produ->wait_for_consumer();

                Streaming::Messages::Welcome welcome;
                welcome.id = id;

                if (produ->has_consumer_attached())
                    writeMessage
                    (
                        *produ,
                        Streaming::Message
                        {
                            std::make_unique <Streaming::Messages::Welcome>(std::move(welcome))
                        }
                    );

                consumeLoop(id, controlStream_, produ);
            })};

            res->send_chunked(*produ, [produ, connectionBasedStreamer, id, commonCleanup, this](auto e)
            {
                commonCleanup(controlStream_, id, connectionBasedStreamer);
                std::cout << "on finish\n";
            });
        });
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        server.get("/api/streamer/data", [this, commonStreamSetup, consumeLoop, commonCleanup](auto req, auto res)
        {
            auto id = commonStreamSetup(req, res, dataStream_);
            if (id == -1) return;

            std::shared_ptr <brotli_encoder> produ;
            produ.reset(new brotli_encoder);

            std::shared_ptr <std::thread> connectionBasedStreamer{new std::thread([produ, id, consumeLoop, this]()
            {
                produ->wait_for_consumer();

                Streaming::Messages::Welcome welcome;
                welcome.id = id;

                if (produ->has_consumer_attached())
                    writeMessage
                    (
                        *produ,
                        Streaming::Message
                        {
                            std::make_unique <Streaming::Messages::Welcome>(std::move(welcome))
                        }
                    );

                while(produ->has_consumer_attached())
                {
                    if (endAllStreams_.load())
                    {
                        // this ends the stream.
                        produ->finish();
                        return;
                    }

                    if (dataStream_.queue.consumeableCount(id) == 0)
                    {
                        std::this_thread::sleep_for(100ms);
                        continue;
                    }

                    auto iter = dataStream_.queue.popMessage(id);
                    std::cout << "msg pop \n";
                    writeMessage(*produ, iter->msg);
                    produ->flush();
                    dataStream_.queue.unrefMessage(iter);
                }
            })};

            res->send_chunked(*produ, [produ, connectionBasedStreamer, commonCleanup, id, this](auto e)
            {
                commonCleanup(dataStream_, id, connectionBasedStreamer);
            });
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void DataStreamer::respondWithError(attender::response_handler* res, char const* msg)
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void DataStreamer::readExcept(boost::system::error_code ec)
    {

    }
//#####################################################################################################################
}
