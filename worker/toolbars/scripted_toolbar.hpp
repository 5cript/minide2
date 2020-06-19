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

        void onClick(int id) override;

        json getJson() const;

    private:
        void initialize();

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
