#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include "GlobalVariables.h"
#include "../include/FastNoise/Cpp/FastNoiseLite.h"

using sf::RenderWindow;
using MapParameters::TileType;

extern void generateMap(TileType tile1, TileType tile2, TileType tile3, TileType tile4, FastNoiseLite::NoiseType noiseType);
extern void renderMap(RenderWindow &window);

#endif // MAP_H
