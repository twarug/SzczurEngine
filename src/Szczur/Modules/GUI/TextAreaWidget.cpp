#include "TextAreaWidget.hpp"
#include <iostream>
#include "Test.hpp"
namespace rat {
    TextAreaWidget::TextAreaWidget() :
    _size(0u, 0u),
    _toWrap(false) {

    }

    TextAreaWidget::TextAreaWidget(sf::Vector2u size, sf::Font* font) :
    _size(size),
    _toWrap(false) {
        _text.setFont(*font);
    }

    void TextAreaWidget::initScript(Script& script) {
        auto object = script.newClass<TextAreaWidget>("TextAreaWidget", "GUI");
        //auto object = script.newClass<ImageWidget>("ImageWidget", "GUI");
        //Widget::basicScript<ImageWidget>(object);
        basicScript(object);

        object.setProperty(
            "font",
            [](TextAreaWidget& owner){owner._text.getFont();},
            [](TextAreaWidget& owner, sf::Font* font){owner.setFont(font);}
        );

        object.setProperty(
            "text",
            [](TextAreaWidget& owner){return owner._text.getString();},
            [](TextAreaWidget& owner, const std::string& text){owner.setString(text);}
        );

        object.setProperty(
            "fontSize",
            [](TextAreaWidget& owner){return owner._text.getCharacterSize();},
            [](TextAreaWidget& owner, size_t size){owner.setCharacterSize(size);}
        );

        object.setProperty(
            "color",
            [](TextAreaWidget& owner){return owner._text.getFillColor();},
            [](TextAreaWidget& owner, sol::table tab){ owner.setColor( sf::Color(tab[1], tab[2], tab[3]) ); }
        );

        object.setProperty(
            "size",
            [](TextAreaWidget& owner){return owner._size;},
            [](TextAreaWidget& owner, sol::table tab){ owner.setSize(sf::Vector2u{tab[1], tab[2]}); }
        );
        
        object.init();
    }

    void TextAreaWidget::setString(const std::string& text) {
        _text.setString(text);
        _toWrap = true;
    }

    void TextAreaWidget::setSize(sf::Vector2u size) {
        _size = size;
        _toWrap = true;
    }

    void TextAreaWidget::setFont(sf::Font* font) {
        _text.setFont(*font);
    }

    void TextAreaWidget::setCharacterSize(size_t size) {
        _text.setCharacterSize(size);
        _toWrap = true;
    }

    void TextAreaWidget::setColor(const sf::Color& color) {
        _text.setFillColor(color);
    }

    void TextAreaWidget::_draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(_text, states);
    }

    void TextAreaWidget::_wrapText() {
        std::string temp = _text.getString().toAnsiString();
        temp.erase(std::remove_if(temp.begin(), temp.end(), [](auto& it){return it=='\n';}), temp.end());
        for(auto i = _size.x; i<temp.size(); i+=_size.x) {
            int x = i;
            while(temp[x]!=' ') {
                if(--x == 0) {
                    x=i;
                    break;
                }
            }
            if(temp[x] == ' ')
                temp[x] = '\n';
            else
                temp.insert(x, 1, '\n');
        }
        _text.setString(temp);
    }

    void TextAreaWidget::_update(float deltaTime) {
        if(_toWrap) {
            _toWrap = false;
            _wrapText();
        }
        
    }

    sf::Vector2u TextAreaWidget::_getSize() const {
        auto rect = _text.getGlobalBounds();
        return {
            static_cast<unsigned int>(rect.left + rect.width),
            static_cast<unsigned int>(rect.top + rect.height)
        };
    }

    void TextAreaWidget::_callback(CallbackType type) {
        if(auto it = _luaCallbacks.find(type); it != _luaCallbacks.end())
            std::invoke(it->second, this);
        if(auto it = _callbacks.find(type); it != _callbacks.end())
            std::invoke(it->second, this);
    }
}