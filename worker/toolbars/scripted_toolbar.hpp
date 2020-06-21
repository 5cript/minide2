#pragma once

#include "basic_toolbar.hpp"

#include "../json.hpp"
#include "../filesystem/filesystem.hpp"

#include <string>
#include <memory>

namespace Toolbars
{
    class ScriptedToolbar : public BasicToolbar
    {
    public:
        ScriptedToolbar(sfs::path const& root);
        ~ScriptedToolbar();

        std::string id() const;

        std::string clickAction(std::string const& itemId);

        json getJson() const;

    private:
        void initialize();
        void passOptions();

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
