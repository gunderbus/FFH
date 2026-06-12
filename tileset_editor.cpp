#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

constexpr int tileSize = 32;
constexpr float toolbarHeight = 76.0f;
constexpr float paletteWidth = 248.0f;
constexpr float paletteTile = 52.0f;

struct SpriteAsset {
    std::string path;
    std::shared_ptr<sf::Texture> texture;
};

struct EditorTile {
    std::string spritePath = "null";
    std::shared_ptr<sf::Texture> texture;
    int collision = 0;
    int interact = 0;
};

struct EditorMap {
    int rows = 15;
    int cols = 20;
    std::vector<EditorTile> tiles;

    EditorMap() {
        resize(rows, cols);
    }

    void resize(int newRows, int newCols) {
        rows = std::max(1, newRows);
        cols = std::max(1, newCols);
        tiles.resize(static_cast<std::size_t>(rows * cols));
    }

    EditorTile& at(int row, int col) {
        return tiles.at(static_cast<std::size_t>(row * cols + col));
    }
};

enum class Tool {
    Paint,
    Collision,
    Interact,
    Erase
};

std::string usage(const char* program) {
    std::ostringstream out;
    out << "Usage: " << program << " <sprite-folder> [save-file] [load-file] [rows] [cols]\n"
        << "Example: " << program << " assets/tiles maps/field.txt maps/field.txt 30 40\n";
    return out.str();
}

bool hasImageExtension(const fs::path& path) {
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga";
}

std::shared_ptr<sf::Texture> loadTexture(
    const std::string& path,
    std::map<std::string, std::shared_ptr<sf::Texture>>& cache
) {
    if (path == "null") {
        return nullptr;
    }

    const auto found = cache.find(path);
    if (found != cache.end()) {
        return found->second;
    }

    auto texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile(path)) {
        std::cerr << "Could not load texture: " << path << '\n';
        return nullptr;
    }

    cache[path] = texture;
    return texture;
}

std::vector<SpriteAsset> loadSpritesFromFolder(
    const fs::path& folder,
    std::map<std::string, std::shared_ptr<sf::Texture>>& cache
) {
    std::vector<SpriteAsset> sprites;
    if (!fs::is_directory(folder)) {
        std::cerr << "Sprite folder does not exist: " << folder << '\n';
        return sprites;
    }

    for (const auto& entry : fs::recursive_directory_iterator(folder)) {
        if (!entry.is_regular_file() || !hasImageExtension(entry.path())) {
            continue;
        }

        const std::string path = entry.path().lexically_normal().string();
        auto texture = loadTexture(path, cache);
        if (texture) {
            sprites.push_back({path, texture});
        }
    }

    std::sort(sprites.begin(), sprites.end(), [](const SpriteAsset& left, const SpriteAsset& right) {
        return left.path < right.path;
    });
    return sprites;
}

bool loadMap(
    const std::string& filename,
    EditorMap& map,
    std::map<std::string, std::shared_ptr<sf::Texture>>& cache
) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open tileset file: " << filename << '\n';
        return false;
    }

    int rows = 0;
    int cols = 0;
    if (!(file >> rows >> cols) || rows <= 0 || cols <= 0) {
        std::cerr << "Tileset file has an invalid size: " << filename << '\n';
        return false;
    }

    map.resize(rows, cols);
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            std::string spritePath;
            int collision = 0;
            int interact = 0;
            if (!(file >> spritePath >> collision >> interact)) {
                std::cerr << "Tileset file ended early at " << y << ", " << x << '\n';
                return false;
            }

            EditorTile& tile = map.at(y, x);
            tile.spritePath = spritePath;
            tile.texture = loadTexture(spritePath, cache);
            tile.collision = collision;
            tile.interact = interact;
        }
    }

    return true;
}

bool saveMap(const std::string& filename, const EditorMap& map) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Could not save tileset file: " << filename << '\n';
        return false;
    }

    file << map.rows << ' ' << map.cols << '\n';
    for (int y = 0; y < map.rows; y++) {
        for (int x = 0; x < map.cols; x++) {
            const EditorTile& tile = map.tiles.at(static_cast<std::size_t>(y * map.cols + x));
            file << tile.spritePath << ' ' << tile.collision << ' ' << tile.interact << '\n';
        }
    }

    return true;
}

std::optional<sf::Font> loadUiFont() {
    const std::vector<std::string> candidates = {
        "/home/gunderbus/FFH/fonts/DotGothic16-Regular.ttf",
        "fonts/DotGothic16-Regular.ttf"
    };

    for (const std::string& path : candidates) {
        sf::Font font;
        if (font.openFromFile(path)) {
            return font;
        }
    }

    return std::nullopt;
}

void drawText(
    sf::RenderTarget& target,
    const std::optional<sf::Font>& font,
    const std::string& text,
    sf::Vector2f position,
    unsigned int size = 15,
    sf::Color color = sf::Color::White
) {
    if (!font) {
        return;
    }

    sf::Text drawable(*font, text, size);
    drawable.setFillColor(color);
    drawable.setPosition(position);
    target.draw(drawable);
}

void drawTextureInRect(sf::RenderTarget& target, const sf::Texture& texture, const sf::FloatRect& rect) {
    sf::Sprite sprite(texture);
    const sf::Vector2u size = texture.getSize();
    if (size.x == 0 || size.y == 0) {
        return;
    }

    const float scale = std::min(rect.size.x / static_cast<float>(size.x), rect.size.y / static_cast<float>(size.y));
    sprite.setScale({scale, scale});
    sprite.setPosition({
        rect.position.x + (rect.size.x - static_cast<float>(size.x) * scale) * 0.5f,
        rect.position.y + (rect.size.y - static_cast<float>(size.y) * scale) * 0.5f
    });
    target.draw(sprite);
}

void paintTile(EditorTile& tile, const std::optional<std::size_t>& selectedSprite, const std::vector<SpriteAsset>& sprites) {
    if (!selectedSprite || *selectedSprite >= sprites.size()) {
        return;
    }

    const SpriteAsset& sprite = sprites.at(*selectedSprite);
    tile.spritePath = sprite.path;
    tile.texture = sprite.texture;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << usage(argv[0]);
        return 1;
    }

    const fs::path spriteFolder = argv[1];
    const std::string saveFile = argc >= 3 ? argv[2] : "tileset.txt";
    const std::string loadFile = argc >= 4 ? argv[3] : "";
    const int startRows = argc >= 5 ? std::max(1, std::atoi(argv[4])) : 15;
    const int startCols = argc >= 6 ? std::max(1, std::atoi(argv[5])) : 20;

    std::map<std::string, std::shared_ptr<sf::Texture>> textureCache;
    const std::vector<SpriteAsset> sprites = loadSpritesFromFolder(spriteFolder, textureCache);
    if (sprites.empty()) {
        std::cerr << "No sprites found in " << spriteFolder << '\n';
    }

    EditorMap map;
    map.resize(startRows, startCols);
    if (!loadFile.empty()) {
        loadMap(loadFile, map, textureCache);
    }

    sf::RenderWindow window(sf::VideoMode({1120, 720}), "FFH Tileset Editor");
    window.setFramerateLimit(60);

    std::optional<sf::Font> font = loadUiFont();
    sf::Vector2f camera(0.0f, 0.0f);
    float paletteScroll = 0.0f;
    Tool tool = Tool::Paint;
    std::optional<std::size_t> selectedSprite;
    bool showGrid = true;
    bool dragging = false;
    sf::Vector2i lastMouse;
    std::string status = "Ready";

    while (window.isOpen()) {
        const sf::Vector2u windowSize = window.getSize();
        const float mapWidth = static_cast<float>(windowSize.x) - paletteWidth;
        const sf::FloatRect paletteRect({mapWidth, toolbarHeight}, {paletteWidth, static_cast<float>(windowSize.y) - toolbarHeight});

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                const bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
                    || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);

                if (key->code == sf::Keyboard::Key::Escape) {
                    window.close();
                } else if (key->code == sf::Keyboard::Key::Num1) {
                    tool = Tool::Paint;
                    status = "Paint tool";
                } else if (key->code == sf::Keyboard::Key::Num2) {
                    tool = Tool::Collision;
                    status = "Collision tool";
                } else if (key->code == sf::Keyboard::Key::Num3) {
                    tool = Tool::Interact;
                    status = "Interaction tool";
                } else if (key->code == sf::Keyboard::Key::Num4 || key->code == sf::Keyboard::Key::Delete) {
                    tool = Tool::Erase;
                    status = "Erase tool";
                } else if (key->code == sf::Keyboard::Key::G) {
                    showGrid = !showGrid;
                } else if (key->code == sf::Keyboard::Key::S && ctrl) {
                    status = saveMap(saveFile, map) ? "Saved to " + saveFile : "Save failed";
                } else if (key->code == sf::Keyboard::Key::L && ctrl && !loadFile.empty()) {
                    status = loadMap(loadFile, map, textureCache) ? "Loaded " + loadFile : "Load failed";
                } else if (key->code == sf::Keyboard::Key::Equal) {
                    map.resize(map.rows + 1, map.cols + 1);
                    status = "Expanded map";
                } else if (key->code == sf::Keyboard::Key::Hyphen && map.rows > 1 && map.cols > 1) {
                    map.resize(map.rows - 1, map.cols - 1);
                    status = "Shrank map";
                }
            }

            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                if (paletteRect.contains(sf::Vector2f(wheel->position))) {
                    paletteScroll = std::max(0.0f, paletteScroll - wheel->delta * 42.0f);
                } else {
                    camera.y += wheel->delta * 48.0f;
                }
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                const sf::Vector2f position(mouse->position);
                if (mouse->button == sf::Mouse::Button::Middle) {
                    dragging = true;
                    lastMouse = mouse->position;
                }

                if (mouse->button == sf::Mouse::Button::Left) {
                    if (paletteRect.contains(position)) {
                        const int paletteCols = 4;
                        const int col = static_cast<int>((position.x - paletteRect.position.x) / paletteTile);
                        const int row = static_cast<int>((position.y - paletteRect.position.y + paletteScroll) / paletteTile);
                        const int index = row * paletteCols + col;
                        if (col >= 0 && col < paletteCols && index >= 0 && index < static_cast<int>(sprites.size())) {
                            selectedSprite = static_cast<std::size_t>(index);
                            tool = Tool::Paint;
                            status = "Selected " + fs::path(sprites.at(*selectedSprite).path).filename().string();
                        }
                    } else if (position.y >= toolbarHeight && position.x < mapWidth) {
                        const int col = static_cast<int>(std::floor((position.x - camera.x) / static_cast<float>(tileSize)));
                        const int row = static_cast<int>(std::floor((position.y - toolbarHeight - camera.y) / static_cast<float>(tileSize)));
                        if (row >= 0 && row < map.rows && col >= 0 && col < map.cols) {
                            EditorTile& tile = map.at(row, col);
                            if (tool == Tool::Paint) {
                                paintTile(tile, selectedSprite, sprites);
                            } else if (tool == Tool::Collision) {
                                tile.collision = tile.collision ? 0 : 1;
                            } else if (tool == Tool::Interact) {
                                tile.interact = tile.interact ? 0 : 1;
                            } else if (tool == Tool::Erase) {
                                tile.spritePath = "null";
                                tile.texture = nullptr;
                            }
                        }
                    }
                }

                if (mouse->button == sf::Mouse::Button::Right && position.y >= toolbarHeight && position.x < mapWidth) {
                    const int col = static_cast<int>(std::floor((position.x - camera.x) / static_cast<float>(tileSize)));
                    const int row = static_cast<int>(std::floor((position.y - toolbarHeight - camera.y) / static_cast<float>(tileSize)));
                    if (row >= 0 && row < map.rows && col >= 0 && col < map.cols) {
                        EditorTile& tile = map.at(row, col);
                        tile.spritePath = "null";
                        tile.texture = nullptr;
                    }
                }
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouse->button == sf::Mouse::Button::Middle) {
                    dragging = false;
                }
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
                if (dragging) {
                    camera.x += static_cast<float>(mouse->position.x - lastMouse.x);
                    camera.y += static_cast<float>(mouse->position.y - lastMouse.y);
                    lastMouse = mouse->position;
                }
            }
        }

        const float moveSpeed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ? 12.0f : 6.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            camera.x += moveSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            camera.x -= moveSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            camera.y += moveSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            camera.y -= moveSpeed;
        }

        window.clear(sf::Color(24, 28, 33));

        sf::RectangleShape toolbar({static_cast<float>(windowSize.x), toolbarHeight});
        toolbar.setFillColor(sf::Color(34, 39, 46));
        window.draw(toolbar);

        const std::string toolName = tool == Tool::Paint ? "Paint" : tool == Tool::Collision ? "Collision" : tool == Tool::Interact ? "Interact" : "Erase";
        const std::string selectedName = selectedSprite ? fs::path(sprites.at(*selectedSprite).path).filename().string() : "none";
        drawText(window, font, "1 Paint  2 Collision  3 Interact  4 Erase  G Grid  Ctrl+S Save  Middle-drag/WASD/Arrows Move  Wheel Scroll", {12.0f, 10.0f}, 14);
        drawText(window, font, "Tool: " + toolName + "   Selected: " + selectedName + "   Size: " + std::to_string(map.rows) + "x" + std::to_string(map.cols) + "   " + status, {12.0f, 38.0f}, 14, sf::Color(210, 224, 238));

        sf::RectangleShape mapBackground({mapWidth, static_cast<float>(windowSize.y) - toolbarHeight});
        mapBackground.setPosition({0.0f, toolbarHeight});
        mapBackground.setFillColor(sf::Color(20, 23, 28));
        window.draw(mapBackground);

        for (int y = 0; y < map.rows; y++) {
            for (int x = 0; x < map.cols; x++) {
                const float px = camera.x + static_cast<float>(x * tileSize);
                const float py = toolbarHeight + camera.y + static_cast<float>(y * tileSize);
                if (px > mapWidth || py > static_cast<float>(windowSize.y) || px + tileSize < 0.0f || py + tileSize < toolbarHeight) {
                    continue;
                }

                const EditorTile& tile = map.tiles.at(static_cast<std::size_t>(y * map.cols + x));
                sf::RectangleShape cell({static_cast<float>(tileSize), static_cast<float>(tileSize)});
                cell.setPosition({px, py});
                cell.setFillColor(sf::Color(29, 34, 40));
                window.draw(cell);

                if (tile.texture) {
                    drawTextureInRect(window, *tile.texture, {{px, py}, {static_cast<float>(tileSize), static_cast<float>(tileSize)}});
                }

                if (tile.collision) {
                    sf::RectangleShape overlay({static_cast<float>(tileSize), static_cast<float>(tileSize)});
                    overlay.setPosition({px, py});
                    overlay.setFillColor(sf::Color(220, 50, 50, 96));
                    window.draw(overlay);
                }

                if (tile.interact) {
                    sf::CircleShape marker(6.0f);
                    marker.setFillColor(sf::Color(80, 190, 255, 220));
                    marker.setPosition({px + 19.0f, py + 6.0f});
                    window.draw(marker);
                }
            }
        }

        if (showGrid) {
            const int minCol = std::max(0, static_cast<int>(std::floor((-camera.x) / tileSize)) - 1);
            const int maxCol = std::min(map.cols, static_cast<int>(std::ceil((mapWidth - camera.x) / tileSize)) + 1);
            const int minRow = std::max(0, static_cast<int>(std::floor((-camera.y) / tileSize)) - 1);
            const int maxRow = std::min(map.rows, static_cast<int>(std::ceil((static_cast<float>(windowSize.y) - toolbarHeight - camera.y) / tileSize)) + 1);

            sf::RectangleShape line;
            line.setFillColor(sf::Color(190, 205, 220, 62));
            for (int x = minCol; x <= maxCol; x++) {
                line.setSize({1.0f, static_cast<float>((maxRow - minRow) * tileSize)});
                line.setPosition({camera.x + static_cast<float>(x * tileSize), toolbarHeight + camera.y + static_cast<float>(minRow * tileSize)});
                window.draw(line);
            }
            for (int y = minRow; y <= maxRow; y++) {
                line.setSize({static_cast<float>((maxCol - minCol) * tileSize), 1.0f});
                line.setPosition({camera.x + static_cast<float>(minCol * tileSize), toolbarHeight + camera.y + static_cast<float>(y * tileSize)});
                window.draw(line);
            }
        }

        sf::RectangleShape paletteBackground({paletteWidth, static_cast<float>(windowSize.y)});
        paletteBackground.setPosition({mapWidth, 0.0f});
        paletteBackground.setFillColor(sf::Color(38, 43, 50));
        window.draw(paletteBackground);
        drawText(window, font, "Sprites", {mapWidth + 12.0f, 12.0f}, 18);
        drawText(window, font, std::to_string(sprites.size()) + " loaded", {mapWidth + 12.0f, 42.0f}, 13, sf::Color(190, 203, 216));

        const int paletteCols = 4;
        for (std::size_t i = 0; i < sprites.size(); i++) {
            const int row = static_cast<int>(i) / paletteCols;
            const int col = static_cast<int>(i) % paletteCols;
            const float x = paletteRect.position.x + static_cast<float>(col) * paletteTile + 6.0f;
            const float y = paletteRect.position.y + static_cast<float>(row) * paletteTile - paletteScroll + 6.0f;
            if (y > static_cast<float>(windowSize.y) || y + paletteTile < toolbarHeight) {
                continue;
            }

            sf::RectangleShape slot({44.0f, 44.0f});
            slot.setPosition({x, y});
            slot.setFillColor(sf::Color(25, 29, 35));
            slot.setOutlineThickness(selectedSprite && *selectedSprite == i ? 3.0f : 1.0f);
            slot.setOutlineColor(selectedSprite && *selectedSprite == i ? sf::Color(246, 201, 96) : sf::Color(92, 105, 118));
            window.draw(slot);
            drawTextureInRect(window, *sprites.at(i).texture, {{x + 3.0f, y + 3.0f}, {38.0f, 38.0f}});
        }

        window.display();
    }

    return 0;
}
