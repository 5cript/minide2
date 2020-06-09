#include "hybrid_read_sink.hpp"

#include <sstream>

//#####################################################################################################################
    JsonDataHybridSink::JsonDataHybridSink
    (
        std::function <void(json const&)> onJsonComplete,
        std::function <void(char const*, size_type)> onData,
        std::function <void(std::string const&)> onExpectationFailure,
        std::size_t contentLength
    )
        : auxBuffer_{}
        , jsonSize_{0}
        , contentLength_{contentLength}
        , onJsonComplete_{std::move(onJsonComplete)}
        , onData_{std::move(onData)}
        , onExpectationFailure_{std::move(onExpectationFailure)}
        , automat_{}
    {
        setupAutomaton();
    }
//---------------------------------------------------------------------------------------------------------------------
    void JsonDataHybridSink::setupAutomaton()
    {
        using namespace MiniAutomata;

        // [[State Declaration]]
        automat_ = makeAutomaton()
            << "Initial"
            << "SizeComplete"
            << "SeparatorComplete"
            << "JsonComplete"
            << "DataPhase"
            << "ExpectationFailure"
        ;

        // [[Transitions]]
        automat_ >
            "Initial" > [this](){return auxBuffer_.size() >= 10;} > "SizeComplete"
        ;
        automat_ >
            "SizeComplete" > [this](){return auxBuffer_.size() >= 1 && auxBuffer_[0] == '|';} > "SeparatorComplete"
        ;
        automat_ >
            "SizeComplete" > [this](){return auxBuffer_.size() >= 1 && auxBuffer_[0] != '|';} > "ExpectationFailure"
        ;
        automat_ >
            "SeparatorComplete" > [this](){return auxBuffer_.size() >= jsonSize_;} > "JsonComplete" > "DataPhase"
        ;

        // [[Actions]]
        automat_["SizeComplete"].bindAction([this]()
        {
            // precondition is met, so that auxbuffer is at least 10 bytes.
            jsonSize_ = std::stoll(auxBuffer_.substr(0, 10), nullptr, 16);
            if (jsonSize_ + 11 > contentLength_)
                onExpectationFailure_("Content-Length not large enough to fit supposed json data");
            auxBuffer_.erase(0, 10);
        });
        automat_["SeparatorComplete"].bindAction([this]()
        {
            auxBuffer_.erase(0, 1);
        });
        automat_["JsonComplete"].bindAction([this]()
        {
            try
            {
                onJsonComplete_(json::parse(auxBuffer_.substr(0, jsonSize_)));
                if (auxBuffer_.size() > jsonSize_)
                    onData_(auxBuffer_.data() + jsonSize_, auxBuffer_.size() - jsonSize_);
                auxBuffer_.clear();
            }
            catch(std::exception const& exc)
            {
                onExpectationFailure_(exc.what());
            }
        });
        automat_["ExpectationFailure"].bindAction([this]()
        {
            onExpectationFailure_("data stream expectation failure");
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    attender::size_type JsonDataHybridSink::write(const char* data, attender::size_type size)
    {
        // cannot have 0 states, so value is always assigned.
        auto activeState = automat_.getCurrentStateName().value();
        if (activeState != "DataPhase")
        {
            auxBuffer_.append(data, size);

            bool didWork = false;
            do
            {
                didWork = automat_.advance();
            } while(didWork);
            return size;
        }

        onData_(data, size);
        return size;
    }
//---------------------------------------------------------------------------------------------------------------------
    attender::size_type JsonDataHybridSink::write(std::vector <char> const& buffer, attender::size_type amount)
    {
        return write(buffer.data(), amount);
    }
//---------------------------------------------------------------------------------------------------------------------
    void JsonDataHybridSink::write(std::string const& data)
    {
        write(data.data(), static_cast <attender::size_type> (data.size()));
    }
//#####################################################################################################################
