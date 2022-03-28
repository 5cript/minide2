#include <backend/utility/uuid.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string makeUuid()
{
    return boost::uuids::to_string(boost::uuids::random_generator()());
}