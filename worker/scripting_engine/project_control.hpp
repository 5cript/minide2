#pragma once

#include "state.hpp"
#include "../session/session_obtainer.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace MinIDE::Scripting
{
    /**
     *  Exposed to lua.
     */
    class LuaProjectControl
    {
    public:
        LuaProjectControl(std::weak_ptr <StateCollection> weakStateRef, SessionObtainer sessionAccess);
        ~LuaProjectControl();

        /**
         *  Get directory of active project, may be the same as workspace dir
         */
        std::string getProjectDirectory() const;

        /**
         *  Get workspace directory, may be the same as the project directory.
         */
        std::string getWorkspaceDirectory() const;

        /**
         *  Create the .minIDE meta directory for project related persistence.
         */
        bool createMinIDEDirectory() const;

        /**
         *  A project has been selected and is active.
         */
        bool hasActiveProject() const;

        /**
         *  is there a project meta directory?
         */
        bool hasMinIDEDirectory() const;

        /**
         *  Get MinIDE Directory of the active project
         *  Creates it if it doesnt exist
         */
        std::string getMinIDEDirectory() const;

        /**
         *  Reload session. Having the most recent information might not be wise, if the user somehow changes the active project during a running script.
         */
        void reloadInformation();

        /**
         *  Reads the project file content as a string. Returns an empty string if the file doesn't exist or some other issue occurs
         */
        std::string readProjectFile(std::string const& fileName);

        /**
         *  Save string to file.
         */
        bool saveProjectFile(std::string const& fileName, std::string const& content);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };

    void loadProjectControl(std::weak_ptr <StateCollection> state, SessionObtainer sessionAccess);
}
