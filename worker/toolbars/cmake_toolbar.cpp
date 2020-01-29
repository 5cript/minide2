#include "cmake_toolbar.hpp"

#include <iostream>
#include <variant>

namespace Toolbars
{
    using namespace Types;
//#####################################################################################################################
    CMakeToolbar::CMakeToolbar(std::string uuid)
        : BasicToolbar(std::move(uuid), "CMake C/C++")
    {
        // Save
        actors_.emplace_back(IconButton{
            "IconButton", "save", "", "Save", [](){}
        });

        // Save All
        actors_.emplace_back(IconButton{
            "IconButton", "saveAll", "Save All", "", [](){}
        });

        // CMake
        actors_.emplace_back(IconButton{
            "IconButton", "cmake", "Run CMake", "", [](){}
        });

        // Build
        actors_.emplace_back(IconButton{
            "IconButton", "make", "Run Make", "", [](){std::cout << "MAKE!" << std::endl;}
        });

        // Run
        actors_.emplace_back(IconButton{
            "IconButton", "run", "Run Target", "", [](){}
        });

        // Build And Run
        actors_.emplace_back(IconButton{
            "IconButton", "buildRun", "Build and Run Target", "", [](){}
        });

        // Abort
        actors_.emplace_back(IconButton{
            "IconButton", "abort", "Abort", "", [](){}
        });

        // Debug Run
        actors_.emplace_back(IconButton{
            "IconButton", "debug", "Run Debugger", "", [](){}
        });

        // Debug NL
        actors_.emplace_back(IconButton{
            "IconButton", "nextLine", "Next Line", "", [](){}
        });

        // Debug Into
        actors_.emplace_back(IconButton{
            "IconButton", "stepInto", "Step Into", "", [](){}
        });

        // Debug Out Of
        actors_.emplace_back(IconButton{
            "IconButton", "stepOut", "Step Out Of", "", [](){}
        });

        // Project Select
        actors_.emplace_back(ComboBox{
            "ComboBox", "projectSelect", "$ProjectSelect", -1, {}
        });

        // Target Select
        actors_.emplace_back(ComboBox{
            "ComboBox", "targetSelect", "$TargetSelect", -1, {}
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void CMakeToolbar::onClick(int id)
    {
        if (id < 0 || id >= actors_.size())
            throw std::out_of_range("toolbar click oob");

        auto button = std::get_if <Types::IconButton>(&actors_[id]);
        std::cout << "clicked " << button->id << "\n";
        button->onClick();
    }
//#####################################################################################################################
}
