#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include <array>
#include <cstddef>
#include <cstdio>
#include <optional>
#include <SFML/Graphics.hpp>

inline void handleInput(sf::RenderWindow& window) {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            std::printf("Key pressed: %d\n", static_cast<int>(key->code));
        }
        if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            std::printf("Mouse button pressed: %d\n", static_cast<int>(mouse->button));
        }
    }
}

inline bool onKeyPress(int key) {
    static std::array<bool, static_cast<std::size_t>(sf::Keyboard::KeyCount)> keys = {};
    if (key < 0 || key >= static_cast<int>(keys.size())) {
        return false;
    }

    const auto sfKey = static_cast<sf::Keyboard::Key>(key);
    if (sf::Keyboard::isKeyPressed(sfKey)) {
        if (!keys[key]) {
            keys[key] = true;
            return true;
        }
    } else {
        keys[key] = false;
    }
    return false;
}

inline bool onMouseButtonPress(int button) {
    static std::array<bool, static_cast<std::size_t>(sf::Mouse::ButtonCount)> buttons = {};
    if (button < 0 || button >= static_cast<int>(buttons.size())) {
        return false;
    }

    const auto sfButton = static_cast<sf::Mouse::Button>(button);
    if (sf::Mouse::isButtonPressed(sfButton)) {
        if (!buttons[button]) {
            buttons[button] = true;
            return true;
        }
    } else {
        buttons[button] = false;
    }
    return false;
}

inline bool isKeyPressed(int key) {
    if (key < 0 || key >= static_cast<int>(sf::Keyboard::KeyCount)) {
        return false;
    }
    return sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(key));
}

inline bool isMouseButtonPressed(int button) {
    if (button < 0 || button >= static_cast<int>(sf::Mouse::ButtonCount)) {
        return false;
    }
    return sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(button));
}

inline bool isMouseMoved() {
    static int lastX = 0;
    static int lastY = 0;
    const sf::Vector2i mousePos = sf::Mouse::getPosition();
    if (mousePos.x != lastX || mousePos.y != lastY) {
        lastX = mousePos.x;
        lastY = mousePos.y;
        return true;
    }
    return false;
}

inline sf::Vector2i getMousePosition() {
    return sf::Mouse::getPosition();
}

inline sf::Vector2i getMousePosition(const sf::Window& window) {
    return sf::Mouse::getPosition(window);
}

#endif
