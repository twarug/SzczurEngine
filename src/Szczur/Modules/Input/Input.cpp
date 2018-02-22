#include "Input.hpp"

namespace rat
{

Input::~Input()
{
    LOG_DESTRUCTOR();
}

void Input::processEvent(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed) {
        _pressKey(event.key.code);
    }
    else if (event.type == sf::Event::KeyReleased) {
        _releaseKey(event.key.code);
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        _pressButton(event.mouseButton.button);
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        _releaseButton(event.mouseButton.button);
    }
    else if (event.type == sf::Event::MouseWheelScrolled) {
        _moveWheel(event.mouseWheelScroll.wheel, event.mouseWheelScroll.delta);
    }
    else if (event.type == sf::Event::TextEntered) {
        _enteredCharacter = event.text.unicode;
    }
}

void Input::finishLogic()
{
    if (_recentlyPressed.isValid()) {
        switch (_recentlyPressed.getType()) {
            case InputDevice_e::Keyboard: _keyboard[_recentlyPressed] = InputStatus_e::Kept; break;
            case InputDevice_e::Mouse: _mouse[_recentlyPressed] = InputStatus_e::Kept; break;
            case InputDevice_e::Scroll: _scroll[_recentlyPressed] = InputStatus_e::Kept; break;
        }

        _recentlyPressed = InputCode();
    }

    if (_recentlyReleased.isValid()) {
        switch (_recentlyReleased.getType()) {
            case InputDevice_e::Keyboard: _keyboard[_recentlyReleased] = InputStatus_e::Unkept; break;
            case InputDevice_e::Mouse: _mouse[_recentlyReleased] = InputStatus_e::Unkept; break;
            case InputDevice_e::Scroll: _scroll[_recentlyReleased] = InputStatus_e::Unkept; break;
        }

        _recentlyReleased = InputCode();
    }

    _enteredCharacter = 0;
}

void Input::press(const InputCode& code)
{
    switch (code.getType()) {
        case InputDevice_e::Keyboard: _pressKey(code); break;
        case InputDevice_e::Mouse: _pressButton(code); break;
        case InputDevice_e::Scroll: _moveWheel(code % 2, code - 2); break;
    }
}

void Input::release(const InputCode& code)
{
    switch (code.getType()) {
        case InputDevice_e::Keyboard: _releaseKey(code); break;
        case InputDevice_e::Mouse: _releaseButton(code); break;
        case InputDevice_e::Scroll: _moveWheel(code % 2, code - 2); break;
    }
}

InputStatus_e Input::getStatus(const InputCode& code) const
{
    switch (code.getType()) {
        case InputDevice_e::Keyboard: return _keyboard[code];
        case InputDevice_e::Mouse: return _mouse[code];
        case InputDevice_e::Scroll: return _scroll[code];
    }

    return InputStatus_e::Unkept;
}

bool Input::checkStatus(const InputCode& code, InputStatus_e _Status) const
{
    return getStatus(code) == _Status;
}

bool Input::isUnkept(const InputCode& code) const
{
    return checkStatus(code, InputStatus_e::Unkept);
}

bool Input::isPressed(const InputCode& code) const
{
    return checkStatus(code, InputStatus_e::Pressed);
}

bool Input::isKept(const InputCode& code) const
{
    return checkStatus(code, InputStatus_e::Kept);
}

bool Input::isReleased(const InputCode& code) const
{
    return checkStatus(code, InputStatus_e::Released);
}

InputCode Input::getRecentlyPressed() const
{
    return _recentlyPressed;
}

InputCode Input::getRecentlyReleased() const
{
    return _recentlyReleased;
}

bool Input::isAnyPressed() const
{
    return _recentlyPressed.isValid();
}

bool Input::isAnyReleased() const
{
    return _recentlyReleased.isValid();
}

bool Input::isTextEntered() const
{
    return _enteredCharacter != 0;
}

unsigned Input::getEnteredCharacter() const
{
    return _enteredCharacter;
}

void Input::_pressKey(int id)
{
    if (_keyboard[id] != InputStatus_e::Kept) {
        _keyboard[id] = InputStatus_e::Pressed;
        _recentlyPressed = static_cast<Keyboard::Code_e>(id);
    }
}

void Input::_releaseKey(int id)
{
    if (_keyboard[id] != InputStatus_e::Unkept) {
        _keyboard[id] = InputStatus_e::Released;
        _recentlyReleased = static_cast<Keyboard::Code_e>(id);
    }
}

void Input::_pressButton(int id)
{
    if (_mouse[id] != InputStatus_e::Kept) {
        _mouse[id] = InputStatus_e::Pressed;
        _recentlyPressed = static_cast<Mouse::Code_e>(id);
    }
}

void Input::_releaseButton(int id)
{
    if (_mouse[id] != InputStatus_e::Unkept) {
        _mouse[id] = InputStatus_e::Released;
        _recentlyReleased = static_cast<Mouse::Code_e>(id);
    }
}

void Input::_moveWheel(int which, int delta)
{
    if (which == sf::Mouse::Wheel::VerticalWheel) {
        if (delta < 0) {
            _scroll[Scroll::Up]   = InputStatus_e::Released;
            _scroll[Scroll::Down] = InputStatus_e::Pressed;
            _recentlyPressed  = Scroll::Down;
            _recentlyReleased = Scroll::Up;
        }
        else {
            _scroll[Scroll::Up]   = InputStatus_e::Pressed;
            _scroll[Scroll::Down] = InputStatus_e::Released;
            _recentlyPressed  = Scroll::Up;
            _recentlyReleased = Scroll::Down;
        }
    }
    else if (which == sf::Mouse::Wheel::HorizontalWheel) {
        if (delta < 0) {
            _scroll[Scroll::Left]  = InputStatus_e::Released;
            _scroll[Scroll::Right] = InputStatus_e::Pressed;
            _recentlyPressed  = Scroll::Right;
            _recentlyReleased = Scroll::Left;
        }
        else {
            _scroll[Scroll::Left]  = InputStatus_e::Pressed;
            _scroll[Scroll::Right] = InputStatus_e::Released;
            _recentlyPressed  = Scroll::Left;
            _recentlyReleased = Scroll::Right;
        }
    }
}

}
