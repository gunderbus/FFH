#ifndef DRAW_HPP
#define DRAW_HPP

#include <SFML/Graphics.hpp>

inline bool loadTexture(sf::Texture& texture, const char* filename) {
    return texture.loadFromFile(filename);
}

inline void drawSprite(sf::RenderTarget& target, const sf::Texture& texture, float posx, float posy) {
    sf::Sprite sprite(texture);
    sprite.setPosition({posx, posy});
    target.draw(sprite);
}

inline void drawSprite(sf::RenderTarget& target, const sf::Sprite& sprite) {
    target.draw(sprite);
}

inline void drawText(sf::RenderTarget& target, const sf::Font& font, const char* text, float posx, float posy) {
    sf::Text sfText(font, text, 30);
    sfText.setPosition({posx, posy});
    target.draw(sfText);
}

inline void drawLine(sf::RenderTarget& target, float x1, float y1, float x2, float y2) {
    sf::Vertex line[] = {
        {{x1, y1}},
        {{x2, y2}}
    };
    target.draw(line, 2, sf::PrimitiveType::Lines);
}

inline void drawRect(sf::RenderTarget& target, float x, float y, float width, float height) {
    sf::RectangleShape rectangle(sf::Vector2f(width, height));
    rectangle.setPosition({x, y});
    target.draw(rectangle);
}

void getDeltaTime(sf::Clock& clock) {
    sf::Time deltaTime = clock.restart();
    float dt = deltaTime.asSeconds();
}

#endif
