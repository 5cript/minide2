#pragma once

#include "basic_toolbar.hpp"
#include "../session/session_fwd.hpp"

#include "../public_settings.hpp"
#include "../workspace/workspace.hpp"
#include "../routers/streamer_fwd.hpp"
#include "../routers/settings_provider_fwd.hpp"

#include "../json.hpp"
#include "../filesystem/filesystem.hpp"
#include "../config.hpp"
#include "../fallible.hpp"

#include <string>
#include <memory>

namespace Toolbars
{
    struct ToolbarApiError
    {
        enum class ErrorType
        {
            LockTimeout,
            Various
        };

        std::optional <std::string> message;
        ErrorType type;

        ToolbarApiError(std::string const& message)
            : message{message}
            , type{ErrorType::Various}
        {
        }

        std::string error_message() const
        {
            if (type == ErrorType::LockTimeout)
                return "lock timed out, some other action is in progress";

            if (message)
                return message.value();
            else
                return "no error message";
        }

        ToolbarApiError(ErrorType type)
            : message{}
            , type{type}
        {
        }
    };

    class ScriptedToolbar : public BasicToolbar
    {
    public:
        ScriptedToolbar
        (
            sfs::path const& root,
            SessionObtainer const& obtainer,
            Routers::DataStreamer* streamer,
            Routers::SettingsProvider* settingsProv,
            Config const& config
        );
        ~ScriptedToolbar();

        std::string id() const;
        Fallible <ToolbarApiError, void> clickAction(std::string const& itemId);
        Fallible <ToolbarApiError, void> menuAction(std::string const& itemId, std::string const& menuEntryLabel);
        Fallible <ToolbarApiError, void> loadCombobox(std::string const& itemId);
        Fallible <ToolbarApiError, void> comboboxSelect(std::string const& itemId, std::string const& selected);
        Fallible <ToolbarApiError, void> onLogDoubleClick(std::string const& logName, int lineNumber, std::string lineString);

        json getJson() const;

    private:
        void initialize
        (
            SessionObtainer const& obtainer,
            Routers::DataStreamer* streamer,
            Routers::SettingsProvider* settingsProv
        );

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
