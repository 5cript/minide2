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
    }

    class BasicToolbar
    {
    public:
        using ActorType = std::variant <
            Types::IconButton,
            Types::ComboBox
        >;

        BasicToolbar(std::string uuid, std::string name)
            : uuid_{std::move(uuid)}
            , name_{std::move(name)}
        {
        }

        ~BasicToolbar() = default;

        virtual void onClick(int id) = 0;

        std::string name() const;
        std::string uuid() const;

        /**
         *  Returns nullptr if not found.
         */
        ActorType* actorById(std::string const& id);

        /**
         *  Returns -1 if not found.
         */
        long actorIndexById(std::string const& id);

        std::vector <ActorType>& getActors();

    protected:
        std::vector <ActorType> actors_;

    private:
        std::string uuid_;
        std::string name_;
    };
}
