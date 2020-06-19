#include "session.hpp"

#include <iostream>

//#####################################################################################################################
Session::Session(std::string id)
    : attender::session{std::move(id)}
    , dataId{-1}
    , controlId{-1}
    , workspace{}
{

}
//---------------------------------------------------------------------------------------------------------------------
void Session::dump() const
{
    std::cout << "[Dumping Session]\n";
    std::cout << "dataId: " << dataId << "\n";
    std::cout << "controlId: " << controlId << "\n";
    std::cout << "workspace.root: " << workspace.root << "\n";
    std::cout << "workspace.activeProject: " << workspace.activeProject << "\n";
    std::cout << "#toolbars loaded: " << toolbarStore.scriptedToolbars.size() << "\n";

    std::cout << "\n";
}
//#####################################################################################################################
