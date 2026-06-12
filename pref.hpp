#pragma region
#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <SFML/Graphics.hpp>
#include "tileset.hpp"
#include "draw.hpp"

typedef struct Preferences {
    int cameraSpeed;
    int textSpeed;
    int enemySightRange;
    sf::Color textColor;
    sf::Texture textBoxTexture;
} Preferences;

#endif
#pragma endregion