#pragma region
#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <string>
#include <SFML/Graphics.hpp>
#include "tileset.hpp"
#include "camera.hpp"
#include "draw.hpp"
#include "camera.hpp"
#include "pref.hpp"

// spritesheet works like this: spriteSheet[direction][animation][frame] = texture

class Player {
public:
    std::string name;
    sf::Texture currentTexture;
    sf::Texture**** spriteSheet;
    Tileset** worlds;
    Preferences* pref;
    Camera* camera;
    int** animationLengths; // animationLengths[direction][animation] = number of frames

    int animation = 0; // current animation
    int frame = 0; // current frame of the animation

    int speed = 4;

    int currentMenu;

    float animationTimer = 0.0f;

    int world;
    int position[2];
    int direction;
    
    int getArrayLength(sf::Texture** array) {
        int length = 0;
        while (array[length] != nullptr) {
            length++;
        }
        return length;
    };

    int getArrayLength(sf::Texture*** array) {
        int length = 0;
        while (array[length][0] != nullptr) {
            length++;
        }
        return length;
    };

    void draw(sf::RenderWindow& window) {
        sf::Texture* texture = spriteSheet[direction][0][0]; // default to the first frame of the first animation
        drawSprite(window, *texture, position[0] * 32 + camera->x, position[1] * 32 + camera->y);
    }

    void move(int dx, int dy) {
        position[0] += dx;
        position[1] += dy;
    }

    void setInteraction() {
        int x = position[0];
        int y = position[1];

        if(direction == 1){
            y += 1;
        }
        else if(direction == 2){
            x += 1;
        }
        else if(direction == 3){
            x += 1;
            y += 1;
        }

        worlds[world]->tiles[x + y * worlds[world]->xlen].interact = 1;
    }

    void setCamera() {
        camera->x = -position[0] * 32 + 400;
        camera->y = -position[1] * 32 + 300;
    }

    void setCurrentMenu(int menu) {
        currentMenu = menu;
    }

        // Implement animation logic here, e.g., update the sprite based on the animation and f
    void update() {
        setCamera();
    }

    void playerInit(const std::string& name, sf::Texture**** spriteSheet, Tileset** worlds, Preferences* pref, Camera* camera, int** animationLengths) {
        this->name = name;
        this->spriteSheet = spriteSheet;
        this->worlds = worlds;
        this->pref = pref;
        this->camera = camera;
        this->animationLengths = animationLengths;
        this->currentTexture = *spriteSheet[0][0][0];
        currentMenu = 0;
        world = 0;
        position[0] = 0;
        position[1] = 0;
        direction = 0;
    }

    void playerUpdate() {
        setCamera();
    }

    void playerMove(int dx, int dy) {
        move(dx, dy);
    }

    void playerSetInteraction() {
        setInteraction();
    }

    sf::Texture getAnimationFrame(int animationa, int frame) {
        sf::Texture currentTexturea = *spriteSheet[direction][animationa][frame];
        return currentTexturea;
    }

    void playAnimation(int animationa, sf::Clock clock){
        float time = getDeltaTime(clock);
        if (animationTimer >= 0.2f) {
            frame++;
            int frameCount = 4; // fallback
            if(animationa < getArrayLength(spriteSheet[direction]) && frame < getArrayLength(spriteSheet[direction][animationa])){
                frameCount = animationLengths[direction][animationa];

                if(frame >= getArrayLength(spriteSheet[direction][animationa])){
                    frame = 0;
                }

                animation = animationa;

            }

            currentTexture = getAnimationFrame(animationa, frame);

            animationTimer = 0.0f;
        }else{
            animationTimer += time;
        }
    }
} Player;

#endif
#pragma endregion