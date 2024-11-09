// main.cpp
#include <SFML/Graphics.hpp>
#include <thread>
#include <vector>
#include <iostream>
#include "Plant.h"
#include "Obstacle.h"
#include "UI.h"
#include "Map.h"
#include <string>
#include "GlobalVariables.h"
#include "Animal.h"
#include "../include/FastNoise/Cpp/FastNoiseLite.h"

using std::cerr, std::cout, std::endl, std::make_unique, std::ref, std::string, std::vector;
using sf::RenderWindow, sf::VideoMode;
using MapParameters::TileType::GRASS, MapParameters::TileType::FOREST, MapParameters::TileType::WATER;

int main() {
    RenderWindow window(VideoMode(800, 800), "2D Ecosystem Simulator");

    generateMap( WATER,  GRASS, FOREST, GRASS, FastNoiseLite::NoiseType_OpenSimplex2);

    Texture tigerTexture;
    if(!tigerTexture.loadFromFile("../public/creatures/carnivores/Tiger.png")) {
        cerr<< "Error loading texture: " << "../public/creatures/carnivores/Tiger.png" << endl;
    }

    animals.emplace_back(make_unique<Animal>("tiger", CARNIVORE, tigerTexture, 99.0f, 99.0f, 2.0f));

    Texture deerTexture;
    if(!deerTexture.loadFromFile("../public/creatures/herbivores/deer.png")) {
        cerr<< "Error loading texture: " << "../public/creatures/herbivores/deer.png" << endl;
    }
    animals.emplace_back(make_unique<Animal>("deer", HERBIVORE, deerTexture, 300.0f, 300.0f, 0.15f));

    vector<animalStructure> animalsReproduce = {
        {"tiger", CARNIVORE, tigerTexture, 300.0f, 300.0f, 2.0f},
        {"deer", HERBIVORE, deerTexture, 300.0f, 300.0f, 0.15f}
    };
    reproduceThread = thread(reproduce, ref(animalsReproduce));

    cout << numberPlants << endl;
    cout << numberObstacles << endl;

    while (window.isOpen()) {

        handleUserInput(window);
        window.clear();


        switch (screen) {
            case 0:
                start(window);
                break;
            case 1:
                renderMap(window);
                for (const auto& obstacle : obstacles) {
                    obstacle->draw(window);
                }

                for (const auto& plant : plants) {
                    plant->draw(window);
                }

                for (size_t i = 0; i < animals.size();) {
                    if (animals[i]->getIsAlive()) {
                        animals[i]->draw(window);
                        ++i;
                    } else {
                        auto iterator = static_cast<vector<unique_ptr<Animal>>::difference_type>(i);
                        animals.erase(animals.begin() + iterator);
                    }
                }
                renderUI(window);
                break;
            case 2:
                window.clear();
                credits(window);
                break;
            case 3:
                window.clear();
                settings(window);
                break;
            case 4:
                window.clear();
                paused(window);
                break;
            default:
                cout << "You entered an invalid choice" << endl;
                break;
        }

        window.display();
    }

    return 0;
}
