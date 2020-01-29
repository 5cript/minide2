#include "basic_toolbar.hpp"

namespace Toolbars
{
//#####################################################################################################################
    std::string BasicToolbar::name() const
    {
        return name_;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string BasicToolbar::uuid() const
    {
        return uuid_;
    }
//---------------------------------------------------------------------------------------------------------------------
    BasicToolbar::ActorType* BasicToolbar::actorById(std::string const& id)
    {
        auto index = actorIndexById(id);
        if (index == -1)
            return nullptr;
        return &actors_[index];
    }
//---------------------------------------------------------------------------------------------------------------------
    long BasicToolbar::actorIndexById(std::string const& id)
    {
        for (std::size_t i = 0; i != actors_.size(); ++i)
        {
            auto res = std::visit([&id](auto& a) -> bool
            {
                return a.id == id;
            }, actors_[i]);

            if (res)
                return static_cast <long> (i);
        }
        return -1;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::vector <BasicToolbar::ActorType>& BasicToolbar::getActors()
    {
        return actors_;
    }
//#####################################################################################################################
}
