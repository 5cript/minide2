#pragma once

#include <string>
#include <variant>
#include <vector>
#include <functional>

namespace Toolbars
{
    namespace Types
    {
        struct BaseElement
        {
            std::string type;
            std::string id;
            std::string helpText;

            BaseElement(std::string type, std::string id, std::string helpText)
                : type(std::move(type))
                , id(std::move(id))
                , helpText(std::move(helpText))
            {
            }

            ~BaseElement() = default;
        };
    }

    class BasicToolbar
    {
    public:
        BasicToolbar(std::string name)
            : name_{std::move(name)}
        {
        }

        ~BasicToolbar() = default;

        void name(std::string const& name);
        std::string name() const;
    private:
        std::string name_;
    };
}
