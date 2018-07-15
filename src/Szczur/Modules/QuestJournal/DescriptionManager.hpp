#include <Szczur/Modules/GUI/GUI.hpp>
#include <Szczur/Modules/GUI/TextAreaWidget.hpp>
#include "Szczur/Modules/GUI/ScrollAreaWidget.hpp"
#include "Szczur/Modules/GUI/ListWidget.hpp"

#include "Quest.hpp"

#include <SFML/graphics.hpp>

namespace rat
{
    
class TextAreaWidget; class ScrollAreaWidget;class ListWidget;

namespace journal
{
    class Quest;
    class DescriptionManager
    {
        public:

            DescriptionManager(sf::Font *font,Widget* interface);
            ~DescriptionManager();

            void refresh();
            void setScrollTextures(sf::Texture *t1,sf::Texture *t2,sf::Texture *t3);

            void setQuest(std::shared_ptr<Quest> quest);

            void clear();
        private:
            std::shared_ptr<std::vector<std::string> > _descriptions;
            sf::Font* _font;
            ScrollAreaWidget *_scroller;
            std::shared_ptr<Quest> _quest;
            ListWidget *_list;
    };
}
}