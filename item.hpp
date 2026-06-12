#ifndef ITEM_HPP
#define ITEM_HPP

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

struct Item {
    std::string name;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;
    int interact = 0;
};

#endif
