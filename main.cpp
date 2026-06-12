#include <SFML/Graphics.hpp>
#include "draw.hpp"
#include "input_handler.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "FFH");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        handleInput(window);

        window.clear(sf::Color(24, 28, 33));
        drawRect(window, 100.0f, 100.0f, 96.0f, 96.0f);
        drawLine(window, 100.0f, 220.0f, 196.0f, 220.0f);
        window.display();
    }

    return 0;
}
