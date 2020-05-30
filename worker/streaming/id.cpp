#include "id.hpp"

#include <limits>
#include <stdexcept>

namespace Streaming
{
//#####################################################################################################################
    IdProvider::id_type IdProvider::acquireId()
    {
        std::lock_guard <std::mutex> guard{protect_};
        if (freeIds_.empty())
        {
            [[likely]] if (increment_ != std::numeric_limits <id_type>::max())
                ++increment_;
            else
            {
                if (freeIds_.empty())
                    throw std::overflow_error("ran out of ids");
            }
        }
        else
        {
            auto iter = freeIds_.begin();
            auto res = *iter;
            freeIds_.erase(iter);
            return res;
        }
        return increment_;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::size_t IdProvider::usedIdCount() const
    {
        return increment_ - freeIds_.size();
    }
//---------------------------------------------------------------------------------------------------------------------
    void IdProvider::freeId(id_parameter id)
    {
        std::lock_guard <std::mutex> guard{protect_};
        freeIds_.insert(id);
    }
//#####################################################################################################################
}
