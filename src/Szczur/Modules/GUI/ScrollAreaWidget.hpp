#pragma once

#include <SFML/Graphics.hpp>

#include "Widget.hpp"
#include "Szczur/Modules/GUITest/Scroller.hpp"

namespace rat {
    class Script;
    class ScrollAreaWidget : public Widget {
    public:
        ScrollAreaWidget();

        static void initScript(Script& script);

        void setScrollerTexture(sf::Texture* texture, int boundsHeight = 140);
        void setPathTexture(sf::Texture* texture);
        void setBoundsTexture(sf::Texture* texture);        

        void setScrollSpeed(float speed);
        float getScrollSpeed() const;
    protected:
        virtual void _draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        virtual void _update(float deltaTime) override;
		virtual void _input(const sf::Event& event) override;
        virtual sf::Vector2u _getSize() const override;
		virtual void _calculateSize() override;

        //virtual void _inputChildren(sf::Event event) override;
        virtual sf::Vector2f _getChildrenShift() const override;
        virtual sf::Vector2u _getChildrenSize() override;
		virtual void _drawChildren(sf::RenderTarget& target, sf::RenderStates states) const override;        
    private:
        mutable sf::RenderTexture _renderTexture;
        sf::Transform _childrenTransform;
        float _scrollSpeed;
        float _offset;
        Scroller _scroller;
        sf::Vector2u _minScrollSize{30, 50};

        float _childrenHeight{0.f};
        float _childrenHeightProp{1.f};

        virtual void _callback(CallbackType type) override;
    };
}