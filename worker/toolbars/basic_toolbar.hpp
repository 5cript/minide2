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

        /*
        struct IconButton : BaseElement
        {
            std::string pngbase64;
            std::function <void()> onClick;

            IconButton(std::string type, std::string id, std::string helpText, std::string pngbase64, std::function <void()> onClick)
                : BaseElement(std::move(type), std::move(id), std::move(helpText))
                , pngbase64(std::move(pngbase64))
                , onClick{std::move(onClick)}
            {
            }
        };

        struct ComboBox : BaseElement
        {
            int selected;
            std::vector <std::string> options;

            ComboBox(std::string type, std::string id, std::string helpText, int selected, std::vector <std::string> options)
                : BaseElement(std::move(type), std::move(id), std::move(helpText))
                , selected(std::move(selected))
                , options(std::move(options))
            {
            }
        };

        struct Splitter : BaseElement
        {
            Splitter(std::string type, std::string id, std::string helpText, int selected, std::vector <std::string> options)
                : BaseElement(std::move(type), std::move(id), std::move(helpText))
            {
            }
        };

        struct MenuButton : BaseElement
        {
            MenuButton(std::string type, std::string id, std::string helpText, int selected, std::vector <std::string> options)
                : BaseElement(std::move(type), std::move(id), std::move(helpText))
            {
            }
        };
        */
    }

    class BasicToolbar
    {
    public:
        BasicToolbar(std::string name)
            : name_{std::move(name)}
        {
        }

        ~BasicToolbar() = default;

        virtual void onClick(int id) = 0;

        void name(std::string const& name);

        std::string name() const;

    private:
        std::string name_;
    };
}
