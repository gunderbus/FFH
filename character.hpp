#pragma region
#ifndef CHARACTER_HPP
#define CHARACTER_HPP
#include <string>
#include <SFML/Graphics.hpp>
#include "tileset.hpp"
#include "camera.hpp"
#include "draw.hpp"

typedef struct Character {
    std::string name;
    sf::Texture texture;

    int position[2];
    std::string* dialogue;
    int direction;
} Character;

void drawCharacter(sf::RenderWindow& window, Character& character, int x, int y, Tileset& tileset) {
    sf::Texture texture = character.texture;
    drawSprite(window, texture, x, y);
}

void setInteraction(Character& character, Tileset& tileset){
    int x = character.position[0];
    int y = character.position[1];

    if(character.direction == 1){
        y += 1;
    }
    else if(character.direction == 2){
        x += 1;
    }
    else if(character.direction == 3){
        x += 1;
        y += 1;
    }

    tileset.tiles[x + y * tileset.xlen].interact = 1;
}

#endif
#pragma endregion