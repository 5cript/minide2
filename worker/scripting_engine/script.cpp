#include "script.hpp"

#include <fstream>
#include <string>

using namespace std::string_literals;

namespace MinIDE::Scripting
{
//#####################################################################################################################
    Script::Script(sfs::path const& scriptFile)
    {
        std::ifstream reader{scriptFile, std::ios_base::binary};

        if (!reader.good())
            throw std::runtime_error("Cannot find script file: "s + scriptFile.string());

        reader.seekg(0, std::ios::end);
        script_.resize(reader.tellg());
        reader.seekg(0);
        reader.read(&script_[0], script_.size());
    }
//---------------------------------------------------------------------------------------------------------------------
    Script::Script(std::string string)
        : script_{std::move(string)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    Script::Script()
        : script_{}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    std::string_view Script::viewScript()
    {
        return script_;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string const& Script::script() const
    {
        return script_;
    }
//#####################################################################################################################
}
