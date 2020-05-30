#include "server_time.hpp"

#include <chrono>
#include <sstream>
#include <iomanip>

using namespace std::string_literals;

namespace Streaming::Messages
{
//#####################################################################################################################
    std::string ServerTime::toJson() const
    {
        std::stringstream sstr;
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&time);
        long long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        sstr << std::setfill('0')
                  << std::put_time(now_tm, "%FT%H:%M:")
                  << std::setw(2) << (timestamp / 1000) % 60 << '.'
                  << std::setw(3) << timestamp % 1000
                  << std::put_time(now_tm, "%z");

        return json{
            {"type", "time"s},
            {"iso8601", sstr.str()}
        }.dump();
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
