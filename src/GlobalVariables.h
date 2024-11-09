#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <mutex>
#include <string>
#include <SFML/Graphics.hpp>

using std::mutex, std::condition_variable, std::string;
using sf::Texture;

namespace MapParameters {
    constexpr int MAP_WIDTH = 100;
    constexpr int MAP_HEIGHT = 100;
    constexpr int TILE_SIZE = 8;

    enum TileType {
        WATER,
        GRASS,
        FOREST,
        SAND
    };

    enum NoiseType
    {
        NoiseType_OpenSimplex2,
        NoiseType_OpenSimplex2S,
        NoiseType_Cellular,
        NoiseType_Perlin,
        NoiseType_ValueCubic,
        NoiseType_Value
    };

    struct Cell {
        TileType type{GRASS};
        bool occupied{false};
        bool hasPlant{false};
        bool hasObstacle{false};
        mutex cellMutex;
    };

    inline Cell map[MAP_HEIGHT][MAP_WIDTH];
};

enum creatureType {
    CARNIVORE,
    HERBIVORE
};

struct animalStructure {
    string name;
    creatureType type{CARNIVORE};
    Texture texture;
    float positionX = 0;
    float positionY = 0;
    float scale = 1;
};

inline int numberPlants = 0;
inline int numberObstacles = 0;

#endif //GLOBALVARIABLES_H
