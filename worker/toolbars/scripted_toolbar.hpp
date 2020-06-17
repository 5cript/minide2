#pragma once

#include "basic_toolbar.hpp"
#include "../scripting_engine/script.hpp"

#include <string>
#include <memory>

namespace Toolbars
{
    class ScriptedToolbar : public BasicToolbar
    {
    public:
        ScriptedToolbar(std::string uuid);

        void onClick(int id) override;

        void setupFromScript(MinIDE::Scripting::Script const& script);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
