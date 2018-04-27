#pragma once

#include <SFML/Graphics.hpp>

namespace rat
{
    class Scroller2 : public sf::Drawable
    {
    public:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        void setPosition(float x, float y);
        void setPosition(const sf::Vector2f& position);
        void setPathTexture(const sf::Texture* texture);
        void setScrollerTexture(const sf::Texture* texture);
        void setBoundTexture(const sf::Texture* texture);

        void setSize(const sf::Vector2i& size);
        void setWidthProportion(float proportion);
        void setBoundShiftProportion(float proportion);

        void setProportion(float proportion);
    private:

        sf::Sprite _path;
        sf::Sprite _scroller;

        sf::Sprite _upperBound;
        sf::Sprite _bottomBound;
        float _boundShiftProp{0.7f};   

        bool _areBoundsSet{false};     

        float _widthProp{1.f};
        int _scrollerLength{60};

        sf::Vector2i _size{0, 0};
        sf::Vector2f _position{0.f, 0.f};

        bool _isScrollerSet{false};
        bool _isPathSet{false};

        float _pathLength{130.f};
        float _scrollerPos{0.f};

        float _proportion{0.f};   

        void _recalcScroller();

        void _recalcPathSize();
        void _recalcPathPos();
        void _recalcScrollerSize();
        void _recalcScrollerPos();
        void _recalcScrollerPosByProp();
        void _recalcSize();

        void _recalcBoundPos();
        void _recalcBoundSize();

        float _getRealPathLength() const;
        float _getRealBoundLength() const;

        void _recalcAll();

    };
}