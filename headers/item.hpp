#ifndef ITEM_HPP
#define ITEM_HPP

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include "pref.hpp"
#include "draw.hpp"

struct Item {
    std::string name;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;
    int interact = 0;
};

void initItem(Item& item, const std::string& name, const std::shared_ptr<sf::Texture>& texture) {
    item.name = name;
    item.texture = texture;
    item.sprite.setTexture(*texture);
}

void drawItem(sf::RenderWindow& window, const Item& item) {
    drawSprite(window, item.sprite);
}

#endif
