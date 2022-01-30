#pragma once

#include "json.hpp"

#include <attender/http/http_read_sink.hpp>
#include <automata/automata.hpp>

#include <functional>

class JsonDataHybridSink : public attender::http_read_sink
{
public:
    using size_type = attender::size_type;

    JsonDataHybridSink
    (
        std::function <void(json const&)> onJsonComplete,
        std::function <void(char const*, size_type)> onData,
        std::function <void(std::string const&)> onExpectationFailure,
        std::size_t contentLength
    );

    size_type write(const char* data, size_type size) override;
    size_type write(std::vector <char> const& buffer, size_type amount) override;
    void write(std::string const& data);

private:
    void setupAutomaton();

private:
    std::string auxBuffer_;
    std::size_t jsonSize_;
    std::size_t contentLength_; // json size cannot be bigger than this.

    std::function <void(json const&)> onJsonComplete_;
    std::function <void(char const*, size_type)> onData_;
    std::function <void(std::string const&)> onExpectationFailure_;

    // Parse State Transition Automaton
    MiniAutomata::Automaton automat_;
};
