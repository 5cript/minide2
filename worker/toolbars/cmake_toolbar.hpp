#pragma once

#include "basic_toolbar.hpp"

namespace Toolbars
{
    class CMakeToolbar : public BasicToolbar
    {
    public:
        CMakeToolbar(std::string uuid);

        void onClick(int id) override;
    };
}
