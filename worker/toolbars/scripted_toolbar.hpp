#pragma once

#include "basic_toolbar.hpp"
#include "../session/session_fwd.hpp"

#include "../public_settings.hpp"
#include "../workspace/workspace.hpp"

#include "../json.hpp"
#include "../filesystem/filesystem.hpp"

#include <string>
#include <memory>

namespace Toolbars
{
    class ScriptedToolbar : public BasicToolbar
    {
    public:
        ScriptedToolbar(sfs::path const& root, SessionObtainer const& obtainer);
        ~ScriptedToolbar();

        std::string id() const;
        std::string clickAction(std::string const& itemId);
        json getJson() const;

    private:
        void initialize(SessionObtainer const& obtainer);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
