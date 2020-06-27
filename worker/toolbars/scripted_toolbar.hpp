#pragma once

#include "basic_toolbar.hpp"
#include "../session/session_fwd.hpp"

#include "../public_settings.hpp"
#include "../workspace/workspace.hpp"
#include "../routers/streamer_fwd.hpp"

#include "../json.hpp"
#include "../filesystem/filesystem.hpp"

#include <string>
#include <memory>

namespace Toolbars
{
    class ScriptedToolbar : public BasicToolbar
    {
    public:
        ScriptedToolbar(sfs::path const& root, SessionObtainer const& obtainer, Routers::DataStreamer* streamer);
        ~ScriptedToolbar();

        std::string id() const;
        std::string clickAction(std::string const& itemId);
        std::string menuAction(std::string const& itemId, std::string const& menuEntryLabel);
        std::string loadCombobox(std::string const& itemId);
        std::string comboboxSelect(std::string const& itemId, std::string const& selected);

        json getJson() const;

    private:
        void initialize(SessionObtainer const& obtainer, Routers::DataStreamer* streamer);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
