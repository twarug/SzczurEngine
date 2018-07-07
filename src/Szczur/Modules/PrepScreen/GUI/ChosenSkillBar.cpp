#include "ChosenSkillBar.hpp"

#include "Szczur/Modules/PrepScreen/PrepScreen.hpp"

#include "Szczur/Modules/GUI/GUI.hpp"
#include "Szczur/Modules/GUI/ImageWidget.hpp"

#include "Szczur/Utility/Logger.hpp"

namespace rat
{
    ChosenSkillBar::ChosenSkillBar(PrepScreen& prepScreen)
    :
    _prepScreen(prepScreen)
    {
        _icon = new ImageWidget;
        _addWidget(_icon);
        _icon->setPropSize(0.07f, 0.07f);
         // zabawa z rectami nowa funkcja ;V

        //_border = new ImageWidget;
        //_addWidget(_border);
        //_border->setPropSize(0.07f, 0.07f);

        _icon->setCallback(Widget::CallbackType::onHoverIn, [this](auto){
            if(hasSkill())
            {
                _icon->setColorInTime({125, 125, 125}, 0.5f);
            }

        });

        _icon->setCallback(Widget::CallbackType::onHoverOut ,[this](auto){
            if(hasSkill())
            {
                _icon->setColorInTime({255, 255, 255}, 0.5f);
            }
        });

        _icon->setCallback(Widget::CallbackType::onRelease, [this](auto){
            _onClick();
        });
    }

    void ChosenSkillBar::initAssetsViaGUI(GUI& gui)
    {
        //_border->setTexture(gui.getAsset<sf::Texture>("Assets/Test/ChosenSkill.png"));
    }

    void ChosenSkillBar::setSkill(const Skill* skill)
    {
        if(skill == nullptr)
        {
            _hasSkill = false;
            _icon->removeTexture();
        }
        else
        {
            _hasSkill = true;
            auto* texture = skill->getTexture();
            _icon->setTexture(texture);
            _icon->setPropTextureRect({{0.25f, 0.f}, {0.5f, 1.f}});
        }
        _skill = skill;
    }
    void ChosenSkillBar::removeSkill()
    {
        setSkill(nullptr);
    }
    bool ChosenSkillBar::hasSkill() const
    {
        return _hasSkill;
    }
    bool ChosenSkillBar::isFree() const
    {
        return !_hasSkill;
    }
    
    void ChosenSkillBar::_onClick()
    {
        if(hasSkill()) _prepScreen.returnSkill(_skill);
    }

    void ChosenSkillBar::swapSkillsWith(ChosenSkillBar& other)
    {
        const Skill* othersSkill = other._skill;

        std::cout << "Skill: " << (_skill ? _skill->getName() : "nullptr")
        << " swaped into Skill: " << (othersSkill ? othersSkill->getName() : "nullptr") << "\n";


        other.setSkill(_skill);
        setSkill(othersSkill);
    }
}