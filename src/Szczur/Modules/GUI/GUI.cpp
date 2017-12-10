#include "GUI.hpp"

#include <iostream>
#include <fstream>

namespace rat {
    void GUI::init() {
        
        _assets.loadFromFile<sf::Texture>("data/button.png");
        _assets.loadFromFile<sf::Texture>("data/button-active.png");
        _assets.loadFromFile<sf::Texture>("data/button-clicked.png");
        _assets.loadFromFile<sf::Texture>("data/check.png");
        _assets.loadFromFile<sf::Texture>("data/check-on.png");
        _assets.loadFromFile<sf::Font>("data/consolab.ttf");
        _assets.loadFromFile<sf::Texture>("data/button.png");
        _assets.loadFromFile<Json>("data/json.json", [](Json& obj, const std::string& path){
            std::ifstream file(path);
            if(file.is_open()) {
                file >> obj;
                file.close();
                return true;
            }
            return false;
        });
        _guiJson.init(_assets.get<Json>("data/json.json"), &_assets, &_root, getModule<Canvas>().getWindow().getSize());
    }

    

    void GUI::input(const sf::Event& event) {
        if(
            event.type == sf::Event::MouseButtonPressed || 
            event.type == sf::Event::MouseButtonReleased || 
            event.type == sf::Event::MouseMoved ||
            event.type == sf::Event::TextEntered
        ) {
            _root.input(event);
        }
    }

    void GUI::update(float deltaTime) {
        _root.update(deltaTime);
    }

    void GUI::render() {
        getModule<Canvas>().draw(Canvas::LayerId::GUI, _root);
    }

    void GUI::reload() {
        _guiJson.reload(getModule<Canvas>().getWindow().getSize(), &_root);
    }
}