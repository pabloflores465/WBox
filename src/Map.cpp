#include "Map.h"
#include "Plant.h"
#include "Obstacle.h"
#include "../include/FastNoise/Cpp/FastNoiseLite.h"
#include <iostream>
#include <thread>
#include <mutex>       // Inclusión para std::mutex
#include <SFML/Graphics.hpp>

using std::cerr, std::cout, std::endl, std::make_unique, std::vector, std::unique_ptr, std::mutex, std::lock_guard;
using sf::RenderWindow, sf::Texture, sf::Sprite, sf::Vector2u;
using namespace MapParameters;

void generateMap(const TileType tile1, const TileType tile2, const TileType tile3, const TileType tile4, const FastNoiseLite::NoiseType noiseType) {

    cout << "Generating map..." << endl;

    FastNoiseLite noise;
    noise.SetNoiseType(noiseType);

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            float value = noise.GetNoise(static_cast<float>(x), static_cast<float>(y));

            if (value < -0.3f) {
                map[y][x].type = tile1;
            } else if (value < 0.0f)
                map[y][x].type = tile2;
            else if (value < 0.5f) {
                map[y][x].type = tile3;
            } else {
                map[y][x].type = tile4;
            }

            map[y][x].occupied = false;
            map[y][x].hasPlant = false;
            map[y][x].hasObstacle = false;

        }
    }

    for (int y = 1; y < MAP_HEIGHT - 1; ++y) {
        for (int x = 1; x < MAP_WIDTH - 1; ++x) {
            if (map[y][x].type == WATER) {
                if (map[y - 1][x].type == GRASS) map[y - 1][x].type = SAND;
                if (map[y + 1][x].type == GRASS) map[y + 1][x].type = SAND;
                if (map[y][x - 1].type == GRASS) map[y][x - 1].type = SAND;
                if (map[y][x + 1].type == GRASS) map[y][x + 1].type = SAND;
            }
        }
    }

    // Semilla para generación aleatoria
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < 50; ++i) {
        {
            const int x = rand() % MAP_WIDTH;
            const int y = rand() % MAP_HEIGHT;
            lock_guard lock(map[y][x].cellMutex);
            if (map[y][x].type == GRASS && map[y][x].type != SAND && !map[y][x].hasPlant && !map[y][x].hasObstacle) {
                auto plant = make_unique<Plant>(x * TILE_SIZE, y * TILE_SIZE);
                plant->startLifeCycle();
                plants.push_back(std::move(plant));
                numberPlants ++;
                map[y][x].hasPlant = true;
                cout << y << " " << x << " " << map[y][x].hasPlant << endl;
            }
        }
    }

    for (int i = 0; i < 30; ++i) {
        {
            const int x = rand() % MAP_WIDTH;
            const int y = rand() % MAP_HEIGHT;
            lock_guard lock(map[y][x].cellMutex);
            if (map[y][x].type != WATER && map[y][x].type != SAND && !map[y][x].hasObstacle && !map[y][x].hasPlant) {
                auto obstacle = make_unique<Obstacle>(x * TILE_SIZE, y * TILE_SIZE);
                obstacles.push_back(std::move(obstacle));
                numberObstacles ++;
                map[y][x].hasObstacle = true;
            }
        }
    }

    cout << "Map generated." << endl;
}

void renderMap(RenderWindow &window) {
    Sprite sprite;

    const Vector2u windowSize = window.getSize();

    Texture grassTexture, waterTexture, forestTexture, sandTexture;

    if (!grassTexture.loadFromFile("../public/environments/grass.jpg")) {
        cerr << "Error loading grass texture." << endl;
    }
    if (!waterTexture.loadFromFile("../public/environments/water.png")) {
        cerr << "Error loading water texture." << endl;
    }
    if (!forestTexture.loadFromFile("../public/environments/forest.jpg")) {
        cerr << "Error loading forest texture." << endl;
    }
    if (!sandTexture.loadFromFile("../public/environments/sand.png")) {
        cerr << "Error loading sand texture." << endl;
    }

    const float tileWidth = static_cast<float>(windowSize.x) / MAP_WIDTH;
    const float tileHeight = static_cast<float>(windowSize.y) / MAP_HEIGHT;

    // Establecer la escala una vez, suponiendo que todas las texturas son del mismo tamaño
    sprite.setScale(tileWidth / static_cast<float>(grassTexture.getSize().x), tileHeight / static_cast<float>(grassTexture.getSize().y));

    const Texture* currentTexture = nullptr;

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            switch (map[y][x].type) {
                case WATER:
                    currentTexture = &waterTexture;
                    break;
                case GRASS:
                    currentTexture = &grassTexture;
                    break;
                case FOREST:
                    currentTexture = &forestTexture;
                    break;
                case SAND:
                    currentTexture = &sandTexture;
                    break;
            }

            // Solo establecer la textura si es diferente de la anterior
            if (sprite.getTexture() != currentTexture && currentTexture != nullptr) {
                sprite.setTexture(*currentTexture);
            }

            sprite.setPosition(static_cast<float>(x) * tileWidth, static_cast<float>(y) * tileHeight);
            window.draw(sprite);
        }
    }
}


