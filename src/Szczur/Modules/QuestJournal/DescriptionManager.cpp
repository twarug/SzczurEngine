#include "DescriptionManager.hpp"
namespace rat
{
namespace journal
{
    DescriptionManager::DescriptionManager(sf::Font *font,InterfaceWidget* interface)
    :_font(font)
    {
        _scroller = new ScrollAreaWidget;
        _list = new ListWidget;
        interface->add(_scroller);
        _scroller->setPropSize(0.7f, 0.6f);
        _scroller->setPropPosition(0.94f, 0.9f);
        _list->setBetweenPadding(20.f);
        _list->setPropSize(0.6f, 0.f);
        _list->makeReversed();
        _scroller->add(_list);
    }

    DescriptionManager::~DescriptionManager()
    {

    }

    void DescriptionManager::refresh()
    {
        _list->clear();
        _descriptions = std::make_shared<std::vector<std::string> >(_quest->getDescription());
        TextWidget* widget;
        for(auto i = _descriptions->begin();i!=_descriptions->end();i++)
        {
            widget = new TextWidget;
            widget->setString(*i);
            widget->setFont(_font);
            widget->setCharacterSize(17);
            widget->setColor(sf::Color(135, 89, 247 ,255));
            _list->add(widget);
        }
        _scroller->resetScrollerPosition();

    }

    void DescriptionManager::setQuest(std::shared_ptr<Quest> quest)
    {
        _list->clear();
        _quest = quest;
        _descriptions = std::make_shared<std::vector<std::string> >(quest->getDescription());
        TextWidget* widget;
        for(auto i = _descriptions->begin();i!=_descriptions->end();i++)
        {
            widget = new TextWidget;
            widget->setString(*i);
            widget->setFont(_font);
            widget->setCharacterSize(17);
            widget->setColor(sf::Color(135, 89, 247 ,255));
            _list->add(widget);
        }
    }

    void DescriptionManager::setScrollTextures(sf::Texture* t1,sf::Texture* t2,sf::Texture* t3)
    {
        _scroller->setScrollerTexture(t1);
        _scroller->setPathTexture(t2);
        _scroller->setBoundsTexture(t3);
    }

    void DescriptionManager::clear()
    {
        _list->clear();
    }
}
}