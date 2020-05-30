#pragma once

#include <set>
#include <mutex>

namespace Streaming
{
    /**
     *  The class efficiency relies on id_type staying low and not much ids are generated per time.
     *  Which is the case for this program.
     */
    class IdProvider
    {
    public:
        using id_type = unsigned int;
        using id_parameter = id_type; // no const&, cause unsigned int.

    public:
        id_type acquireId();
        void freeId(id_parameter id);
        std::size_t usedIdCount() const;

    private:
        std::mutex protect_;
        std::set <id_type> freeIds_;
        id_type increment_;
    };
}
