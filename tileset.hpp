#ifndef TILESET_HPP
#define TILESET_HPP

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "camera.hpp"

struct Tile {
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;
    int collision = 0;
    int interact = 0;
};

struct Tileset {
    int ylen = 0;
    int xlen = 0;
    int tileWidth = 32;
    int tileHeight = 32;
    std::vector<Tile> tiles;

    Tile& at(int y, int x) {
        return tiles.at(static_cast<std::size_t>(y * xlen + x));
    }

    const Tile& at(int y, int x) const {
        return tiles.at(static_cast<std::size_t>(y * xlen + x));
    }
};

inline void drawTileset(sf::RenderTarget& target, const Tileset& tileset, const Camera& camera) {
    for (int y = 0; y < tileset.ylen; y++) {
        for (int x = 0; x < tileset.xlen; x++) {
            const Tile& tile = tileset.at(y, x);
            if (!tile.texture) {
                continue;
            }

            sf::Sprite sprite = tile.sprite;
            sprite.setPosition(
                {
                    x * static_cast<float>(tileset.tileWidth) + camera.x,
                    y * static_cast<float>(tileset.tileHeight) + camera.y
                }
            );
            target.draw(sprite);
        }
    }
}

inline Tileset* loadTileset(const char* filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << '\n';
        return nullptr;
    }

    auto tileset = std::make_unique<Tileset>();
    if (!(file >> tileset->ylen >> tileset->xlen)) {
        std::cerr << "Error reading tileset size: " << filename << '\n';
        return nullptr;
    }

    if (tileset->ylen <= 0 || tileset->xlen <= 0) {
        std::cerr << "Tileset size must be positive: " << filename << '\n';
        return nullptr;
    }

    tileset->tiles.resize(static_cast<std::size_t>(tileset->ylen * tileset->xlen));

    for (int y = 0; y < tileset->ylen; y++) {
        for (int x = 0; x < tileset->xlen; x++) {
            std::string spriteFilename;
            int collision = 0;
            int interact = 0;

            if (!(file >> spriteFilename >> collision >> interact)) {
                std::cerr << "Error reading tile at " << y << ", " << x << " from " << filename << '\n';
                return nullptr;
            }

            Tile& tile = tileset->at(y, x);
            tile.collision = collision;
            tile.interact = interact;

            if (spriteFilename == "null") {
                continue;
            }

            auto texture = std::make_shared<sf::Texture>();
            if (!texture->loadFromFile(spriteFilename)) {
                std::cerr << "Error loading texture: " << spriteFilename << '\n';
                continue;
            }

            tile.texture = texture;
            tile.sprite.setTexture(*tile.texture);
        }
    }

    return tileset.release();
}

inline void freeTileset(Tileset* tileset) {
    delete tileset;
}

#endif
