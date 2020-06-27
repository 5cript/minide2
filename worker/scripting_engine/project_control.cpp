#include "project_control.hpp"

#include <fstream>
#include <sstream>

namespace MinIDE::Scripting
{
//#####################################################################################################################
    struct LuaProjectControl::Implementation
    {
        std::weak_ptr <StateCollection> weakStateRef;
        SessionObtainer sessionAccess;

        Implementation(std::weak_ptr <StateCollection>&& stateRef, SessionObtainer&& sessionAccess)
            : weakStateRef{std::move(stateRef)}
            , sessionAccess{std::move(sessionAccess)}
        {
        }
    };
//#####################################################################################################################
    LuaProjectControl::LuaProjectControl(std::weak_ptr <StateCollection> weakStateRef, SessionObtainer sessionAccess)
        : impl_{new LuaProjectControl::Implementation(std::move(weakStateRef), std::move(sessionAccess))}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    LuaProjectControl::~LuaProjectControl() = default;
//---------------------------------------------------------------------------------------------------------------------
    std::string LuaProjectControl::getProjectDirectory() const
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return "";
        return s.value().workspace.activeProject.string();
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string LuaProjectControl::getWorkspaceDirectory() const
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return "";
        return s.value().workspace.root.string();
    }
//---------------------------------------------------------------------------------------------------------------------
    bool LuaProjectControl::createMinIDEDirectory() const
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return false;
        auto p = s.value().workspace.activeProject;
        if (!sfs::exists(p))
            return false; // do not create a project directory, thats not what this is for

        if (!sfs::exists(p / ".minIDE"))
            sfs::create_directory(p / ".minIDE");
        return sfs::exists(p / ".minIDE");
    }
//---------------------------------------------------------------------------------------------------------------------
    void LuaProjectControl::reloadInformation()
    {
        impl_->sessionAccess.reload();
    }
//---------------------------------------------------------------------------------------------------------------------
    bool LuaProjectControl::hasMinIDEDirectory() const
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return false;
        auto p = s.value().workspace.activeProject;

        return sfs::exists(p / ".minIDE");
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string LuaProjectControl::getMinIDEDirectory() const
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return "";
        auto p = s.value().workspace.activeProject / ".minIDE";
        if (!sfs::exists(p))
            return "";
        return p.string();
    }
//---------------------------------------------------------------------------------------------------------------------
    bool LuaProjectControl::hasActiveProject() const
    {
        return getProjectDirectory() != "";
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string LuaProjectControl::readProjectFile(std::string const& fileName)
    {
        sfs::path project{getMinIDEDirectory()};
        if (project.string() == "")
            return "";

        auto settingsFile = project / fileName;
        if (!sfs::exists(settingsFile))
            return "";

        std::ifstream reader{settingsFile.string(), std::ios_base::binary};
        if (!reader.good())
            return "";

        std::stringstream sstr;
        sstr << reader.rdbuf();
        return sstr.str();
    }
//---------------------------------------------------------------------------------------------------------------------
    int LuaProjectControl::saveProjectFile(std::string const& fileName, std::string const& jsonString)
    {
        sfs::path project{getMinIDEDirectory()};
        if (project.string() == "")
            return -1;

        std::string formatted;
        try
        {
            formatted = json::parse(jsonString).dump(4);
        }
        catch(...)
        {
            return -3;
        }

        auto settingsFile = project / fileName;
        std::ofstream writer{settingsFile.string(), std::ios_base::binary};
        if (!writer.good())
            return -2;

        writer.write(formatted.c_str(), formatted.size());
        return 0;
    }
//#####################################################################################################################
    void loadProjectControl(std::weak_ptr <StateCollection> state, SessionObtainer sessionAccess)
    {
        auto strongRef = state.lock();
        if (!strongRef)
            return;
        std::lock_guard <StateCollection::mutex_type> {strongRef->globalMutex};

        auto usertype = strongRef->lua.new_usertype<LuaProjectControl>
        (
            "ProjectControl",
            "new", sol::initializers
            (
                [state, sessionAccess{std::move(sessionAccess)}](LuaProjectControl& p) -> void
                {
                    new (&p) LuaProjectControl(state, std::move(sessionAccess));
                }
            ),
            "get_project_directory", &LuaProjectControl::getProjectDirectory,
            "get_workspace_directory", &LuaProjectControl::getWorkspaceDirectory,
            "create_meta_directory", &LuaProjectControl::createMinIDEDirectory,
            "has_meta_directory", &LuaProjectControl::hasMinIDEDirectory,
            "get_meta_directory", &LuaProjectControl::getMinIDEDirectory,
            "has_active_project", &LuaProjectControl::hasActiveProject,
            "update", &LuaProjectControl::reloadInformation,
            "read_project_file", &LuaProjectControl::readProjectFile,
            "save_project_file", &LuaProjectControl::saveProjectFile
        );
    }
//#####################################################################################################################
}